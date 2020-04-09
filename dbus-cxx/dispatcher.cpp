/***************************************************************************
 *   Copyright (C) 2007,2008,2009 by Rick L. Vinyard, Jr.                  *
 *   rvinyard@cs.nmsu.edu                                                  *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 *                                                                         *
 *   The dbus-cxx library is free software; you can redistribute it and/or *
 *   modify it under the terms of the GNU General Public License           *
 *   version 3 as published by the Free Software Foundation.               *
 *                                                                         *
 *   The dbus-cxx library is distributed in the hope that it will be       *
 *   useful, but WITHOUT ANY WARRANTY; without even the implied warranty   *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   *
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this software. If not see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#include "dispatcher.h"
#include <dbus-cxx/connection.h>
#include <dbus-cxx/error.h>
#include <dbus/dbus.h>
#include <errno.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <deque>
#include <utility>
#include "dbus-cxx-private.h"
#include <sigc++/sigc++.h>
#include "timeout.h"
#include "watch.h"

namespace DBus
{

  Dispatcher::Dispatcher(bool is_running):
      m_running(false),
      m_dispatch_loop_limit(0)
  {
    if( socketpair( AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0, process_fd ) < 0 ){
        SIMPLELOGGER_ERROR( "DBus.Dispatcher", "error creating socket pair" );
        throw ErrorDispatcherInitFailed();
    }

    if ( is_running ) this->start();
  }
  
  std::shared_ptr<Dispatcher> Dispatcher::create( bool is_running )
  {
    return std::shared_ptr<Dispatcher>( new Dispatcher(is_running) );
  }

  Dispatcher::~Dispatcher()
  {
    this->stop();
  }

  std::shared_ptr<Connection> Dispatcher::create_connection(std::string address )
  {
    std::shared_ptr<Connection> conn = Connection::create(address);
    conn->bus_register();
    if ( this->add_connection(conn) ) return conn;
    return std::shared_ptr<Connection>();
  }

  std::shared_ptr<Connection> Dispatcher::create_connection(BusType type)
  {
    std::shared_ptr<Connection> conn = Connection::create(type);
    conn->bus_register();
    if ( this->add_connection(conn) ) return conn;
    return std::shared_ptr<Connection>();
  }
  
  bool Dispatcher::add_connection( std::shared_ptr<Connection> connection )
  {
    if ( not connection or not connection->is_valid() ) return false;
    
    connection->set_dispatching_thread( m_dispatch_thread.get_id() );
    connection->set_dispatching_thread_wakeup_func( sigc::mem_fun(*this, &Dispatcher::wakeup_thread) );
    m_connections.push_back(connection);
    wakeup_thread();
    
    connection->signal_add_watch().connect(sigc::mem_fun(*this, &Dispatcher::on_add_watch));
    connection->signal_remove_watch().connect(sigc::mem_fun(*this, &Dispatcher::on_remove_watch));
    connection->signal_watch_toggled().connect(sigc::mem_fun(*this, &Dispatcher::on_watch_toggled));
    connection->signal_add_timeout().connect(sigc::mem_fun(*this, &Dispatcher::on_add_timeout));
    connection->signal_remove_timeout().connect(sigc::mem_fun(*this, &Dispatcher::on_remove_timeout));
    connection->signal_timeout_toggled().connect(sigc::mem_fun(*this, &Dispatcher::on_timeout_toggled));
    connection->signal_wakeup_main().connect(sigc::bind(sigc::mem_fun(*this, &Dispatcher::on_wakeup_main), connection));
    connection->signal_dispatch_status_changed().connect(sigc::bind(sigc::mem_fun(*this, &Dispatcher::on_dispatch_status_changed), connection));
  
    std::deque<std::shared_ptr<Watch>>::iterator wi;
//     Connection::Timeouts::iterator ti;
    
    while ( connection->unhandled_watches().size() > 0 )
    {
      std::shared_ptr<Watch> w = connection->unhandled_watches().front();
      this->on_add_watch(w);
      connection->remove_unhandled_watch(w);
    }
  
//     while ( connection->unhandled_timeouts().size() > 0 )
//     {
//       std::shared_ptr<Timeout> t = connection->unhandled_timeouts().front();
//       this->on_add_timeout(t);
//       connection->remove_unhandled_timeout(t);
//     }
    return true;
  }

  bool Dispatcher::start()
  {
    if ( m_running ) return false;
    
    m_running = true;
    
    m_dispatch_thread = std::thread( &Dispatcher::dispatch_thread_main, this );
    
    return true;
  }

  bool Dispatcher::stop()
  {
    if ( not m_running ) return false;

    m_running = false;
    
    wakeup_thread();

    if( m_dispatch_thread.joinable() ){
      m_dispatch_thread.join();
    }
    
    return true;
  }

  bool Dispatcher::is_running()
  {
    return m_running;
  }

  void Dispatcher::dispatch_thread_main()
  {
    std::vector<int> fds;

    for( std::shared_ptr<Connection> conn : m_connections ){
        conn->set_dispatching_thread( std::this_thread::get_id() );
    }
    
    while ( m_running ) {
      fds.clear();
      fds.push_back( process_fd[ 1 ] );

      for( std::shared_ptr<Connection> conn : m_connections ){
          if( !conn->is_registered() ){
              SIMPLELOGGER_ERROR( "dbus.Dispatcher", "going to register bus" );
              conn->bus_register();
          }

          fds.push_back( conn->unix_fd() );
      }

      std::tuple<bool,int,std::vector<int>,std::chrono::milliseconds> fdResponse =
              DBus::priv::wait_for_fd_activity( fds, -1 );
      std::vector<int> fdsToRead = std::get<2>( fdResponse );

      if( fdsToRead[ 0 ] == process_fd[ 1 ] ){
          char discard;
          read( process_fd[ 1 ], &discard, sizeof(char) );
      }

      dispatch_connections();
    }
  }

  void Dispatcher::add_read_and_write_watches( std::vector<struct pollfd>* fds ){
      std::lock_guard<std::mutex> watch_lock( m_mutex_watches );

      for( std::pair<int,WatchPair> entry : m_watches_map ){
        struct pollfd newfd;
        newfd.fd = entry.first;
        newfd.events = 0;

        std::shared_ptr<Watch> read = entry.second.read_watch;
        std::shared_ptr<Watch> write = entry.second.write_watch;

        if( read != nullptr && 
            read->is_enabled() ){
          newfd.events |= POLLIN;
        }
        if( write != nullptr &&
            write->is_enabled() ){
          newfd.events |= POLLOUT;
        }
        fds->push_back( newfd );
      }
  }

  void Dispatcher::handle_read_and_write_watches( std::vector<struct pollfd>* fds ){
    std::map<int,WatchPair>::iterator witer;

    for ( const struct pollfd& fd : *fds ){
      std::lock_guard<std::mutex> watch_lock( m_mutex_watches );
      witer = m_watches_map.find( fd.fd );

      if( fd.revents & POLLERR ){
        SIMPLELOGGER_ERROR( "dbus.Dispatcher", "got POLLERR back from fd" );
      }
      if( fd.revents & POLLHUP ){
        SIMPLELOGGER_ERROR( "dbus.Dispatcher", "got POLLHUP back from fd" );
      }
      if( fd.revents & POLLNVAL ){
        SIMPLELOGGER_ERROR( "dbus.Dispatcher", "got POLLNVAL back from fd" );
      }

      if( witer == m_watches_map.end() ) continue;
      
      bool read_good = witer->second.read_watch != nullptr && 
                       witer->second.read_watch->is_enabled();
      bool write_good = witer->second.write_watch != nullptr && 
                        witer->second.write_watch->is_enabled();

      if( (fd.events & POLLIN) && 
          (fd.revents & POLLIN ) &&
          read_good ){
          witer->second.read_watch->handle_read( true, true );
      }else if( (fd.events & POLLOUT) && 
             (fd.revents & POLLOUT ) &&
              write_good ){
          witer->second.write_watch->handle_write( true, true );
      }

    }
  }

  void Dispatcher::dispatch_connections(){
    unsigned int loop_count;
    Connections::iterator ci;

    for ( ci = m_connections.begin(); ci != m_connections.end(); ci++ )
    {
      // If the dispatch loop limit is zero we will loop as long as status is DispatchStatus::DATA_REMAINS
      if ( m_dispatch_loop_limit == 0 )
      {
        SIMPLELOGGER_DEBUG( "dbus.Dispatcher", "Dispatch Status: " << (*ci)->dispatch_status() );
        while ( (*ci)->dispatch_status() == DispatchStatus::DATA_REMAINS )
          (*ci)->dispatch();
      }
      // Otherwise, we will only perform a number of dispatches up to the loop limit
      else
      {
        for ( loop_count = 0; loop_count < m_dispatch_loop_limit; loop_count++ )
        {
          // Make sure we need to dispatch before calling it
          if ( (*ci)->dispatch_status() != DispatchStatus::COMPLETE ) (*ci)->dispatch();

          // Are we done? If so, let's break out of the loop.
          if ( (*ci)->dispatch_status() != DispatchStatus::DATA_REMAINS ) break;
        }

        // If we still have more to process let's set the processing flag to true
        if ( (*ci)->dispatch_status() != DispatchStatus::DATA_REMAINS )
        {
          wakeup_thread();
        }
      }

    }
  }

  bool Dispatcher::on_add_watch(std::shared_ptr<Watch> watch)
  {
    if ( not watch or not watch->is_valid() ){ 
      SIMPLELOGGER_ERROR( "dbus.Dispatcher", "Tried to add invalid watch" );
      return false;
    }
    
    SIMPLELOGGER_DEBUG( "dbus.Dispatcher", "add watch  fd:" << watch->unix_fd() );
  
    std::lock_guard<std::mutex> watch_lock( m_mutex_watches );
    /* If an element does not exist, it will get created with default constructor */
    WatchPair& watchPair = m_watches_map[ watch->unix_fd() ];
    if( watch->is_readable() ){
      watchPair.read_watch = watch;
    }
    if( watch->is_writable() ){
      watchPair.write_watch = watch;
    }

    wakeup_thread();

    return true;
  }

  bool Dispatcher::on_remove_watch(std::shared_ptr<Watch> watch)
  {
    if ( not watch or not watch->is_valid() ){
      SIMPLELOGGER_ERROR( "dbus.Dispatcher", "Tried to remove invalid watch" );
      return false;
    }
    
    SIMPLELOGGER_DEBUG( "dbus.Dispatcher", "remove watch  fd:" << watch->unix_fd() );

    std::lock_guard<std::mutex> watch_lock( m_mutex_watches );
    std::map<int,WatchPair>::iterator it = m_watches_map.find( watch->unix_fd() );
    if( it != m_watches_map.end() ){
      if( watch->is_readable() ){
        it->second.read_watch = nullptr;
      }
      if( watch->is_writable() ){
        it->second.write_watch = nullptr;
      }

      // no watches left - erase entry
      if( it->second.read_watch == nullptr &&
          it->second.write_watch == nullptr ){
        m_watches_map.erase( it );
      }
    }
  
    wakeup_thread();

    return true;
  }

  void Dispatcher::on_watch_toggled(std::shared_ptr<Watch> watch)
  {
    if ( not watch or not watch->is_valid() ) return;

    SIMPLELOGGER_DEBUG( "dbus.Dispatcher", "toggle watch  fd:" << watch->unix_fd() << "  enabled: " << watch->is_enabled() );

    wakeup_thread();

    return;
  }

  bool Dispatcher::on_add_timeout(std::shared_ptr<Timeout> timeout)
  {
    if ( not timeout or not timeout->is_valid() ) return false;
    
    SIMPLELOGGER_DEBUG( "dbus.Dispatcher", "add timeout  enabled:" << timeout->is_enabled() << "  interval: " << timeout->interval() );
    return true;
  }

  bool Dispatcher::on_remove_timeout(std::shared_ptr<Timeout> timeout)
  {
    if ( not timeout or not timeout->is_valid() ) return false;
    
    SIMPLELOGGER_DEBUG( "dbus.Dispatcher", "remove timeout  enabled:" << timeout->is_enabled() << "  interval: " << timeout->interval() );
    return true;
  }

  bool Dispatcher::on_timeout_toggled(std::shared_ptr<Timeout> timeout)
  {
    if ( not timeout or not timeout->is_valid() ) return false;
    
    SIMPLELOGGER_DEBUG( "dbus.Dispatcher", "timeout toggled  enabled:" << timeout->is_enabled() << "  interval: " << timeout->interval() );
    return true;
  }

  void Dispatcher::on_wakeup_main(std::shared_ptr<Connection> conn)
  {
    SIMPLELOGGER_DEBUG( "dbus.Dispatcher", "wakeup main" );
    
    wakeup_thread();
  }

  void Dispatcher::on_dispatch_status_changed(DispatchStatus status, std::shared_ptr<Connection> conn)
  {
    SIMPLELOGGER_DEBUG( "dbus.Dispatcher", "dispatch status changed: " << static_cast<int>( status ) );

    if ( status == DispatchStatus::DATA_REMAINS )
    {
      wakeup_thread();
    }

  }
  
  void Dispatcher::wakeup_thread(){
    char to_write = '0';
    if( write( process_fd[ 0 ], &to_write, sizeof( char ) ) < 0 ){
        SIMPLELOGGER_ERROR( "dbus.Dispatcher", "Can't write to socketpair?!" );
    }
  }
}

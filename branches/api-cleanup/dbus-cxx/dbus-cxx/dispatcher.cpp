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
#include "utility.h"
#include "dispatcher.h"
#include "dbus-cxx-private.h"
#include <iostream>
#include <dbus/dbus.h>
#include <dbus-cxx/error.h>

#include <dbus-cxx/signalmessage.h>

#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>

namespace DBus
{

  Dispatcher::Dispatcher(bool is_running):
      m_running(false),
      m_dispatch_thread(0),
      m_dispatch_loop_limit(0)
  {
    m_responsiveness.tv_sec = 0;
    m_responsiveness.tv_usec = 100000;

    if( socketpair( AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0, process_fd ) < 0 ){
        SIMPLELOGGER_ERROR( "dbus.Dispatcher", "error creating socket pair" );
        throw ErrorDispatcherInitFailed::create();
    }

    if ( is_running ) this->start();
  }
  
  Dispatcher::pointer Dispatcher::create( bool is_running )
  {
    return pointer( new Dispatcher(is_running) );
  }

  Dispatcher::~Dispatcher()
  {
    if ( this->is_running() ) this->stop();
  }

  Connection::pointer Dispatcher::create_connection(DBusConnection * cobj, bool is_private)
  {
    Connection::pointer conn = Connection::create(cobj, is_private);
    if ( this->add_connection(conn) ) return conn;
    return Connection::pointer();
  }

  Connection::pointer Dispatcher::create_connection(BusType type, bool is_private)
  {
    Connection::pointer conn = Connection::create(type, is_private);
    if ( this->add_connection(conn) ) return conn;
    return Connection::pointer();
  }

  Connection::pointer Dispatcher::create_connection(const Connection & other)
  {
    Connection::pointer conn = Connection::create(other);
    if ( this->add_connection(conn) ) return conn;
    return Connection::pointer();
  }
  
  bool Dispatcher::add_connection( Connection::pointer connection )
  {
    if ( not connection or not connection->is_valid() ) return false;
    
    m_connections.push_back(connection);
    
    connection->signal_add_watch().connect(sigc::mem_fun(*this, &Dispatcher::on_add_watch));
    connection->signal_remove_watch().connect(sigc::mem_fun(*this, &Dispatcher::on_remove_watch));
    connection->signal_watch_toggled().connect(sigc::mem_fun(*this, &Dispatcher::on_watch_toggled));
    connection->signal_add_timeout().connect(sigc::mem_fun(*this, &Dispatcher::on_add_timeout));
    connection->signal_remove_timeout().connect(sigc::mem_fun(*this, &Dispatcher::on_remove_timeout));
    connection->signal_timeout_toggled().connect(sigc::mem_fun(*this, &Dispatcher::on_timeout_toggled));
    connection->signal_wakeup_main().connect(sigc::bind(sigc::mem_fun(*this, &Dispatcher::on_wakeup_main), connection));
    connection->signal_dispatch_status_changed().connect(sigc::bind(sigc::mem_fun(*this, &Dispatcher::on_dispatch_status_changed), connection));
  
    Connection::Watches::iterator wi;
//     Connection::Timeouts::iterator ti;
    
    while ( connection->unhandled_watches().size() > 0 )
    {
      Watch::pointer w = connection->unhandled_watches().front();
      this->on_add_watch(w);
      connection->remove_unhandled_watch(w);
    }
  
//     while ( connection->unhandled_timeouts().size() > 0 )
//     {
//       Timeout::pointer t = connection->unhandled_timeouts().front();
//       this->on_add_timeout(t);
//       connection->remove_unhandled_timeout(t);
//     }
    return true;
  }

  bool Dispatcher::start()
  {
    if ( m_running ) return false;
    
    m_running = true;
    
    m_dispatch_thread = new std::thread( &Dispatcher::dispatch_thread_main, this );

    m_dispatch_thread->detach();
    
    return true;
  }

  bool Dispatcher::stop()
  {
    if ( not m_running ) return false;

    m_running = false;
    
    wakeup_thread();

    if( m_dispatch_thread->joinable() ){
      m_dispatch_thread->join();
    }
    
    delete m_dispatch_thread;
    m_dispatch_thread = nullptr;
    
    return true;
  }

  bool Dispatcher::is_running()
  {
    return m_running;
  }

  void Dispatcher::dispatch_thread_main()
  {
    int selresult;
    std::vector<struct pollfd> fds;
    struct pollfd thread_wakeup;

    thread_wakeup.fd = process_fd[ 1 ];
    thread_wakeup.events = POLLIN;
    
    while ( m_running ) {
      fds.clear();
      fds.push_back( thread_wakeup );

      add_read_and_write_watches( &fds );

      // wait forever until some file descriptor has events
      selresult = poll( fds.data(), fds.size(), -1 );

      // Oops, poll had a serious error
      if ( selresult == -1 && errno == EINTR ){
        //if we were interrupted, continue on
        continue;
      } else if( selresult == -1 ) throw(errno);

      // Discard data from process_fd if we were woken up by that
      if( fds[ 0 ].revents & POLLIN ){
        char discard;
        read( process_fd[ 1 ], &discard, sizeof(char) );
      }

      handle_read_and_write_watches( &fds );

      dispatch_connections();
    }
  }

  void Dispatcher::add_read_and_write_watches( std::vector<struct pollfd>* fds ){
      std::lock_guard<std::mutex> read_lock( m_mutex_read_watches );
      std::lock_guard<std::mutex> write_lock( m_mutex_write_watches );

      for( int i : m_enabled_read_fds ){
        struct pollfd newfd;
        newfd.fd = i;
        newfd.events = POLLIN;
        fds->push_back( newfd );
      }
 
      for( int i : m_enabled_write_fds ){
        struct pollfd newfd;
        newfd.fd = i;
        newfd.events = POLLOUT;
        fds->push_back( newfd );
      }
  }

  void Dispatcher::handle_read_and_write_watches( std::vector<struct pollfd>* fds ){
    std::map<int,Watch::pointer>::iterator witer;

    for ( const struct pollfd& fd : *fds ){
      if( (fd.events == POLLIN) && 
          (fd.revents & POLLIN ) ){
          std::lock_guard<std::mutex> read_lock( m_mutex_read_watches );
          witer = m_read_watches.find( fd.fd );
          if( witer != m_read_watches.end() ){
            witer->second->handle_read();
          }
      }else if( (fd.events == POLLOUT) && 
             (fd.revents & POLLOUT ) ){
          std::lock_guard<std::mutex> write_lock( m_mutex_write_watches );
          witer = m_write_watches.find( fd.fd );
          if( witer != m_write_watches.end() ){
            witer->second->handle_write();
          }
      }

    }
  }

  void Dispatcher::dispatch_connections(){
    unsigned int loop_count;
    Connections::iterator ci;

    for ( ci = m_connections.begin(); ci != m_connections.end(); ci++ )
    {
      // If the dispatch loop limit is zero we will loop as long as status is DISPATCH_DATA_REMAINS
      if ( m_dispatch_loop_limit == 0 )
      {
        SIMPLELOGGER_DEBUG( "dbus.Dispatcher", "Dispatch Status: " << (*ci)->dispatch_status() << "   Hints: DISPATCH_DATA_REMAINS=" << DISPATCH_DATA_REMAINS );
        while ( (*ci)->dispatch_status() == DISPATCH_DATA_REMAINS )
          (*ci)->dispatch();
      }
      // Otherwise, we will only perform a number of dispatches up to the loop limit
      else
      {
        for ( loop_count = 0; loop_count < m_dispatch_loop_limit; loop_count++ )
        {
          // Make sure we need to dispatch before calling it
          if ( (*ci)->dispatch_status() != DISPATCH_COMPLETE ) (*ci)->dispatch();

          // Are we done? If so, let's break out of the loop.
          if ( (*ci)->dispatch_status() != DISPATCH_DATA_REMAINS ) break;
        }

        // If we still have more to process let's set the processing flag to true
        if ( (*ci)->dispatch_status() != DISPATCH_DATA_REMAINS )
        {
          wakeup_thread();
        }
      }

    }
  }

  bool Dispatcher::on_add_watch(Watch::pointer watch)
  {
    if ( not watch or not watch->is_valid() ) return false;
    
    SIMPLELOGGER_DEBUG( "dbus.Dispatcher", "add watch  fd:" << watch->unix_fd() );
  
    if ( watch->is_readable() )
    {
      std::lock_guard<std::mutex> read_lock( m_mutex_read_watches );

      m_read_watches[watch->unix_fd()] = watch;
      if( watch->is_enabled() ) m_enabled_read_fds.insert( watch->unix_fd() );
    }
    
    if ( watch->is_writable() )
    {
      std::lock_guard<std::mutex> read_lock( m_mutex_write_watches );

      m_write_watches[watch->unix_fd()] = watch;
      if( watch->is_enabled() ) m_enabled_write_fds.insert( watch->unix_fd() );
    }

    wakeup_thread();

    return true;
  }

  bool Dispatcher::on_remove_watch(Watch::pointer watch)
  {
    if ( not watch or not watch->is_valid() ) return false;
    
    //TODO has this ever actually worked??
    SIMPLELOGGER_DEBUG( "dbus.Dispatcher", "remove watch  fd:" << watch->unix_fd() );
  
    if ( watch->is_readable() )
    {
      std::lock_guard<std::mutex> read_lock( m_mutex_read_watches );

      m_read_watches[watch->unix_fd()] = watch;
      if( watch->is_enabled() ) m_enabled_read_fds.insert( watch->unix_fd() );
    }
    
    if ( watch->is_writable() )
    {
      std::lock_guard<std::mutex> read_lock( m_mutex_write_watches );

      m_write_watches[watch->unix_fd()] = watch;
      if( watch->is_enabled() ) m_enabled_write_fds.insert( watch->unix_fd() );
    }

    wakeup_thread();

    return true;
  }

  bool Dispatcher::on_watch_toggled(Watch::pointer watch)
  {
    if ( not watch or not watch->is_valid() ) return false;
    
    std::set<int>::iterator i;
    
    SIMPLELOGGER_DEBUG( "dbus.Dispatcher", "toggle watch  fd:" << watch->unix_fd() << "  enabled: " << watch->is_enabled() );

    if ( watch->is_enabled() )
    {
      if ( watch->is_readable() )
      {
        std::lock_guard<std::mutex> read_lock( m_mutex_read_watches );
        i = m_enabled_read_fds.find(watch->unix_fd());
        if ( i != m_enabled_read_fds.end() )
        {
          return true;
        }
        m_enabled_read_fds.insert( watch->unix_fd() );
      }
      else if ( watch->is_writable() )
      {
        std::lock_guard<std::mutex> read_lock( m_mutex_write_watches );
        i = m_enabled_write_fds.find(watch->unix_fd());
        if ( i != m_enabled_write_fds.end() )
        {
          return true;
        }
        m_enabled_write_fds.insert( watch->unix_fd() );
      }
    }
    else
    {
      if ( watch->is_readable() )
      {
        std::lock_guard<std::mutex> read_lock( m_mutex_read_watches );
        i = m_enabled_read_fds.find(watch->unix_fd());
        if ( i == m_enabled_read_fds.end() )
        {
          return true;
        }
        m_enabled_read_fds.erase( i );
      }
      else if ( watch->is_writable() )
      {
        std::lock_guard<std::mutex> read_lock( m_mutex_write_watches );
        i = m_enabled_write_fds.find(watch->unix_fd());
        if ( i == m_enabled_write_fds.end() )
        {
          return true;
        }
        m_enabled_write_fds.erase( i );
      }
    }

    wakeup_thread();

    return true;
  }

  bool Dispatcher::on_add_timeout(Timeout::pointer timeout)
  {
    if ( not timeout or not timeout->is_valid() ) return false;
    
    SIMPLELOGGER_DEBUG( "dbus.Dispatcher", "add timeout  enabled:" << timeout->is_enabled() << "  interval: " << timeout->interval() );
    return true;
  }

  bool Dispatcher::on_remove_timeout(Timeout::pointer timeout)
  {
    if ( not timeout or not timeout->is_valid() ) return false;
    
    SIMPLELOGGER_DEBUG( "dbus.Dispatcher", "remove timeout  enabled:" << timeout->is_enabled() << "  interval: " << timeout->interval() );
    return true;
  }

  bool Dispatcher::on_timeout_toggled(Timeout::pointer timeout)
  {
    if ( not timeout or not timeout->is_valid() ) return false;
    
    SIMPLELOGGER_DEBUG( "dbus.Dispatcher", "timeout toggled  enabled:" << timeout->is_enabled() << "  interval: " << timeout->interval() );
    return true;
  }

  void Dispatcher::on_wakeup_main(Connection::pointer conn)
  {
    SIMPLELOGGER_DEBUG( "dbus.Dispatcher", "wakeup main" );
    
    wakeup_thread();
  }

  void Dispatcher::on_dispatch_status_changed(DispatchStatus status, Connection::pointer conn)
  {
    SIMPLELOGGER_DEBUG( "dbus.Dispatcher", "dispatch status changed: " << status );

    if ( status == DISPATCH_DATA_REMAINS )
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

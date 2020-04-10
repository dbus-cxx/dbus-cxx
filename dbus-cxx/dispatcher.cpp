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
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <deque>
#include <utility>
#include "dbus-cxx-private.h"
#include <sigc++/sigc++.h>

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
    
    connection->signal_dispatch_status_changed().connect(sigc::bind(sigc::mem_fun(*this, &Dispatcher::on_dispatch_status_changed), connection));
  
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

  void Dispatcher::dispatch_connections(){
    uint32_t loop_limit = m_dispatch_loop_limit;
    if( loop_limit == 0 ){
        loop_limit = UINT32_MAX;
    }

    SIMPLELOGGER_DEBUG( "dbus.Dispatcher", "Dispatching connections" );

    for ( std::shared_ptr<Connection> conn : m_connections )
    {
        for( uint32_t x = 0; x < loop_limit; x++ ){
            DispatchStatus stat = conn->dispatch();
            if( stat == DispatchStatus::COMPLETE ){
                break;
            }
        }

        if( conn->dispatch_status() != DispatchStatus::COMPLETE ){
            wakeup_thread();
        }
    }

    SIMPLELOGGER_DEBUG( "dbus.Dispatcher", "done dispatching" );
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

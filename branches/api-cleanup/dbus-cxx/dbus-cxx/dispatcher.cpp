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
#define DBUSCXX_INTERNAL
#include "utility.h"
#include "dispatcher.h"
#include <iostream>
#include <dbus/dbus.h>

#include <dbus-cxx/signalmessage.h>

#include <poll.h>
#include <unistd.h>
#include <errno.h>

namespace DBus
{

  Dispatcher::Dispatcher(bool is_running):
      m_running(false),
      m_dispatch_thread(0),
      m_watch_thread(0),
      m_dispatch_loop_limit(0),
      m_initiate_processing(false)
  {
    m_responsiveness.tv_sec = 0;
    m_responsiveness.tv_usec = 100000;

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
    m_watch_thread = new std::thread( &Dispatcher::watch_thread_main, this );

    m_dispatch_thread->detach();
    m_watch_thread->detach();
    
    return true;
  }

  bool Dispatcher::stop()
  {
    if ( not m_running ) return false;

    m_running = false;

    // The dispach thread may be sleeping, let's wake it up so it will terminate
    // We'll wrap this in mutexes to ensure that there isn't a race condition in waking up
    {
      std::lock_guard<std::mutex> lock(m_mutex_initiate_processing);
      m_cond_initiate_processing.notify_all();
    }
    
    if( m_dispatch_thread->joinable() ){
      m_dispatch_thread->join();
    }
    if( m_watch_thread->joinable() ){
      m_watch_thread->join();
    }
    
    delete m_dispatch_thread;
    delete m_watch_thread;
    m_dispatch_thread = 0;
    m_watch_thread = 0;
    
    return true;
  }

  bool Dispatcher::is_running()
  {
    return m_running;
  }
  
  const struct timeval & Dispatcher::responsiveness()
  {
    return m_responsiveness;
  }

  void Dispatcher::set_responsiveness(const struct timeval & r)
  {
    m_responsiveness = r;
  }

  void Dispatcher::set_responsiveness(time_t sec, suseconds_t usec)
  {
    m_responsiveness.tv_sec = sec;
    m_responsiveness.tv_usec = usec;
  }

  void Dispatcher::dispatch_thread_main()
  {
    Connections::iterator ci;
    unsigned int loop_count;

    // Setting this guarantees that we will not stop on the condition
    m_initiate_processing = true;
    
    while ( m_running ) {

      // Here, we lock the mutex before checking m_initiate_processing
      // If something has changed m_initiate_processing there is no reason to wait on the condition
      // since we have some work to do
      {
        std::unique_lock<std::mutex> lock( m_mutex_initiate_processing );
      
        if ( not m_initiate_processing ){
          // If we don't have any work to do we will wait here for someone to wake us up
          m_cond_initiate_processing.wait( lock );
        }

        // At this point we have one of two situations: either there is work for us to do, or we were
        // sleeping waiting for the condition to wake us up.
        // It's possible that we were woken up because we are shutting down processing.
        if ( not m_running )
        {
          break;
        }

        m_initiate_processing = false;
      }
      
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
            std::lock_guard<std::mutex> lock( m_mutex_initiate_processing );
            m_initiate_processing = true;
          }
        }

      }
      
    }
  }

  void Dispatcher::watch_thread_main()
  {
    int selresult;
    std::map<int,Watch::pointer>::iterator witer;
    int timeout;
    std::vector<struct pollfd> fds;

    // HACK this is to alleviate a race condition between the read/write handling and dispatching
    bool need_initiate_processing_hack = false;
    
    while ( m_running )
    {
      // Lock the read/write/exception fd sets so that the sets for this iteration can be created
      {
        std::lock_guard<std::mutex> read_lock( m_mutex_read_watches );
        std::lock_guard<std::mutex> write_lock( m_mutex_write_watches );
        fds.clear();

        for( int i : m_enabled_read_fds ){
          struct pollfd newfd;
          newfd.fd = i;
          newfd.events = POLLIN;
          fds.push_back( newfd );
        }
 
        for( int i : m_enabled_write_fds ){
          struct pollfd newfd;
          newfd.fd = i;
          newfd.events = POLLOUT;
          fds.push_back( newfd );
        }
      }

      // If we have no watches we'll sleep for the timeout period and continue the loop
      if ( fds.empty() )
      {
        sleep( m_responsiveness.tv_sec );
        usleep( m_responsiveness.tv_usec );
        continue;
      }
      
      // Now we'll wait in the select call for activity or a timeout
      timeout = (m_responsiveness.tv_sec * 1000) + (m_responsiveness.tv_usec / 1000 );
      selresult = poll( fds.data(), fds.size(), timeout );

      // If we timed out we'll loop back and see if we should still be running
      if ( selresult == 0 ) continue;

      // Oops, select had a serious error
      if ( selresult == -1 && errno == EINTR ){
        //if we were interrupted, continue on
        continue;
      } else if( selresult == -1 ) throw(errno);

      // If we made it here we have some activity we need to handle
      //
      // We'll start by updating the fds that are ready for reading and add them to the set of read fds to handle
      for ( const struct pollfd& fd : fds ){
          bool useit = false;
          bool do_write;
          Watch::pointer w;

          if( (fd.events == POLLIN) && 
              (fd.revents & POLLIN ) ){
            useit = true;
            do_write = false;
          }else if( (fd.events == POLLOUT) && 
              (fd.revents & POLLOUT ) ){
            useit = true;
            do_write = true;
          }

          if( !useit ){ 
            continue;
          }

          if( do_write == false ){
            std::lock_guard<std::mutex> read_lock( m_mutex_read_watches );
            witer = m_read_watches.find( fd.fd );
            if( witer != m_read_watches.end() ){
              w = witer->second;
            }else{
              SIMPLELOGGER_ERROR( "dbus.Dispatcher", "SHOULD NOT GET HERE" );
            }
          }else{
            std::lock_guard<std::mutex> write_lock( m_mutex_write_watches );
            witer = m_write_watches.find( fd.fd );
            if( witer != m_write_watches.end() ){
              w = witer->second;
            }else{
              SIMPLELOGGER_ERROR( "dbus.Dispatcher", "SHOULD NOT GET HERE" );
            }
          }
  
          if( w ){
            if( do_write ){ w->handle_write(); } else { w->handle_read(); }
            // HACK this is to alleviate a race condition between the read handling and dispatching
            need_initiate_processing_hack = true;
          }
      }
      
      // HACK This is to alleviate a race condition between read/write handling and
      // dispatching. The problem occurs when the read/write handler acquires the IO
      // lock and the dispatcher silently queues a response.
      // This should guarantee that the dispatch loop wakes up after reading/writing
      // and if nothing is to be done will go back to sleep.
      if ( need_initiate_processing_hack )
      {
        // We'll lock the initiate processing mutex before setting the initiate processing variable
        // to true and signalling the initiate processing condition
        std::lock_guard<std::mutex> read_lock( m_mutex_initiate_processing );
        m_initiate_processing = true;
        need_initiate_processing_hack = false;
        m_cond_initiate_processing.notify_all();
      }
      
    }
  }

  bool Dispatcher::on_add_watch(Watch::pointer watch)
  {
    if ( not watch or not watch->is_valid() ) return false;
    
    SIMPLELOGGER_DEBUG( "dbus.Dispatcher", "add watch  fd:" << watch->unix_fd() << "  readable: " << watch->is_readable() << "  writable: " << watch->is_writable() );

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
    
    // We'll lock the initiate processing mutex before setting the initiate processing variable
    // to true and signalling the initiate processing condition
    std::lock_guard<std::mutex> read_lock( m_mutex_initiate_processing );
    m_initiate_processing = true;
    m_cond_initiate_processing.notify_all();
  }

  void Dispatcher::on_dispatch_status_changed(DispatchStatus status, Connection::pointer conn)
  {
    SIMPLELOGGER_DEBUG( "dbus.Dispatcher", "dispatch status changed: " << status );

    if ( status == DISPATCH_DATA_REMAINS )
    {
      // We'll lock the initiate processing mutex before setting the initiate processing variable
      // to true and signalling the initiate processing condition
      std::lock_guard<std::mutex> read_lock( m_mutex_initiate_processing );
      m_initiate_processing = true;
      m_cond_initiate_processing.notify_all();
    }

  }

  void * Dispatcher::proxy_dispatch_thread_main(void *arg)
  {
    Dispatcher* dispatcher = static_cast<Dispatcher*>(arg);
    dispatcher->dispatch_thread_main();
    return NULL;
  }

  void * Dispatcher::proxy_watch_thread_main(void *arg)
  {
    Dispatcher* dispatcher = static_cast<Dispatcher*>(arg);
    dispatcher->watch_thread_main();
    return NULL;
  }
  
}

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
#include <iostream>
#include <dbus/dbus.h>

#include <dbus-cxx/signalmessage.h>

#include <sys/select.h>
#include <unistd.h>
#include <errno.h>

namespace DBus
{

  Dispatcher::Dispatcher(bool is_running):
      m_running(false),
      m_dispatch_thread(0),
      m_watch_thread(0),
      m_maximum_read_fd(-1),
      m_maximum_write_fd(-1),
      m_dispatch_loop_limit(0),
      m_initiate_processing(false)
  {
    pthread_mutex_init( &m_mutex_read_watches, NULL );
    pthread_mutex_init( &m_mutex_write_watches, NULL );
    
    pthread_cond_init( &m_cond_initiate_processing, NULL );
    pthread_mutex_init( &m_mutex_initiate_processing, NULL );
    
    FD_ZERO( &m_read_fd_set );
    FD_ZERO( &m_write_fd_set );
    FD_ZERO( &m_exception_fd_set );
    
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
    pthread_mutex_destroy( &m_mutex_read_watches );
    pthread_mutex_destroy( &m_mutex_write_watches );
    pthread_cond_destroy( &m_cond_initiate_processing );
    pthread_mutex_destroy( &m_mutex_initiate_processing );
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
    
    pthread_create( &m_dispatch_thread, NULL, Dispatcher::proxy_dispatch_thread_main, this );
    pthread_create( &m_watch_thread, NULL, Dispatcher::proxy_watch_thread_main, this );
    
    return true;
  }

  bool Dispatcher::stop()
  {
    if ( not m_running ) return false;

    m_running = false;

    // The dispach thread may be sleeping, let's wake it up so it will terminate
    // We'll wrap this in mutexes to ensure that there isn't a race condition in waking up
    pthread_mutex_lock( &m_mutex_initiate_processing );
    pthread_cond_broadcast( &m_cond_initiate_processing );
    pthread_mutex_unlock( &m_mutex_initiate_processing );
    
    pthread_join( m_dispatch_thread, NULL );
    pthread_join( m_watch_thread, NULL );
    
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

  void Dispatcher::build_read_fd_set()
  {
    std::set<int>::iterator i;
    pthread_mutex_lock( &m_mutex_read_watches );
    FD_ZERO(&m_read_fd_set);
    m_maximum_read_fd = -1;
    for ( i = m_enabled_read_fds.begin(); i != m_enabled_read_fds.end(); i++ )
    {
      FD_SET( *i, &m_read_fd_set );
      m_maximum_read_fd = std::max( m_maximum_read_fd, *i + 1 );
    }
    pthread_mutex_unlock( &m_mutex_read_watches );
  }

  void Dispatcher::build_write_fd_set()
  {
    std::set<int>::iterator i;
    pthread_mutex_lock( &m_mutex_write_watches );
    FD_ZERO(&m_write_fd_set);
    m_maximum_write_fd = -1;
    for ( i = m_enabled_write_fds.begin(); i != m_enabled_write_fds.end(); i++ )
    {
      FD_SET( *i, &m_write_fd_set );
      m_maximum_write_fd = std::max( m_maximum_write_fd, *i + 1 );
    }
    pthread_mutex_unlock( &m_mutex_write_watches );
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
      pthread_mutex_lock( &m_mutex_initiate_processing );
      
      if ( not m_initiate_processing )
        // If we don't have any work to do we will wait here for someone to wake us up
        pthread_cond_wait( &m_cond_initiate_processing, &m_mutex_initiate_processing );

      // At this point we have one of two situations: either there is work for us to do, or we were
      // sleeping waiting for the condition to wake us up.
      // It's possible that we were woken up because we are shutting down processing.
      // If that's the case we need out of the while loop, but we also need to unlock the mutex
      if ( not m_running )
      {
        pthread_mutex_unlock( &m_mutex_initiate_processing );
        break;
      }

      m_initiate_processing = false;
      pthread_mutex_unlock( &m_mutex_initiate_processing );
      
      for ( ci = m_connections.begin(); ci != m_connections.end(); ci++ )
      {
        // If the dispatch loop limit is zero we will loop as long as status is DISPATCH_DATA_REMAINS
        if ( m_dispatch_loop_limit == 0 )
        {
          DBUS_CXX_DEBUG( "Dispatch Status: " << (*ci)->dispatch_status() << "   Hints: DISPATCH_DATA_REMAINS=" << DISPATCH_DATA_REMAINS );
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
            pthread_mutex_lock( &m_mutex_initiate_processing );
            m_initiate_processing = true;
            pthread_mutex_unlock( &m_mutex_initiate_processing );
          }
        }

      }
      
    }
  }

  void Dispatcher::watch_thread_main()
  {
    fd_set read_fds, write_fds;
    int max_fd;
    int selresult;
    std::set<int>::iterator fditer;
    std::map<int,Watch::pointer>::iterator witer;
    struct timeval timeout;
    bool no_watches;
    
    // HACK this is to alleviate a race condition between the read/write handling and dispatching
    bool need_initiate_processing_hack = false;
    
    while ( m_running )
    {
      // Lock the read/write/exception fd sets so that the sets for this iteration can be created
      // TODO improve this by caching the max fd and only updating the cached version as fd's are added/removed/toggled
      pthread_mutex_lock( &m_mutex_read_watches );
      pthread_mutex_lock( &m_mutex_write_watches );
      read_fds = m_read_fd_set;
      write_fds = m_write_fd_set;
      no_watches = m_enabled_read_fds.empty() and m_enabled_write_fds.empty();
      // We can go ahead and unlock here since we have copies of the fds and the maximums from the read/write sets
      pthread_mutex_unlock( &m_mutex_read_watches );
      pthread_mutex_unlock( &m_mutex_write_watches );

      // Get the max fd from the max between the read and write sets
      max_fd = std::max(m_maximum_read_fd, m_maximum_write_fd);

      // If we have no watches we'll sleep for the timeout period and continue the loop
      if ( no_watches )
      {
        sleep( m_responsiveness.tv_sec );
        usleep( m_responsiveness.tv_usec );
        continue;
      }
      
      // Now we'll wait in the select call for activity or a timeout
      timeout = m_responsiveness;
      selresult = select(max_fd, &read_fds, &write_fds, NULL, &timeout);

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
      for ( fditer = m_enabled_read_fds.begin(); fditer != m_enabled_read_fds.end(); fditer++ )
      {
        if ( FD_ISSET( *fditer, &read_fds ) )
        {
          pthread_mutex_lock( &m_mutex_read_watches );
          witer = m_read_watches.find(*fditer);
          if ( witer != m_read_watches.end() ) {
            Watch::pointer w = witer->second;
            pthread_mutex_unlock( &m_mutex_read_watches );
            w->handle_read();
            // HACK this is to alleviate a race condition between the read handling and dispatching
            need_initiate_processing_hack = true;
          }
          else {
            pthread_mutex_unlock( &m_mutex_read_watches );
          }
        }
      }

      // Now we'll check the fds that are ready for writing and add them to the set of write fds to handle
      for ( fditer = m_enabled_write_fds.begin(); fditer != m_enabled_write_fds.end(); fditer++ )
      {
        if ( FD_ISSET( *fditer, &write_fds ) )
        {
          pthread_mutex_lock( &m_mutex_write_watches );
          witer = m_write_watches.find(*fditer);
          if ( witer != m_write_watches.end() ) {
            Watch::pointer w = witer->second;
            pthread_mutex_unlock( &m_mutex_write_watches );
            w->handle_write();
            // HACK this is to alleviate a race condition between the write handling and dispatching
            need_initiate_processing_hack = true;
          }
          else {
            pthread_mutex_unlock( &m_mutex_write_watches );
          }
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
        pthread_mutex_lock( &m_mutex_initiate_processing );
        m_initiate_processing = true;
        need_initiate_processing_hack = false;
        pthread_cond_broadcast( &m_cond_initiate_processing );
        pthread_mutex_unlock( &m_mutex_initiate_processing );
      }
      
    }
  }

  bool Dispatcher::on_add_watch(Watch::pointer watch)
  {
    if ( not watch or not watch->is_valid() ) return false;
    
    DBUS_CXX_DEBUG( "Dispatcher::on_add_watch  fd:" << watch->unix_fd() << "  readable: " << watch->is_readable() << "  writable: " << watch->is_writable() );

    if ( watch->is_readable() )
    {
      pthread_mutex_lock( &m_mutex_read_watches );

      m_read_watches[watch->unix_fd()] = watch;
      
      if ( watch->is_enabled() )
      {
        FD_SET( watch->unix_fd(), &m_read_fd_set );
        m_enabled_read_fds.insert(watch->unix_fd());
        m_maximum_read_fd = std::max( m_maximum_read_fd, watch->unix_fd() + 1 );
      }

      pthread_mutex_unlock( &m_mutex_read_watches );
    }
    
    if ( watch->is_writable() )
    {
      pthread_mutex_lock( &m_mutex_write_watches );

      m_write_watches[watch->unix_fd()] = watch;
      
      if ( watch->is_enabled() )
      {
        FD_SET( watch->unix_fd(), &m_write_fd_set );
        m_enabled_write_fds.insert(watch->unix_fd());
        m_maximum_write_fd = std::max( m_maximum_write_fd, watch->unix_fd() + 1 );
      }
      
      pthread_mutex_unlock( &m_mutex_write_watches );
    }
    
    return true;
  }

  bool Dispatcher::on_remove_watch(Watch::pointer watch)
  {
    if ( not watch or not watch->is_valid() ) return false;
    
    DBUS_CXX_DEBUG( "Dispatcher::on_remove_watch  fd:" << watch->unix_fd() );
  
    if ( watch->is_readable() )
    {
      pthread_mutex_lock( &m_mutex_read_watches );

      m_read_watches[watch->unix_fd()] = watch;
      
      if ( watch->is_enabled() )
      {
        FD_SET( watch->unix_fd(), &m_read_fd_set );
        m_enabled_read_fds.insert(watch->unix_fd());
        m_maximum_read_fd = std::max( m_maximum_read_fd, watch->unix_fd() + 1 );
      }

      pthread_mutex_unlock( &m_mutex_read_watches );
    }
    
    if ( watch->is_writable() )
    {
      pthread_mutex_lock( &m_mutex_write_watches );

      m_write_watches[watch->unix_fd()] = watch;
      
      if ( watch->is_enabled() )
      {
        FD_SET( watch->unix_fd(), &m_write_fd_set );
        m_enabled_write_fds.insert(watch->unix_fd());
        m_maximum_write_fd = std::max( m_maximum_write_fd, watch->unix_fd() + 1 );
      }
      
      pthread_mutex_unlock( &m_mutex_write_watches );
    }

    return true;
  }

  bool Dispatcher::on_watch_toggled(Watch::pointer watch)
  {
    if ( not watch or not watch->is_valid() ) return false;
    
    std::set<int>::iterator i;
    
    DBUS_CXX_DEBUG( "Dispatcher::on_watch_toggled  fd:" << watch->unix_fd() << "  enabled: " << watch->is_enabled() );

    if ( watch->is_enabled() )
    {
      if ( watch->is_readable() )
      {
        pthread_mutex_lock(&m_mutex_read_watches);
        i = m_enabled_read_fds.find(watch->unix_fd());
        if ( i != m_enabled_read_fds.end() )
        {
          pthread_mutex_unlock(&m_mutex_read_watches);
          return true;
        }
        m_enabled_read_fds.insert( watch->unix_fd() );
        FD_SET( watch->unix_fd(), &m_read_fd_set );
        pthread_mutex_unlock(&m_mutex_read_watches);
      }
      else if ( watch->is_writable() )
      {
        pthread_mutex_lock(&m_mutex_write_watches);
        i = m_enabled_write_fds.find(watch->unix_fd());
        if ( i != m_enabled_write_fds.end() )
        {
          pthread_mutex_unlock(&m_mutex_write_watches);
          return true;
        }
        m_enabled_write_fds.insert( watch->unix_fd() );
        FD_SET( watch->unix_fd(), &m_write_fd_set );
        pthread_mutex_unlock(&m_mutex_write_watches);
      }
    }
    else
    {
      if ( watch->is_readable() )
      {
        pthread_mutex_lock(&m_mutex_read_watches);
        i = m_enabled_read_fds.find(watch->unix_fd());
        if ( i == m_enabled_read_fds.end() )
        {
          pthread_mutex_unlock(&m_mutex_read_watches);
          return true;
        }
        m_enabled_read_fds.erase( i );
        FD_CLR( watch->unix_fd(), &m_read_fd_set );
        pthread_mutex_unlock(&m_mutex_read_watches);
      }
      else if ( watch->is_writable() )
      {
        pthread_mutex_lock(&m_mutex_write_watches);
        i = m_enabled_write_fds.find(watch->unix_fd());
        if ( i == m_enabled_write_fds.end() )
        {
          pthread_mutex_unlock(&m_mutex_write_watches);
          return true;
        }
        m_enabled_write_fds.erase( i );
        FD_CLR( watch->unix_fd(), &m_write_fd_set );
        pthread_mutex_unlock(&m_mutex_write_watches);
      }
    }

    return true;
  }

  bool Dispatcher::on_add_timeout(Timeout::pointer timeout)
  {
    if ( not timeout or not timeout->is_valid() ) return false;
    
    DBUS_CXX_DEBUG( "Dispatcher::on_add_timeout  enabled:" << timeout->is_enabled() << "  interval: " << timeout->interval() );
    return true;
  }

  bool Dispatcher::on_remove_timeout(Timeout::pointer timeout)
  {
    if ( not timeout or not timeout->is_valid() ) return false;
    
    DBUS_CXX_DEBUG( "Dispatcher::on_remove_timeout  enabled:" << timeout->is_enabled() << "  interval: " << timeout->interval() );
    return true;
  }

  bool Dispatcher::on_timeout_toggled(Timeout::pointer timeout)
  {
    if ( not timeout or not timeout->is_valid() ) return false;
    
    DBUS_CXX_DEBUG( "Dispatcher::on_timeout_toggled  enabled:" << timeout->is_enabled() << "  interval: " << timeout->interval() );
    return true;
  }

  void Dispatcher::on_wakeup_main(Connection::pointer conn)
  {
    DBUS_CXX_DEBUG( "Dispatcher::on_wakeup_main" );
    
    // We'll lock the initiate processing mutex before setting the initiate processing variable
    // to true and signalling the initiate processing condition
    pthread_mutex_lock( &m_mutex_initiate_processing );
    m_initiate_processing = true;
    pthread_cond_broadcast( &m_cond_initiate_processing );
    pthread_mutex_unlock( &m_mutex_initiate_processing );
  }

  void Dispatcher::on_dispatch_status_changed(DispatchStatus status, Connection::pointer conn)
  {
    DBUS_CXX_DEBUG( "Dispatcher::on_dispatch_status_changed: " << status );

    if ( status == DISPATCH_DATA_REMAINS )
    {
      // We'll lock the initiate processing mutex before setting the initiate processing variable
      // to true and signalling the initiate processing condition
      pthread_mutex_lock( &m_mutex_initiate_processing );
      m_initiate_processing = true;
      pthread_cond_broadcast( &m_cond_initiate_processing );
      pthread_mutex_unlock( &m_mutex_initiate_processing );
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

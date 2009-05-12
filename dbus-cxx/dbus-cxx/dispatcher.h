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
#ifndef __DBUSXX_DISPATCHER_H
#define __DBUSXX_DISPATCHER_H

#include <map>
#include <list>
#include <set>

#include <sys/select.h>

#include <dbus/dbus.h>
#include <dbus-cxx/connection.h>
#include <dbus-cxx/watch.h>
#include <dbus-cxx/timeout.h>

namespace DBus
{

  /**
   * Handles multi-threaded dispatching of one or more connections.
   *
   * This class provides a way to handle multiple connections with one set of
   * dispatching threads.
   *
   * This dispatcher creates two threads, one to watch I/O file descriptors
   * for activity and the other to handle message dispatching.
   *
   * @ingroup core
   *
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class Dispatcher
  {
    public:

      Dispatcher();

      virtual ~Dispatcher();

      /** @name Managing Connections */
      //@{

      Connection::pointer create_connection( DBusConnection* cobj = NULL, bool is_private=false );

      Connection::pointer create_connection( BusType type, bool is_private=false );

      Connection::pointer create_connection( const Connection& other );

      bool add_connection( Connection::pointer connection );

      //@}

      bool start();
      
      bool stop();
      
      bool is_running();

      const struct timeval& responsiveness();
      
      void set_responsiveness( const struct timeval& r );
      
      void set_responsiveness( time_t sec, suseconds_t usec );
      
    protected:
      
      typedef std::list<Connection::pointer> Connections;
      Connections m_connections;
      
      bool m_running;
      
      pthread_t m_dispatch_thread;
      pthread_t m_watch_thread;

      pthread_mutex_t m_mutex_read_watches;
      std::map<int, Watch::pointer> m_read_watches;
      std::set<int> m_enabled_read_fds;
      fd_set m_read_fd_set;
      int m_maximum_read_fd;

      void build_read_fd_set();
      
      pthread_mutex_t m_mutex_write_watches;
      std::map<int, Watch::pointer> m_write_watches;
      std::set<int> m_enabled_write_fds;
      fd_set m_write_fd_set;
      int m_maximum_write_fd;

      void build_write_fd_set();
      
      fd_set m_exception_fd_set;
      
      pthread_mutex_t m_mutex_exception_fd_set;
      
      struct timeval m_responsiveness;

      /**
       * This is the maximum number of dispatches that will occur for a
       * connection in one iteration of the dispatch thread.
       *
       * If set to 0, a particular connection will continue to dispatch
       * as long as its status remains DISPATCH_DATA_REMAINS.
       */
      unsigned int m_dispatch_loop_limit;
      
      bool m_initiate_processing;      
      pthread_cond_t m_cond_initiate_processing;
      pthread_mutex_t m_mutex_initiate_processing;
      
      void dispatch_thread_main();
      
      void watch_thread_main();
      
      /**
       * Since pthread_create cannot take a class method as the
       * thread-main parameter this function is called with an
       * argument pointing to the Dispatcher instance to start
       * in the thread.
       */
      static void* proxy_dispatch_thread_main(void*);
      
      /**
       * Since pthread_create cannot take a class method as the
       * thread-main parameter this function is called with an
       * argument pointing to the Dispatcher instance to start
       * in the thread.
       */
      static void* proxy_watch_thread_main(void*);
      
      bool on_add_watch(Watch::pointer);
      
      bool on_remove_watch(Watch::pointer);
      
      bool on_watch_toggled(Watch::pointer);
      
      bool on_add_timeout(Timeout::pointer);
      
      bool on_remove_timeout(Timeout::pointer);
      
      bool on_timeout_toggled(Timeout::pointer);
      
      void on_wakeup_main(Connection::pointer);
      
      void on_dispatch_status_changed(DispatchStatus, Connection::pointer);
  };

}

#endif

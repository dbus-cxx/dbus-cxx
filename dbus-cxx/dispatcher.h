/***************************************************************************
 *   Copyright (C) 2007,2008,2009,2010 by Rick L. Vinyard, Jr.             *
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
#include <map>
#include <list>
#include <set>
#include <thread>
#include <mutex>

#include <poll.h>

#include <dbus/dbus.h>
#include <dbus-cxx/connection.h>
#include <dbus-cxx/watch.h>
#include <dbus-cxx/timeout.h>

#ifndef DBUSCXX_DISPATCHER_H
#define DBUSCXX_DISPATCHER_H

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
    protected:

      Dispatcher(bool is_running=true);

    public:
      
      typedef DBusCxxPointer<Dispatcher> pointer;

      typedef DBusCxxPointer<const Dispatcher> const_pointer;

      static pointer create( bool is_running=true );

      virtual ~Dispatcher();
      
      /** @name Managing Connections */
      //@{

      Connection::pointer create_connection( DBusConnection* cobj = NULL, bool is_private=false );

      Connection::pointer create_connection( BusType type, bool is_private=false );

      Connection::pointer create_connection( std::string address, bool is_private = false );

      Connection::pointer create_connection( const Connection& other );

      bool add_connection( Connection::pointer connection );

      //@}

      bool start();
      
      bool stop();
      
      bool is_running();

    protected:
      
      typedef std::list<Connection::pointer> Connections;
      Connections m_connections;
      
      volatile bool m_running;
      
      std::thread* m_dispatch_thread;

      class WatchPair {
      public:
          WatchPair() : read_watch( NULL ), write_watch( NULL ){}
          WatchPair( Watch::pointer read, Watch::pointer write ) :
              read_watch( read ),
              write_watch( write ) {}

          Watch::pointer read_watch;
          Watch::pointer write_watch;
      };
      std::mutex m_mutex_watches;
      std::map<int,WatchPair> m_watches_map;
      
      std::mutex m_mutex_exception_fd_set;
      std::vector<int> m_exception_fd_set;
      
      /* socketpair for telling the thread to process data */
      int process_fd[ 2 ];

      /**
       * This is the maximum number of dispatches that will occur for a
       * connection in one iteration of the dispatch thread.
       *
       * If set to 0, a particular connection will continue to dispatch
       * as long as its status remains DISPATCH_DATA_REMAINS.
       */
      unsigned int m_dispatch_loop_limit;
      
      virtual void dispatch_thread_main();
      
      bool on_add_watch(Watch::pointer);
      
      bool on_remove_watch(Watch::pointer);
      
      void on_watch_toggled(Watch::pointer);
      
      bool on_add_timeout(Timeout::pointer);
      
      bool on_remove_timeout(Timeout::pointer);
      
      bool on_timeout_toggled(Timeout::pointer);
      
      void on_wakeup_main(Connection::pointer);
      
      void on_dispatch_status_changed(DispatchStatus, Connection::pointer);

      void wakeup_thread();

      /**
       * Add all read and write watch FDs to the given vector to watch.
       */
      void add_read_and_write_watches( std::vector<struct pollfd>* fds );

      /**
       * Handle all of the read and write watches if the given FD needs to be serviced.
       */
      void handle_read_and_write_watches( std::vector<struct pollfd>* fds );

      /**
       * Dispatch all of our connections
       */
      void dispatch_connections();
  };

}

#endif

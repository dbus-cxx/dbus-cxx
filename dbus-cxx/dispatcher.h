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

#include <dbus/dbus.h>

#include <poll.h>

#include "forward_decls.h"

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

      static std::shared_ptr<Dispatcher> create( bool is_running=true );

      virtual ~Dispatcher();
      
      /** @name Managing Connections */
      //@{

      std::shared_ptr<Connection> create_connection( DBusConnection* cobj = NULL, bool is_private=false );

      std::shared_ptr<Connection> create_connection( BusType type, bool is_private=false );

      std::shared_ptr<Connection> create_connection( const Connection& other );

      bool add_connection( std::shared_ptr<Connection> connection );

      //@}

      bool start();
      
      bool stop();
      
      bool is_running();

    protected:
      
      typedef std::list<std::shared_ptr<Connection>> Connections;
      Connections m_connections;
      
      volatile bool m_running;
      
      std::thread* m_dispatch_thread;

      class WatchPair {
      public:
          WatchPair() : read_watch( NULL ), write_watch( NULL ){}
          WatchPair( std::shared_ptr<Watch> read, std::shared_ptr<Watch> write ) :
              read_watch( read ),
              write_watch( write ) {}

          std::shared_ptr<Watch> read_watch;
          std::shared_ptr<Watch> write_watch;
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
      
      bool on_add_watch(std::shared_ptr<Watch>);
      
      bool on_remove_watch(std::shared_ptr<Watch>);
      
      void on_watch_toggled(std::shared_ptr<Watch>);
      
      bool on_add_timeout(std::shared_ptr<Timeout>);
      
      bool on_remove_timeout(std::shared_ptr<Timeout>);
      
      bool on_timeout_toggled(std::shared_ptr<Timeout>);
      
      void on_wakeup_main(std::shared_ptr<Connection>);
      
      void on_dispatch_status_changed(DispatchStatus, std::shared_ptr<Connection>);

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

/***************************************************************************
 *   Copyright (C) 2020 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
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
#ifndef DBUSCXX_STANDALONE_DISPATCHER
#define DBUSCXX_STANDALONE_DISPATCHER

#include "dispatcher.h"

namespace DBus {

class Connection;

/**
 * The StandaloneDispatcher creates a new thread that handles all of the
 * reading and writing to the bus.
 *
 * One dispatcher can handle multiple connections.
 */
class StandaloneDispatcher : public Dispatcher
{
  private:

    StandaloneDispatcher(bool is_running=true);

  public:

    static std::shared_ptr<StandaloneDispatcher> create( bool is_running=true );

    ~StandaloneDispatcher();

    /** @name Managing Connections */
    //@{
    std::shared_ptr<Connection> create_connection( BusType type );

    std::shared_ptr<Connection> create_connection( std::string address );

    bool add_connection( std::shared_ptr<Connection> connection );

    //@}

    bool start();

    bool stop();

    bool is_running();

  private:

    void dispatch_thread_main();

    void wakeup_thread();

    /**
     * Dispatch all of our connections
     */
    void dispatch_connections();

private:
    class priv_data;

    DBUS_CXX_PROPAGATE_CONST(std::unique_ptr<priv_data>) m_priv;
};

} /* namespace DBus */

#endif /* DBUSCXX_STANDALONE_DISPATCHER */

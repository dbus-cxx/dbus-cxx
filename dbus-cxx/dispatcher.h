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
#include <dbus-cxx/dbus-cxx-config.h>
#include <string>
#include <memory>
#include "enums.h"

#ifndef DBUSCXX_DISPATCHER_H
#define DBUSCXX_DISPATCHER_H

namespace DBus {
class Connection;

/**
 * A Dispatcher handles all of the reading/writing that a Connection
 * needs in order to work correctly.  Logically, there should only be
 * one Dispatcher per application for the lifetime of that application.
 */
class Dispatcher {
public:
    virtual ~Dispatcher();

    /** @name Managing Connections */
    //@{
    virtual std::shared_ptr<Connection> create_connection( BusType type ) = 0;

    virtual std::shared_ptr<Connection> create_connection( std::string address ) = 0;

    virtual bool add_connection( std::shared_ptr<Connection> connection ) = 0;

    //@}
};

}

#endif

// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
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
#ifndef DBUS_CXX_UV_DISPATCHER_H
#define DBUS_CXX_UV_DISPATCHER_H

#include <dbus-cxx/dispatcher.h>

namespace DBus {
namespace Uv {

class UvDispatcher : public ::DBus::Dispatcher {
private:
    UvDispatcher();

public:
    ~UvDispatcher();

    static std::shared_ptr<UvDispatcher> create();

    std::shared_ptr<Connection> create_connection( BusType type );

    std::shared_ptr<Connection> create_connection( std::string address );

    bool add_connection( std::shared_ptr<Connection> connection );

private:
    class priv_data;

    DBUS_CXX_PROPAGATE_CONST(std::unique_ptr<priv_data>) m_priv;
};

} /* namespace Uv */
} /* namespace DBus */

#endif /* DBUS_CXX_UV_DISPATCHER_H */

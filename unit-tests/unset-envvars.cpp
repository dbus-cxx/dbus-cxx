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

#include <string>
#include <dbus-cxx.h>
#include <stdlib.h>

#include "test_macros.h"

static bool envvars_connect_session_bus() {
    unsetenv( "DBUS_SESSION_BUS_ADDRESS" );
    std::shared_ptr<DBus::Connection> conn = DBus::Connection::create( DBus::BusType::SESSION );

    return !conn->is_valid();
}

static bool envvars_connect_starter_bus() {
    unsetenv( "DBUS_STARTER_ADDRESS" );
    std::shared_ptr<DBus::Connection> conn = DBus::Connection::create( DBus::BusType::STARTER );

    return !conn->is_valid();
}

#define ADD_TEST(name) do{ if( test_name == STRINGIFY(name) ){ \
            ret = envvars_##name();\
        } \
    } while( 0 )

int main( int argc, char** argv ) {
    if( argc < 1 ) {
        return 1;
    }

    std::string test_name = argv[1];
    bool ret = false;

    DBus::set_logging_function( DBus::log_std_err );
    DBus::set_log_level( SL_TRACE );

    ADD_TEST( connect_session_bus );
    ADD_TEST( connect_starter_bus );

    return !ret;
}

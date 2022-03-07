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

#include <dbus-cxx.h>
#include <thread>

static std::shared_ptr<DBus::Dispatcher> dispatch;
static std::shared_ptr<DBus::Connection> conn;

static int result = 0;

static int a_test( int a, int b ) {
    return a + b;
}

static void b_test() {
    std::shared_ptr<DBus::ObjectProxy> proxyObject =
        conn->create_object_proxy( "dbuscxx.test.sideA", "/atest" );
    std::shared_ptr<DBus::MethodProxy<int( int, int )>> proxyMethod =
            proxyObject->create_method<int( int, int )>( "dbuscxx.interface", "a_test" );

    result = ( *proxyMethod )( 5, 2 );
}

// This test does the following:
// Call B.test() from A
// Call A.test(5,2) from B.test()
int main( int argc, char** argv ) {
    int retval = 0;

    if( argc < 1 ) {
        return 1;
    }

    DBus::set_logging_function( DBus::log_std_err );
    DBus::set_log_level( SL_TRACE );

    dispatch = DBus::StandaloneDispatcher::create();
    conn = dispatch->create_connection( DBus::BusType::SESSION );

    if( argv[ 1 ][ 0 ] == 'A' ) {
        conn->request_name( "dbuscxx.test.sideA" );

        std::shared_ptr<DBus::ObjectProxy> proxy =
            conn->create_object_proxy( "dbuscxx.test.sideB", "/btest" );
        std::shared_ptr<DBus::MethodProxy<void()>> method =
                proxy->create_method<void()>( "dbuscxx.interface", "b_test" );

        std::shared_ptr<DBus::Object> obj =
            conn->create_object( "/atest", DBus::ThreadForCalling::DispatcherThread );

        obj->create_method<int( int, int )>( "dbuscxx.interface", "a_test", sigc::ptr_fun( a_test ) );

        std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );

        ( *method )();

    } else if( argv[ 1 ][ 0 ] == 'B' ) {
        conn->request_name( "dbuscxx.test.sideB" );

        std::shared_ptr<DBus::Object> obj =
            conn->create_object( "/btest", DBus::ThreadForCalling::DispatcherThread );

        obj->create_method<void()>( "dbuscxx.interface", "b_test", sigc::ptr_fun( b_test ) );

        std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

        if( result != 7 ) {
            retval = 1;
        }
    }

    return retval;
}

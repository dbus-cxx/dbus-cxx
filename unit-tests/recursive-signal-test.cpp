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
#include <iostream>
#include <thread>

static std::shared_ptr<DBus::Dispatcher> dispatch;
static std::shared_ptr<DBus::Connection> conn;

static std::string signal_message;
static int proxy_retval;
static std::shared_ptr<DBus::ObjectProxy> a_side_proxy;

static void some_signal_handler( std::string str ){
    signal_message = str;

    std::shared_ptr<DBus::MethodProxy<int(int,int)>> methodproxy =
            a_side_proxy->create_method<int(int,int)>( "dbuscxx.interface", "a_test" );

    proxy_retval = (*methodproxy)( 5, 2 );
}

static int a_test( int a, int b ){
    return a + b;
}

// This test does the following:
// Emit a signal from A
// Call A.test(5,2) B signal handler
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

        std::shared_ptr<DBus::Object> obj =
            conn->create_object( "/atest", DBus::ThreadForCalling::DispatcherThread );

        obj->create_method<int( int, int )>( "dbuscxx.interface", "a_test", sigc::ptr_fun( a_test ) );
        std::shared_ptr<DBus::Signal<void(std::string)>> sig =
                obj->create_signal<void(std::string)>( "dbuscxx.interface", "some_signal" );

        std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );

        sig->emit( "foobar" );
        conn->flush();

        std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
    } else if( argv[ 1 ][ 0 ] == 'B' ) {
        conn->request_name( "dbuscxx.test.sideB" );

        a_side_proxy =
            conn->create_object_proxy( "dbuscxx.test.sideA", "/atest" );

        std::shared_ptr<DBus::SignalProxy<void(std::string)>> sigproxy =
                a_side_proxy->create_signal<void(std::string)>( "dbuscxx.interface", "some_signal" );

        sigproxy->connect( sigc::ptr_fun( some_signal_handler ) );

        std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

        if( signal_message != "foobar" ){
            retval = 1;
        }

        if( proxy_retval != 7 ){
            retval = 1;
        }
    }

    return retval;
}

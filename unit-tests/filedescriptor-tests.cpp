// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
/***************************************************************************
 *   Copyright (C) 2019 by Robert Middleton                                *
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
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <errno.h>
#include <thread>

#include "test_macros.h"

static std::shared_ptr<DBus::Dispatcher> dispatch;
static std::shared_ptr<DBus::Connection> conn;
// read = pipes[0], write = pipes[1]
static int pipes[2];

// Client variables
static std::shared_ptr<DBus::ObjectProxy> proxy;
static std::shared_ptr<DBus::MethodProxy<std::shared_ptr<DBus::FileDescriptor>()>> fd_return_method_proxy;
static std::shared_ptr<DBus::MethodProxy<void( std::shared_ptr<DBus::FileDescriptor> )>> fd_set_method_proxy;

// Server variables
static std::shared_ptr<DBus::Object> object;
static std::shared_ptr<DBus::Method<std::shared_ptr<DBus::FileDescriptor>()>> fd_return_method;
static std::shared_ptr<DBus::Method<void( std::shared_ptr<DBus::FileDescriptor> )>> fd_set_method;

void write_to_fd( int fd ) {
    std::this_thread::sleep_for( std::chrono::milliseconds( 250 ) );

    std::string daString = "This is a message";
    ::write( fd, daString.c_str(), daString.length() );
}

// Server Function
std::shared_ptr<DBus::FileDescriptor> get_filedescriptor() {
    std::async( std::launch::async, write_to_fd, pipes[1] );

    return DBus::FileDescriptor::create( pipes[0] );
}

// Server Function
void set_filedescriptor( std::shared_ptr<DBus::FileDescriptor> fd ) {
    std::async( std::launch::async, write_to_fd, fd->descriptor() );
}

// Client Function
bool filedescriptor_send() {
    char bytes[ 64 ];

    if( pipe( pipes ) < 0 ) {
        std::cerr << "Can't create pipe" << std::endl;
        exit( 1 );
    }

    ( *fd_set_method_proxy )( DBus::FileDescriptor::create( pipes[1] ) );

    int bytesGot = ::read( pipes[0], bytes, 64 );

    if( bytesGot < 0 ) {
        return false;
    }

    bytes[ bytesGot ] = 0;
    std::string str( bytes );

    return str.compare( "This is a message" ) == 0;
}

// Client function
bool filedescriptor_get() {
    std::shared_ptr<DBus::FileDescriptor> fd = ( *fd_return_method_proxy )();
    char bytes[ 64 ];

    int bytesGot = ::read( fd->descriptor(), bytes, 64 );

    if( bytesGot < 0 ) {
        std::cerr << "Unable to read: " << strerror( errno ) << std::endl;
        return false;
    }

    bytes[ bytesGot ] = 0;
    std::string str( bytes );

    return str.compare( "This is a message" ) == 0;
}

void client_setup() {
    proxy = conn->create_object_proxy( "dbuscxx.test", "/fdtest" );

    fd_return_method_proxy = proxy->create_method<std::shared_ptr<DBus::FileDescriptor>()>( "foo.what", "get_filedescriptor" );
    fd_set_method_proxy = proxy->create_method<void( std::shared_ptr<DBus::FileDescriptor> )>( "foo.what", "set_filedescriptor" );
}

void server_setup() {
    DBus::RequestNameResponse ret = conn->request_name( "dbuscxx.test", DBUSCXX_NAME_FLAG_REPLACE_EXISTING );

    if( ret != DBus::RequestNameResponse::PrimaryOwner ) { exit( 1 ); }

    if( pipe( pipes ) < 0 ) {
        std::cerr << "Can't create pipe" << std::endl;
        exit( 1 );
    }

    object = conn->create_object( "/fdtest", DBus::ThreadForCalling::DispatcherThread );
    fd_return_method = object->create_method<std::shared_ptr<DBus::FileDescriptor>()>( "foo.what", "get_filedescriptor", sigc::ptr_fun( get_filedescriptor ) );
    fd_set_method = object->create_method<void( std::shared_ptr<DBus::FileDescriptor> )>( "foo.what", "set_filedescriptor", sigc::ptr_fun( set_filedescriptor ) );
}

#define ADD_TEST(name) do{ if( test_name == STRINGIFY(name) ){ \
            ret = filedescriptor_##name();\
        } \
    } while( 0 )

int main( int argc, char** argv ) {
    if( argc < 1 ) {
        return 1;
    }

    std::string test_name = argv[2];
    bool ret = false;
    bool is_client = std::string( argv[1] ) == "client";

    DBus::set_logging_function( DBus::log_std_err );
    DBus::set_log_level( SL_TRACE );
    dispatch = DBus::StandaloneDispatcher::create();
    conn = dispatch->create_connection( DBus::BusType::SESSION );

    if( is_client ) {
        client_setup();
        ADD_TEST( send );
        ADD_TEST( get );
    } else {
        server_setup();
        ret = true;
        std::this_thread::sleep_for( std::chrono::seconds( 1 ) );;
    }

    return !ret;
}

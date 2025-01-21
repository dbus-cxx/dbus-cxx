// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
/***************************************************************************
 *   Copyright (C) 2025 by Robert Middleton                                *
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
#include <dbus-cxx/standard-interfaces/objectmanagerproxy.h>
#include <memory>
#include <unistd.h>
#include <iostream>

#include "test_macros.h"

std::shared_ptr<DBus::Dispatcher> dispatch;
std::shared_ptr<DBus::Connection> conn;

std::shared_ptr<DBus::ObjectProxy> proxy_root;

void client_setup() {
    proxy_root = conn->create_object_proxy( "dbuscxx.test", "/" );
}

void server_setup() {
    DBus::RequestNameResponse ret = conn->request_name( "dbuscxx.test", DBUSCXX_NAME_FLAG_REPLACE_EXISTING );

    if( ret != DBus::RequestNameResponse::PrimaryOwner ) { exit( 1 ); }

    std::shared_ptr<DBus::Object> root_obj = conn->create_object( "/" );

    std::shared_ptr<DBus::Object> testobj1 = conn->create_object( "/test" );
    std::shared_ptr<DBus::Interface> interface1 = testobj1->create_interface( "org.bar.fuzz" );
    interface1->create_property<uint32_t>( "wojack" )->set_value( 98 );

    std::shared_ptr<DBus::Object> testobj2 = conn->create_object( "/test/two" );
    std::shared_ptr<DBus::Interface> interface2 = testobj2->create_interface( "shim.sum" );
    interface2->create_property<std::string>( "eggs" )->set_value( "whatup" );

    root_obj->set_has_objectmanager( true );
}

bool objmanager_query(){
    DBus::ObjectManagerObjects interfaces =
            proxy_root->getObjectManagerInterface()->GetManagedObjects();

    if( interfaces.size() != 2 ){
        return false;
    }

    if( interfaces[ "/test" ].size() != 1 ){
        return false;
    }

    if( interfaces[ "/test" ][ "org.bar.fuzz" ][ "wojack" ].to_uint32() != 98 ){
        return false;
    }

    return true;
}

#define ADD_TEST(name) do{ if( test_name == STRINGIFY(name) ){ \
        ret = objmanager_##name();\
    } \
} while( 0 )

int main( int argc, char** argv )
{
    if ( argc < 3 ) {
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

        ADD_TEST( query );

        std::cout << "Test case \"" + test_name + "\" " + (ret ? "PASSED" : "FAIL") << std::endl;

    } else {
        server_setup();
        ret = true;
        sleep( 1 );
    }

    return !ret;
}

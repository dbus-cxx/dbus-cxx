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

std::shared_ptr<DBus::Object> root_obj;

void client_setup() {
    proxy_root = conn->create_object_proxy( "dbuscxx.test", "/" );
}

void server_setup() {
    DBus::RequestNameResponse ret = conn->request_name( "dbuscxx.test", DBUSCXX_NAME_FLAG_REPLACE_EXISTING );

    if( ret != DBus::RequestNameResponse::PrimaryOwner ) { exit( 1 ); }

    root_obj = conn->create_object( "/" );

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

bool objmanager_interfaces_added_signal1(){
    bool received_signal = false;

    proxy_root->getObjectManagerInterface()->signal_InterfacesAdded()
            .connect( [&received_signal](DBus::Path path, std::map<std::string,std::map<std::string,DBus::Variant>> iface_and_props){
        if( path.compare( "/test" ) == 0 &&
                iface_and_props.size() == 1 &&
                iface_and_props[ "/test" ].size() == 0){
            received_signal = true;
        }
    });

    std::this_thread::sleep_for( std::chrono::seconds( 2 ) );

    return received_signal;
}

bool objmanager_server_interfaces_added_signal1(){
    std::this_thread::sleep_for( std::chrono::milliseconds(500) );

    // This should emit the InterfacesAdded signal, but without any properties
    std::shared_ptr<DBus::Interface> interface1 = root_obj->child( "test" )->create_interface( "fuzzbar" );

    return true;
}

bool objmanager_interfaces_added_signal2(){
    bool received_signal = false;

    proxy_root->getObjectManagerInterface()->signal_InterfacesAdded()
            .connect( [&received_signal](DBus::Path path, std::map<std::string,std::map<std::string,DBus::Variant>> iface_and_props){
        bool rx_props = false;

        if( path.compare( "/test" ) == 0 &&
                iface_and_props.size() == 1 &&
                iface_and_props[ "/test" ].size() == 2 ){
            received_signal = true;
        }

        if( received_signal ){
            std::map<std::string,DBus::Variant> props = iface_and_props[ "/test" ];
            if( props[ "someprop" ].to_uint32() == 92 &&
                    props[ "eggscom" ].to_string() == "eyo" ){
                rx_props = true;
            }
        }

        received_signal = received_signal && rx_props;
    });

    std::this_thread::sleep_for( std::chrono::seconds( 2 ) );

    return received_signal;
}

bool objmanager_server_interfaces_added_signal2(){
    std::this_thread::sleep_for( std::chrono::milliseconds(500) );

    std::shared_ptr<DBus::Interface> iface = DBus::Interface::create( "fuzzbar" );
    iface->create_property<int32_t>( "someprop" )->set_value( 92 );
    iface->create_property<std::string>( "eggscom" )->set_value( "eyo" );
    root_obj->child( "test" )->add_interface( iface );

    return true;
}

bool objmanager_interfaces_removed_signal1(){
    bool received_signal = false;

    proxy_root->getObjectManagerInterface()->signal_InterfacesRemoved()
            .connect( [&received_signal](DBus::Path path, std::vector<std::string> interfaces){
        if( path.compare( "/test" ) == 0 &&
                interfaces.size() == 1 &&
                interfaces[0] == "fuzzbar"){
            received_signal = true;
        }
    });

    std::this_thread::sleep_for( std::chrono::seconds( 2 ) );

    return received_signal;
}

bool objmanager_server_interfaces_removed_signal1(){
    std::this_thread::sleep_for( std::chrono::milliseconds(500) );

    std::shared_ptr<DBus::Interface> interface1 = root_obj->child( "test" )->create_interface( "fuzzbar" );
    root_obj->child( "test" )->remove_interface( "fuzzbar" );

    return true;
}

#define ADD_TEST(name) do{ if( test_name == STRINGIFY(name) ){ \
        ret = objmanager_##name();\
    } \
} while( 0 )

#define ADD_TEST_SERVER(name) do{ if( test_name == STRINGIFY(name) ){ \
    ret = objmanager_server_##name();\
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
        ADD_TEST( interfaces_added_signal1 );
        ADD_TEST( interfaces_added_signal2 );
        ADD_TEST( interfaces_removed_signal1 );

        std::cout << "Test case \"" + test_name + "\" " + (ret ? "PASSED" : "FAIL") << std::endl;

    } else {
        server_setup();
        ret = true;

        ADD_TEST_SERVER( interfaces_added_signal1 );
        ADD_TEST_SERVER( interfaces_added_signal2 );
        ADD_TEST_SERVER( interfaces_removed_signal1 );

        std::this_thread::sleep_for( std::chrono::seconds(1) );
    }

    return !ret;
}

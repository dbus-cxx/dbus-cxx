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
#include <string>
#include "test_macros.h"
#include <thread>

static std::shared_ptr<DBus::Dispatcher> dispatch;
static std::shared_ptr<DBus::Connection> conn;

static std::shared_ptr<DBus::ObjectProxy> proxy;

static std::shared_ptr<DBus::Object> object;

bool property_get_all(){
    std::shared_ptr<DBus::InterfaceProxy> iface = proxy->interface_by_name( "dbuscxx.interface" );

    iface->cache_properties();

    return iface->properties().size() == 2;
}

bool property_get_valid(){
    std::shared_ptr<DBus::InterfaceProxy> iface = proxy->interface_by_name( "dbuscxx.interface" );

    bool propertyExists = iface->property( "intproperty" ).operator bool();
    bool propertyValueOk = false;

    if( propertyExists ){
        propertyValueOk = iface->property( "intproperty" )->variant_value().to_int32() == 9834;
    }

    return propertyExists && propertyValueOk;
}

bool property_set_valid(){
    std::shared_ptr<DBus::InterfaceProxy> iface = proxy->interface_by_name( "dbuscxx.interface" );

    iface->property( "intproperty" )->set_value( 5248 );

    return iface->property( "intproperty" )->variant_value().to_int32() == 5248;
}

bool property_set_invalid(){
    std::shared_ptr<DBus::InterfaceProxy> iface = proxy->interface_by_name( "dbuscxx.interface" );

    try {
        iface->create_property<int64_t>( "ThisDoesntExist" )->set_value( 7888 );
    }catch( DBus::ErrorUnknownProperty ex ){
        return true;
    }

    return false;
}

bool property_set_readonly(){
    std::shared_ptr<DBus::InterfaceProxy> iface = proxy->interface_by_name( "dbuscxx.interface" );

    try {
        iface->create_property<int64_t>( "readonly" )->set_value( 7888 );
    }catch( DBus::ErrorPropertyReadOnly ex ){
        return true;
    }

    return false;
}

bool property_signal_emitted(){
    std::shared_ptr<DBus::InterfaceProxy> iface = proxy->interface_by_name( "dbuscxx.interface" );
    std::shared_ptr<DBus::SignalProxy<void(std::string,std::map<std::string,DBus::Variant>,std::vector<std::string>)>> sig =
            conn->create_free_signal_proxy<void(std::string,std::map<std::string,DBus::Variant>,std::vector<std::string>)>(
                DBus::MatchRuleBuilder::create()
                .set_path( "/test" )
                .set_interface( DBUS_CXX_PROPERTIES_INTERFACE )
                .set_member( "PropertiesChanged" )
                .as_signal_match()
                );

    int done = 0;
    DBus::Variant variantValue;
    sig->connect( [&done,&variantValue](std::string,std::map<std::string,DBus::Variant> changed,std::vector<std::string>) mutable {
        done = 1;
        variantValue = changed[ "intproperty" ];
                  });

    iface->property( "intproperty" )->set_value( 5248 );

    std::this_thread::sleep_for( std::chrono::milliseconds( 750 ) );

    return done == 1 && variantValue.to_int32() == 5248;
}

bool property_updated_from_signal(){
    std::shared_ptr<DBus::InterfaceProxy> iface = proxy->interface_by_name( "dbuscxx.interface" );
    std::shared_ptr<DBus::PropertyProxyBase> prop = iface->property( "intproperty" );

    // Wait for the signal - this signal will get emitted when the value gets updated via a signal
    int times = 0;
    int value;
    prop->signal_generic_property_changed().connect( [&times, &value]( DBus::Variant v ){
        times++;
        value = v.to_int32();
    });

    prop->set_value( 7878 );

    std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );

    return times == 1 && value == 7878;
}

void client_setup() {
    proxy = conn->create_object_proxy( "dbuscxx.test", "/test" );

    proxy->create_property<int32_t>( "dbuscxx.interface", "intproperty" );
    proxy->create_property<int32_t>( "dbuscxx.interface", "readonly" );
}

void server_setup() {
    DBus::RequestNameResponse ret = conn->request_name( "dbuscxx.test", DBUSCXX_NAME_FLAG_REPLACE_EXISTING );

    if( ret != DBus::RequestNameResponse::PrimaryOwner ) { exit( 1 ); }

    object = conn->create_object( "/test", DBus::ThreadForCalling::DispatcherThread );
    std::shared_ptr<DBus::Property<int32_t>> intproperty =
            object->create_property<int32_t>( "dbuscxx.interface", "intproperty" );
    // must set the value before it will be returned by dbus-cxx
    intproperty->set_value( 9834 );

    std::shared_ptr<DBus::Property<int32_t>> unsetproperty =
            object->create_property<int32_t>( "dbuscxx.interface", "unset" );

    std::shared_ptr<DBus::Property<int32_t>> readonly =
            object->create_property<int32_t>( "dbuscxx.interface", "readonly", DBus::PropertyAccess::ReadOnly );
    readonly->set_value( 44 );
}

#define ADD_TEST(name) do{ if( test_name == STRINGIFY(name) ){ \
            ret = property_##name();\
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

        ADD_TEST( get_all );
        ADD_TEST( get_valid );
        ADD_TEST( set_valid );
        ADD_TEST( set_invalid );
        ADD_TEST( set_readonly );
        ADD_TEST( signal_emitted );
        ADD_TEST( updated_from_signal );
    } else {
        server_setup();
        ret = true;
        std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
    }


    return !ret;
}

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
//#include <dbus-cxx.h>
#include <unistd.h>
#include <iostream>

#include "test_macros.h"
#include "custom-type.h"

std::shared_ptr<DBus::Dispatcher> dispatch;
std::shared_ptr<DBus::Connection> conn;

std::shared_ptr<DBus::ObjectProxy> proxy;
std::shared_ptr<DBus::MethodProxy<int( int, int )>> int_method_proxy;
std::shared_ptr<DBus::MethodProxy<void()>> void_method_proxy;
std::shared_ptr<DBus::MethodProxy<void( struct custom )>> void_custom_method_proxy;
std::shared_ptr<DBus::MethodProxy<int( int, struct custom )>> int_custom_method_proxy2;
std::shared_ptr<DBus::MethodProxy<bool( std::tuple<int, double, std::string> )>> tuple_method_proxy;
std::shared_ptr<DBus::MethodProxy<void( DBus::Variant )>> variant_proxy;
std::shared_ptr<DBus::MethodProxy<void()>> nonexistant_proxy;

std::shared_ptr<DBus::Object> object;
std::shared_ptr<DBus::Method<int( int, int )>> int_method;
std::shared_ptr<DBus::Method<void()>> void_method;
std::shared_ptr<DBus::Method<void( struct custom )>> void_custom_method;
std::shared_ptr<DBus::Method<int( int, struct custom )>> int_custom_method2;
std::shared_ptr<DBus::Method<bool( std::tuple<int, double, std::string> )>> tuple_method;
std::shared_ptr<DBus::Method<void( DBus::Variant )>> variant_method;

bool tuple_method_symbol( std::tuple<int, double, std::string> tup ) {
    bool retval = true;

    if( std::get<0>( tup ) != 26 ) {
        retval = false;
    }

    if( std::get<1>( tup ) != 1.1 ) {
        retval = false;
    }

    if( std::get<2>( tup ) != "Tuple Check" ) {
        retval = false;
    }

    return retval;
}

int add( int a, int b ) {
    return a + b;
}

void void_method_symbol() {}

void void_custom_method_symbol( struct custom c ) {
}

int int_intcustom_symbol( int i, struct custom c ) {
    return i + c.first + c.second;
}

void variant_method_symbol( DBus::Variant v ) {
    std::cerr << "Got incoming variant: " << v << std::endl;
}

void client_setup() {
    proxy = conn->create_object_proxy( "dbuscxx.test", "/test" );

    int_method_proxy = proxy->create_method<int( int, int )>( "foo.what", "add" );
    void_method_proxy = proxy->create_method<void()>( "foo.what", "void" );
    void_custom_method_proxy = proxy->create_method<void( struct custom )>( "foo.what", "void_custom" );
    int_custom_method_proxy2 = proxy->create_method<int( int, struct custom )>( "foo.what", "int_intcustom" );
    tuple_method_proxy = proxy->create_method<bool( std::tuple<int, double, std::string> )>( "foo.what", "tuple_method" );
    variant_proxy = proxy->create_method<void( DBus::Variant )>( "foo.what", "variant_method" );
    nonexistant_proxy = proxy->create_method<void()>( "foo.what", "nonexistant" );
}

void server_setup() {
    DBus::RequestNameResponse ret = conn->request_name( "dbuscxx.test", DBUSCXX_NAME_FLAG_REPLACE_EXISTING );

    if( ret != DBus::RequestNameResponse::PrimaryOwner ) { exit( 1 ); }

    object = conn->create_object( "/test", DBus::ThreadForCalling::DispatcherThread );
    int_method = object->create_method<int( int, int )>( "foo.what", "add", sigc::ptr_fun( add ) );
    void_method = object->create_method<void()>( "foo.what", "void", sigc::ptr_fun( void_method_symbol ) );
    void_custom_method = object->create_method<void( struct custom )>( "foo.what", "void_custom", sigc::ptr_fun( void_custom_method_symbol ) );
    int_custom_method2 = object->create_method<int( int, struct custom )>( "foo.what", "int_intcustom", sigc::ptr_fun( int_intcustom_symbol ) );
    tuple_method = object->create_method<bool( std::tuple<int, double, std::string> )>( "foo.what", "tuple_method", sigc::ptr_fun( tuple_method_symbol ) );
    variant_method = object->create_method<void( DBus::Variant )>( "foo.what", "variant_method", sigc::ptr_fun( variant_method_symbol ) );
}

bool data_send_integers() {
    int val = ( *int_method_proxy )( 2, 3 );

    return TEST_EQUALS( val, 5 );
}

bool data_void_method() {
    ( *void_method_proxy )();

    return true;
}

bool data_void_custom() {
    struct custom c;
    c.first = 5;
    c.second = 10;
    ( *void_custom_method_proxy )( c );

    return true;
}

bool data_send_intcustom() {
    struct custom c;
    c.first = 5;
    c.second = 10;
    int val = ( *int_custom_method_proxy2 )( 10, c );

    return TEST_EQUALS( val, 25 );
}

bool data_ping() {
    std::shared_ptr<DBus::MethodProxy<void()>> pingMethod =
            proxy->create_method<void()>( DBUS_CXX_PEER_INTERFACE, "Ping" );

    ( *pingMethod )();

    return true;
}

bool data_machine_uuid() {
    std::shared_ptr<DBus::MethodProxy<std::string()>> machineIdMethod =
            proxy->create_method<std::string()>( DBUS_CXX_PEER_INTERFACE, "GetMachineId" );

    std::string retval = ( *machineIdMethod )();

    // DBus Specification:
    // The hex-encoded string may not contain hyphens or other non-hex-digit characters,
    // and it must be exactly 32 characters long.
    if( retval.length() == 32 ) {
        return true;
    }

    return false;
}

bool data_tuple() {
    std::tuple<int, double, std::string> tup = std::make_tuple( 26, 1.1, "Tuple Check" );

    return ( *tuple_method_proxy )( tup );
}

bool data_variant_array() {
    std::vector<int> vec;
    vec.push_back( 5 );
    vec.push_back( 5248 );
    vec.push_back( 8888 );
    DBus::Variant send( vec );

    ( *variant_proxy )( send );

    return true;
}

bool data_variant_map() {
    std::map<uint16_t, int> good;
    good[ 1 ] = 1001;
    good[ 2 ] = 1002;
    good[ 3 ] = 1003;
    DBus::Variant send( good );

    ( *variant_proxy )( send );

    return true;
}

bool data_variant_tuple() {
    std::tuple<uint16_t, int> good = { 5, 0x0b12cd67 };
    DBus::Variant send( good );

    ( *variant_proxy )( send );

    return true;
}

bool data_nonexistant_method() {
    try {
        ( *nonexistant_proxy )();
    } catch( DBus::ErrorUnknownMethod ex ) {
        return true;
    }

    return false;
}

#define ADD_TEST(name) do{ if( test_name == STRINGIFY(name) ){ \
            ret = data_##name();\
        } \
    } while( 0 )

int main( int argc, char** argv ) {
    if( argc < 1 ) {
        return 1;
    }

    std::string test_name = argv[2];
    bool ret = false;
    bool is_client = std::string( argv[1] ) == "client";

    DBus::setLoggingFunction( DBus::logStdErr );
    DBus::setLogLevel( SL_TRACE );

    dispatch = DBus::StandaloneDispatcher::create();
    conn = dispatch->create_connection( DBus::BusType::SESSION );

    if( is_client ) {
        client_setup();
        ADD_TEST( send_integers );
        ADD_TEST( void_method );
        ADD_TEST( void_custom );
        ADD_TEST( send_intcustom );
        ADD_TEST( ping );
        ADD_TEST( machine_uuid );
        ADD_TEST( tuple );
        ADD_TEST( variant_array );
        ADD_TEST( variant_map );
        ADD_TEST( variant_tuple );
        ADD_TEST( nonexistant_method );
    } else {
        server_setup();
        ret = true;
        sleep( 1 );
    }


    return !ret;
}

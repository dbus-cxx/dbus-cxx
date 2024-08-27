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
// #include <dbus-cxx.h>
#include <memory>
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
std::shared_ptr<DBus::MethodProxy<std::map<DBus::Path, std::map<std::string, std::map<std::string, DBus::Variant>>>()>> complex_method_proxy;
std::shared_ptr<DBus::MethodProxy<bool( std::tuple<int, double, std::string> )>> tuple_method_proxy;
std::shared_ptr<DBus::MethodProxy<void( DBus::Variant )>> variant_proxy;
std::shared_ptr<DBus::MethodProxy<void()>> nonexistant_proxy;
std::shared_ptr<DBus::MethodProxy<DBus::MultipleReturn<int32_t, int32_t, std::string, std::vector<int>>()>> multiplereturn_method_proxy;
std::shared_ptr<DBus::MethodProxy<DBus::MultipleReturn<int32_t>(int32_t)>> multiplereturn_method_proxy2;
std::shared_ptr<DBus::MethodProxy<std::string()>> error_method_proxy;
std::shared_ptr<DBus::MethodProxy<bool( std::tuple<int, std::tuple<int, uint32_t>, double, std::string>)>> nested_tuple_method_proxy;

std::shared_ptr<DBus::Object> object;
std::shared_ptr<DBus::Method<int( int, int )>> int_method;
std::shared_ptr<DBus::Method<void()>> void_method;
std::shared_ptr<DBus::Method<void( struct custom )>> void_custom_method;
std::shared_ptr<DBus::Method<int( int, struct custom )>> int_custom_method2;
std::shared_ptr<DBus::Method<std::map<DBus::Path, std::map<std::string, std::map<std::string,DBus::Variant>>>()>> complex_method;
std::shared_ptr<DBus::Method<bool( std::tuple<int, double, std::string> )>> tuple_method;
std::shared_ptr<DBus::Method<void( DBus::Variant )>> variant_method;
std::shared_ptr<DBus::Method<DBus::MultipleReturn<int32_t, int32_t, std::string, std::vector<int>>()>> multiplereturn_method;
std::shared_ptr<DBus::Method<DBus::MultipleReturn<int32_t>(int32_t)>> multiplereturn_method2;
std::shared_ptr<DBus::Method<std::string()>> error_method;
std::shared_ptr<DBus::Method<bool( std::tuple<int, std::tuple<int, uint32_t>, double, std::string>)>> nested_tuple_method;


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

std::map<DBus::Path, std::map<std::string, std::map<std::string, DBus::Variant>>> complex_custom_symbol() {
    std::vector<DBus::Path> map1Keys = { "/foo/org/1", "/foo/org/blah" };
    std::vector<std::string> map2Keys = { "string1", "a_longer_string", "x" };
    std::vector<std::string> map3Keys = { "inner3-1 FIRST", "inner3-2", "inner3-3 num3" };
    std::map<DBus::Path, std::map<std::string, std::map<std::string, DBus::Variant>>> result;

    for( DBus::Path path : map1Keys ){
        std::map<std::string, std::map<std::string, DBus::Variant>> map2;
        for( std::string map2_key : map2Keys ){
            std::map<std::string, DBus::Variant> map3;
            for( std::string map3_key : map3Keys ){
                map3[ map3_key ] = DBus::Variant( "this is a string" );
            }

            std::map<int, int> complexVariantMap;
            complexVariantMap[5] = 6;
            map3[ "complex-variant" ] = DBus::Variant(complexVariantMap);

            map2[ map2_key ] = map3;
        }

        result[ path ] = map2;
    }

    return result;
}

DBus::MultipleReturn<int32_t, int32_t, std::string, std::vector<int>>
multiplereturn_symbol() {
    return DBus::MultipleReturn<int32_t, int32_t, std::string, std::vector<int>>(1, 1024, "test", {1, 2, 3, 4} );
}

DBus::MultipleReturn<int32_t>
multiplereturn_symbol2(int32_t arg) {
    return DBus::MultipleReturn<int32_t>( arg + 10 );
}

void variant_method_symbol( DBus::Variant v ) {
    std::cerr << "Got incoming variant: " << v << std::endl;
}

std::string error_method_symbol(){
    throw std::runtime_error("error_message_here");
}

bool nested_tuple_method_symbol(std::tuple<int, std::tuple<int, uint32_t>, double, std::string> arg) {
    return true;
}

void client_setup() {
    proxy = conn->create_object_proxy( "dbuscxx.test", "/test" );

    int_method_proxy = proxy->create_method<int( int, int )>( "foo.what", "add" );
    void_method_proxy = proxy->create_method<void()>( "foo.what", "void" );
    void_custom_method_proxy = proxy->create_method<void( struct custom )>( "foo.what", "void_custom" );
    int_custom_method_proxy2 = proxy->create_method<int( int, struct custom )>( "foo.what", "int_intcustom" );
    complex_method_proxy = proxy->create_method<std::map<DBus::Path, std::map<std::string, std::map<std::string, DBus::Variant>>>()>( "foo.what", "complex_custom" );
    tuple_method_proxy = proxy->create_method<bool( std::tuple<int, double, std::string> )>( "foo.what", "tuple_method" );
    variant_proxy = proxy->create_method<void( DBus::Variant )>( "foo.what", "variant_method" );
    nonexistant_proxy = proxy->create_method<void()>( "foo.what", "nonexistant" );
    multiplereturn_method_proxy = proxy->create_method<DBus::MultipleReturn<int32_t, int32_t, std::string, std::vector<int>>()>( "foo.what", "multiplereturn" );
    multiplereturn_method_proxy2 = proxy->create_method<DBus::MultipleReturn<int32_t>(int32_t)>( "foo.what", "multiplereturn2" );
    error_method_proxy = proxy->create_method<std::string()>( "foo.what", "error_method" );
    nested_tuple_method_proxy = proxy->create_method<bool( std::tuple<int, std::tuple<int, uint32_t>, double, std::string>)>( "foo.what", "nested_tuple_method" );
}

void server_setup() {
    DBus::RequestNameResponse ret = conn->request_name( "dbuscxx.test", DBUSCXX_NAME_FLAG_REPLACE_EXISTING );

    if( ret != DBus::RequestNameResponse::PrimaryOwner ) { exit( 1 ); }

    object = conn->create_object( "/test", DBus::ThreadForCalling::DispatcherThread );
    int_method = object->create_method<int( int, int )>( "foo.what", "add", sigc::ptr_fun( add ) );
    void_method = object->create_method<void()>( "foo.what", "void", sigc::ptr_fun( void_method_symbol ) );
    void_custom_method = object->create_method<void( struct custom )>( "foo.what", "void_custom", sigc::ptr_fun( void_custom_method_symbol ) );
    int_custom_method2 = object->create_method<int( int, struct custom )>( "foo.what", "int_intcustom", sigc::ptr_fun( int_intcustom_symbol ) );
    complex_method = object->create_method<std::map<DBus::Path, std::map<std::string, std::map<std::string, DBus::Variant>>>()>( "foo.what", "complex_custom", sigc::ptr_fun( complex_custom_symbol ) );
    tuple_method = object->create_method<bool( std::tuple<int, double, std::string> )>( "foo.what", "tuple_method", sigc::ptr_fun( tuple_method_symbol ) );
    variant_method = object->create_method<void( DBus::Variant )>( "foo.what", "variant_method", sigc::ptr_fun( variant_method_symbol ) );
    multiplereturn_method = object->create_method<DBus::MultipleReturn<int32_t, int32_t, std::string, std::vector<int>>()>( "foo.what", "multiplereturn", sigc::ptr_fun( multiplereturn_symbol ) );
    multiplereturn_method2 = object->create_method<DBus::MultipleReturn<int32_t>(int32_t)>( "foo.what", "multiplereturn2", sigc::ptr_fun( multiplereturn_symbol2 ) );
    error_method = object->create_method<std::string()>( "foo.what", "error_method", sigc::ptr_fun( error_method_symbol ) );
    nested_tuple_method = object->create_method<bool( std::tuple<int, std::tuple<int, uint32_t>, double, std::string>)>( "foo.what", "nested_tuple_method", sigc::ptr_fun( nested_tuple_method_symbol ) );
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

bool data_send_complex() {
    std::map<DBus::Path, std::map<std::string, std::map<std::string, DBus::Variant>>> val = ( *complex_method_proxy )();

    //return TEST_EQUALS( val[0].at("s1"), 32 );
    return true;
}

bool data_send_multiplereturn() {
    DBus::MultipleReturn<int32_t, int32_t, std::string, std::vector<int>> val;
    val = (*multiplereturn_method_proxy)();
    std::tuple<int32_t, int32_t, std::string, std::vector<int>> d = val.m_data;
    return (TEST_EQUALS(std::get<0>(d), 1)) &
           (TEST_EQUALS(std::get<1>(d), 1024)) &
           (TEST_EQUALS(std::get<2>(d), std::string("test"))) &
           (TEST_EQUALS(std::get<3>(d), std::vector<int>({1, 2, 3, 4})));
}

bool data_send_multiplereturn2() {
    DBus::MultipleReturn<int32_t> val;
    val = (*multiplereturn_method_proxy2)(5);
    std::tuple<int32_t> d = val.m_data;
    return (TEST_EQUALS(std::get<0>(d), 15));
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

bool data_error_return() {
    try{
        std::string str = ( *error_method_proxy )();
    } catch( DBus::Error e ){
        std::string what(e.what());
        if( what == "error_message_here" ){
            return true;
        }
    }

    return false;
}

bool data_nested_tuple() {
    std::tuple<int, uint32_t> nested_tuple =
        std::make_tuple(22, 33);

    std::tuple<int, std::tuple<int, uint32_t>, double, std::string> tuple =
        std::make_tuple(00, nested_tuple, 1.0, "Dette");

    auto reuslt = (*nested_tuple_method_proxy)(tuple);

    return reuslt;
}

#define ADD_TEST(name) do{ if( test_name == STRINGIFY(name) ){ \
            ret = data_##name();\
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
        ADD_TEST( send_integers );
        ADD_TEST( void_method );
        ADD_TEST( void_custom );
        ADD_TEST( send_intcustom );
        ADD_TEST( send_complex );
        ADD_TEST( ping );
        ADD_TEST( machine_uuid );
        ADD_TEST( tuple );
        ADD_TEST( variant_array );
        ADD_TEST( variant_map );
        ADD_TEST( variant_tuple );
        ADD_TEST( nonexistant_method );
        ADD_TEST(send_multiplereturn);
        ADD_TEST(send_multiplereturn2);
        ADD_TEST(error_return);
        ADD_TEST(nested_tuple);

    	std::cout << "Test case \"" + test_name + "\" " + (ret ? "PASSED" : "FAIL") << std::endl;

    } else {
        server_setup();
        ret = true;
        sleep( 1 );
    }

    return !ret;
}

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
#include <unistd.h>
#include <dbus-cxx.h>
#include <expat.h>
#include <stack>

#include "test_macros.h"

std::shared_ptr<DBus::Dispatcher> dispatch;
std::shared_ptr<DBus::Connection> conn;

std::shared_ptr<DBus::ObjectProxy> proxy;
std::shared_ptr<DBus::MethodProxy<int( int, int )>> int_method_proxy;
std::shared_ptr<DBus::ObjectProxy> introspectionProxy;
std::shared_ptr<DBus::MethodProxy<std::string()>> introspection_method_proxy;

std::shared_ptr<DBus::Object> object;
std::shared_ptr<DBus::Method<int( int, int )>> int_method;

struct XMLParseResults {
    XMLParseResults() :
        found_iface_foowhat( false ),
        found_iface_whatbob( false ),
        found_add_method_under_foowhat( false ),
        first_arg_name_correct( false ),
        second_arg_name_correct( false ),
        found_signal( false ),
        num_multi_return( 0 )
    {}

    bool found_iface_foowhat;
    bool found_iface_whatbob;
    bool found_add_method_under_foowhat;
    bool first_arg_name_correct;
    bool second_arg_name_correct;
    bool found_signal;
    int num_multi_return;
};

XMLParseResults parseResults;
std::stack<std::string> tagStack;
std::string currentInterface;
std::string currentMethod;
std::string currentSignal;
int argNum;

//
// Functions for the method proxies
//
int add( int, int ) {
    return 1;
}

void doublefun( double ) {}

DBus::MultipleReturn<int32_t,std::string> multi_return(){
    return DBus::MultipleReturn<int32_t,std::string>( 5, "hi" );
}

//
// XML parsing functions
//
static void startElement( void*, const XML_Char* name, const XML_Char** attrs ) {
    std::string elemName = std::string( name );
    std::map<std::string, std::string> tagAttrs;
    int i = 0;

    tagStack.push( elemName );

    while( attrs[ i ] != NULL ) {
        std::string attrKey( attrs[ i ] );
        std::string attrValue( attrs[ i + 1 ] );

        tagAttrs.insert( std::make_pair( attrKey, attrValue ) );

        i += 2;
    }

    if( elemName == "interface" ) {
        if( tagAttrs[ "name" ] == "foo.what" ) {
            parseResults.found_iface_foowhat = true;
        }

        if( tagAttrs[ "name" ] == "what.bob" ) {
            parseResults.found_iface_whatbob = true;
        }

        currentInterface = tagAttrs[ "name" ];
    }

    if( elemName == "method" ) {
        if( tagAttrs[ "name" ] == "add" && currentInterface == "foo.what" ) {
            parseResults.found_add_method_under_foowhat = true;
        }

        currentMethod = tagAttrs[ "name" ];
        argNum = 0;
    }

    if( elemName == "arg" ) {
        if( tagAttrs[ "name" ] == "first" && argNum == 1 && currentMethod == "add" ) {
            parseResults.first_arg_name_correct = true;
        }

        if( tagAttrs[ "name" ] == "second" && argNum == 2 && currentMethod == "add" ) {
            parseResults.second_arg_name_correct = true;
        }

        if( currentMethod == "multi" ){
            parseResults.num_multi_return++;
        }

        argNum++;
    }

    if( elemName == "signal" ) {
        if( tagAttrs[ "name" ] == "tim" && currentInterface == "sig.interface" ) {
            parseResults.found_signal = true;
        }

        currentSignal = tagAttrs[ "name" ];
        argNum = 0;
    }
}

static void endElement( void*, const XML_Char* name ) {
    tagStack.pop();
}

//
// Test Functions
//
bool introspect_basic_introspect() {
    std::string introspectionData = ( *introspection_method_proxy )();

    return introspectionData.size() > 10;
}

bool introspect_valid_xml() {
    std::string introspectionData = ( *introspection_method_proxy )();

    XML_Parser parser = XML_ParserCreate( NULL );
    XML_SetElementHandler( parser, startElement, endElement );

    if( XML_Parse( parser, introspectionData.c_str(), introspectionData.size(), 1 ) == XML_STATUS_ERROR ) {
        return false;
    }

    XML_ParserFree( parser );
    return true;
}

bool introspect_validate_interfaces() {
    std::string introspectionData = ( *introspection_method_proxy )();

    XML_Parser parser = XML_ParserCreate( NULL );
    XML_SetElementHandler( parser, startElement, endElement );

    if( XML_Parse( parser, introspectionData.c_str(), introspectionData.size(), 1 ) == XML_STATUS_ERROR ) {
        return false;
    }

    XML_ParserFree( parser );
    return parseResults.found_iface_foowhat && parseResults.found_iface_whatbob;
}

bool introspect_first_argname_correct() {
    std::string introspectionData = ( *introspection_method_proxy )();

    XML_Parser parser = XML_ParserCreate( NULL );
    XML_SetElementHandler( parser, startElement, endElement );

    if( XML_Parse( parser, introspectionData.c_str(), introspectionData.size(), 1 ) == XML_STATUS_ERROR ) {
        return false;
    }

    XML_ParserFree( parser );
    return parseResults.first_arg_name_correct;
}

bool introspect_second_argname_correct() {
    std::string introspectionData = ( *introspection_method_proxy )();

    XML_Parser parser = XML_ParserCreate( NULL );
    XML_SetElementHandler( parser, startElement, endElement );

    if( XML_Parse( parser, introspectionData.c_str(), introspectionData.size(), 1 ) == XML_STATUS_ERROR ) {
        return false;
    }

    XML_ParserFree( parser );
    return parseResults.second_arg_name_correct;
}

bool introspect_add_method_under_foowhat() {
    std::string introspectionData = ( *introspection_method_proxy )();

    XML_Parser parser = XML_ParserCreate( NULL );
    XML_SetElementHandler( parser, startElement, endElement );

    if( XML_Parse( parser, introspectionData.c_str(), introspectionData.size(), 1 ) == XML_STATUS_ERROR ) {
        return false;
    }

    XML_ParserFree( parser );
    return parseResults.found_add_method_under_foowhat;
}

bool introspect_signal_found() {
    std::string introspectionData = ( *introspection_method_proxy )();

    XML_Parser parser = XML_ParserCreate( NULL );
    XML_SetElementHandler( parser, startElement, endElement );

    if( XML_Parse( parser, introspectionData.c_str(), introspectionData.size(), 1 ) == XML_STATUS_ERROR ) {
        return false;
    }

    XML_ParserFree( parser );
    return parseResults.found_signal;
}

bool introspect_multiple_return() {
    std::string introspectionData = ( *introspection_method_proxy )();

    XML_Parser parser = XML_ParserCreate( NULL );
    XML_SetElementHandler( parser, startElement, endElement );

    if( XML_Parse( parser, introspectionData.c_str(), introspectionData.size(), 1 ) == XML_STATUS_ERROR ) {
        return false;
    }

    XML_ParserFree( parser );
    printf( "num multi return %d\n", parseResults.num_multi_return );
    return parseResults.num_multi_return == 2;
}

void client_setup() {
    proxy = conn->create_object_proxy( "dbuscxx.test", "/test" );

    int_method_proxy = proxy->create_method<int( int, int )>( "foo.what", "add" );

    introspectionProxy = conn->create_object_proxy( "dbuscxx.test", "/test" );
    introspection_method_proxy = proxy->create_method<std::string()>( "org.freedesktop.DBus.Introspectable", "Introspect" );
}

void server_setup() {
    DBus::RequestNameResponse ret = conn->request_name( "dbuscxx.test", DBUSCXX_NAME_FLAG_REPLACE_EXISTING );

    if( ret != DBus::RequestNameResponse::PrimaryOwner ) { exit( 1 ); }

    object = conn->create_object( "/test", DBus::ThreadForCalling::DispatcherThread );
    int_method = object->create_method<int( int, int )>( "foo.what", "add", sigc::ptr_fun( add ) );
    int_method->set_arg_name( 1, "first" );
    int_method->set_arg_name( 2, "second" );

    object->create_method<void( double )>( "what.bob", "setBar", sigc::ptr_fun( doublefun ) );

    object->create_signal<void(std::string)>( "sig.interface", "tim" );

    object->create_method<DBus::MultipleReturn<int32_t,std::string>()>( "foo.multi", "multi", sigc::ptr_fun( multi_return ) );
}

#define ADD_TEST(name) do{ if( test_name == STRINGIFY(name) ){ \
            ret = introspect_##name();\
        } \
    } while( 0 )

int main( int argc, char** argv ) {
    if( argc < 1 ) {
        return 1;
    }

    std::string test_name = argv[2];
    bool ret = false;
    bool is_client = std::string( argv[1] ) == "client";

    DBus::set_log_level(SL_TRACE);
    DBus::set_logging_function(DBus::log_std_err);
    dispatch = DBus::StandaloneDispatcher::create();
    conn = dispatch->create_connection( DBus::BusType::SESSION );

    if( is_client ) {
        client_setup();
        ADD_TEST( basic_introspect );
        ADD_TEST( valid_xml );
        ADD_TEST( validate_interfaces );
        ADD_TEST( add_method_under_foowhat );
        ADD_TEST( first_argname_correct );
        ADD_TEST( second_argname_correct );
        ADD_TEST( signal_found );
        ADD_TEST( multiple_return );
    } else {
        server_setup();
        ret = true;
        sleep( 1 );
    }


    return !ret;
}

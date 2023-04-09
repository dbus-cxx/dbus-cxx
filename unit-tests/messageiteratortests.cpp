// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
/***************************************************************************
 *   Copyright (C) 2009 by Rick L. Vinyard, Jr.                            *
 *   rvinyard@cs.nmsu.edu                                                  *
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
#include <cstring>
#include <unistd.h>
#include <dbus-cxx.h>
#include <iostream>

#include "test_macros.h"

template <typename T>
bool test_numeric_call_message_append_extract_iterator( T v ) {
    T v2 = 0;

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << v;

    DBus::MessageIterator iter2( msg );
    v2 = ( T )iter2;

    return TEST_EQUALS( v, v2 );
}

bool call_message_append_extract_iterator_bool() {
    bool v = true;
    return test_numeric_call_message_append_extract_iterator( v );
}

bool call_message_append_extract_iterator_byte() {
    uint8_t v = 67;
    return test_numeric_call_message_append_extract_iterator( v );
}

bool call_message_append_extract_iterator_int16() {
    int16_t v = -16782;
    return test_numeric_call_message_append_extract_iterator( v );
}

bool call_message_append_extract_iterator_uint16() {
    uint16_t v = 16794;
    return test_numeric_call_message_append_extract_iterator( v );
}

bool call_message_append_extract_iterator_int32() {
    int32_t v = -47892;
    return test_numeric_call_message_append_extract_iterator( v );
}

bool call_message_append_extract_iterator_uint32() {
    uint32_t v = 53938;
    return test_numeric_call_message_append_extract_iterator( v );
}

bool call_message_append_extract_iterator_int64() {
    int64_t v = -378983;
    return test_numeric_call_message_append_extract_iterator( v );
}

bool call_message_append_extract_iterator_uint64() {
    uint64_t v = 4924953;
    return test_numeric_call_message_append_extract_iterator( v );
}

bool call_message_append_extract_iterator_double() {
    double v = 3.141592654;
    return test_numeric_call_message_append_extract_iterator( v );
}

bool call_message_append_extract_iterator_cstring() {
    const char* v = "Hello World";
    std::string v2;

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << v;

    DBus::MessageIterator iter2( msg );
    v2 = ( std::string )iter2;

    return TEST_EQUALS( 0, strcmp( v, v2.c_str() ) );
}

bool call_message_append_extract_iterator_string() {
    std::string v( "Hello World" );
    std::string v2;

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << v;

    DBus::MessageIterator iter2( msg );
    v2 = ( std::string )iter2;

    return TEST_EQUALS( v, v2 );
}

bool call_message_append_extract_iterator_signature() {
    DBus::Signature v( "a{iq}" );
    DBus::Signature v2;

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << v;

    DBus::MessageIterator iter2( msg );
    v2 = ( DBus::Signature )iter2;

    return TEST_EQUALS( v, v2 );
}

bool call_message_append_extract_iterator_array_int( ) {
    std::vector<int> v, v2;

    for( int i = 0; i < 35; i++ ) {
        v.push_back( rand() );
    }

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << v;

    DBus::MessageIterator iter2( msg );
    iter2 >> v2;

    TEST_EQUALS_RET_FAIL( v.size(), v2.size() );

    for( int i = 0; i < 35; i++ ) {
        TEST_EQUALS_RET_FAIL( v[i], v2[i] );
    }

    return true;
}

bool call_message_append_extract_iterator_array_string( ) {
    std::vector<std::string> v, v2;
    char string[ 30 ];

    for( int i = 0; i < 35; i++ ) {
        snprintf( string, 30, "string%d", i );
        v.push_back( string );
    }

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << v;

    DBus::MessageIterator iter2( msg );
    iter2 >> v2;

    TEST_EQUALS_RET_FAIL( v.size(), v2.size() );

    for( int i = 0; i < 35; i++ ) {
        TEST_EQUALS_RET_FAIL( v[i], v2[i] );
    }

    return true;
}

bool call_message_append_extract_iterator_array_array_string( ) {
    std::vector<std::vector<std::string>> sv, sv2;
    std::vector<std::string> v, v2;
    char string[ 30 ];

    for( int y = 0; y < 6; y++ ) {
        v.clear();

        for( int i = 0; i < 35; i++ ) {
            snprintf( string, 30, "string%d-%d", i, y );
            v.push_back( string );
        }

        sv.push_back( v );
    }

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << sv;

    DBus::MessageIterator iter2( msg );
    iter2 >> sv2;

    TEST_EQUALS_RET_FAIL( sv.size(), sv2.size() );

    for( int y = 0; y < 6; y++ ) {
        for( int i = 0; i < 35; i++ ) {
            TEST_EQUALS_RET_FAIL( ( sv[ y ] )[i], ( sv2[y] )[i] );
        }
    }

    return true;
}

bool call_message_iterator_insertion_extraction_operator_filedescriptor() {
    std::shared_ptr<DBus::FileDescriptor> v;
    std::shared_ptr<DBus::FileDescriptor> v2;
    int pipes[2];
    const char* firstString = "FirstString";
    const char* secondString = "SecondString";
    const char* appended = "FirstStringSecondString";
    char readData[ 24 ];

    memset( readData, 0, 24 );

    if( pipe( pipes ) < 0 ) {
        return false;
    }

    v = DBus::FileDescriptor::create( pipes[1] );

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << v;

    DBus::MessageIterator iter2( msg );
    v2 = ( std::shared_ptr<DBus::FileDescriptor> )iter2;

    if( write( v->descriptor(), firstString, strlen( firstString ) ) < 0 ) {
        return false;
    }

    if( write( v2->descriptor(), secondString, strlen( secondString ) ) < 0 ) {
        return false;
    }

    if( read( pipes[ 0 ], readData, 23 ) != 23 ) {
        return false;
    }

    return TEST_EQUALS( memcmp( appended, readData, 23 ), 0 );
}

bool call_message_append_extract_iterator_filedescriptor() {
    std::shared_ptr<DBus::FileDescriptor> v;
    std::shared_ptr<DBus::FileDescriptor> v2;
    int pipes[2];
    const char* firstString = "FirstString";
    const char* secondString = "SecondString";
    const char* appended = "FirstStringSecondString";
    char readData[ 24 ];

    memset( readData, 0, 24 );

    if( pipe( pipes ) < 0 ) {
        return false;
    }

    v = DBus::FileDescriptor::create( pipes[1] );

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << v;

    DBus::MessageIterator iter2( msg );
    iter2 >> v2;

    if( write( v->descriptor(), firstString, strlen( firstString ) ) < 0 ) {
        return false;
    }

    if( write( v2->descriptor(), secondString, strlen( secondString ) ) < 0 ) {
        return false;
    }

    if( read( pipes[ 0 ], readData, 23 ) != 23 ) {
        return false;
    }

    return TEST_EQUALS( memcmp( appended, readData, 23 ), 0 );
}

bool call_message_append_extract_iterator_struct() {
    std::tuple<int, uint8_t> struct1 = std::make_tuple( 68, 'o' );
    std::tuple<int, uint8_t> struct2;

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << struct1;

    DBus::MessageIterator iter2( msg );
    struct2 = ( std::tuple<int, uint8_t> )iter2;

    TEST_EQUALS_RET_FAIL( std::get<int>( struct2 ), 68 );
    TEST_EQUALS_RET_FAIL( std::get<uint8_t>( struct2 ), 'o' );

    return true;
}

bool call_message_append_extract_iterator_variant() {
    DBus::Variant var1( 99 );
    DBus::Variant var2;

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << var1;

    DBus::MessageIterator iter2( msg );
    var2 = DBUSCXX_MESSAGEITERATOR_OPERATOR_VARIANT( iter2 );

    TEST_EQUALS_RET_FAIL( var2.to_int32(), 99 );

    return true;
}

bool call_message_append_extract_iterator_variant_vector() {
    std::vector<int> good;
    good.push_back( 5 );
    DBus::Variant var1( good );
    DBus::Variant var2;

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << var1;

    DBus::MessageIterator iter2( msg );
    var2 = DBUSCXX_MESSAGEITERATOR_OPERATOR_VARIANT( iter2 );

    std::vector<int> casted = var2.to_vector<int>();
    TEST_EQUALS_RET_FAIL( good.size(), casted.size() );
    TEST_EQUALS_RET_FAIL( good[0], casted[0] );

    return true;
}

bool call_message_append_extract_iterator_variant_map() {
    std::map<uint16_t, int> good;
    good[ 1 ] = 1001;
    good[ 2 ] = 1002;
    good[ 3 ] = 1003;
    DBus::Variant var1( good );
    DBus::Variant var2;

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << var1;

    DBus::MessageIterator iter2( msg );
    var2 = DBUSCXX_MESSAGEITERATOR_OPERATOR_VARIANT( iter2 );

    std::map<uint16_t, int> casted = var2.to_map<uint16_t, int>();
    TEST_EQUALS_RET_FAIL( good.size(), casted.size() );
    TEST_EQUALS_RET_FAIL( good[0], casted[0] );

    return true;
}

bool call_message_append_extract_iterator_variant_struct() {
    std::tuple<int, std::string, uint16_t> good = { 5, "hi there", 66 };
    DBus::Variant var1( good );
    DBus::Variant var2;

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << var1;

    DBus::MessageIterator iter2( msg );
    var2 = DBUSCXX_MESSAGEITERATOR_OPERATOR_VARIANT( iter2 );

    std::tuple<int, std::string, uint16_t> casted = var2.to_tuple<int, std::string, uint16_t>();
    TEST_EQUALS_RET_FAIL( std::get<0>( casted ), std::get<0>( good ) );
    TEST_EQUALS_RET_FAIL( std::get<1>( casted ), std::get<1>( good ) );
    TEST_EQUALS_RET_FAIL( std::get<2>( casted ), std::get<2>( good ) );

    return true;
}

bool call_message_append_extract_iterator_map_string_string() {
    std::map<std::string, std::string> themap;
    std::map<std::string, std::string> extracted_map;

    themap[ "first" ] = "what";
    themap[ "second" ] = "hi";

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << themap;

    DBus::MessageIterator iter2( msg );
    extracted_map = ( std::map<std::string, std::string> )iter2;

    TEST_EQUALS_RET_FAIL( extracted_map["first"], "what" );
    TEST_EQUALS_RET_FAIL( extracted_map["second"], "hi" );

    return true;
}

bool call_message_append_extract_iterator_map_string_string_many() {
    std::map<std::string, std::string> themap;
    std::map<std::string, std::string> extracted_map;

    themap[ "first" ] = "what";
    themap[ "second" ] = "hi";
    themap[ "RANDOM TEXT" ] = "7";
    themap[ "f" ] = "6";
    themap[ "2" ] = "xkcd";

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << themap;

    DBus::MessageIterator iter2( msg );
    extracted_map = ( std::map<std::string, std::string> )iter2;

    TEST_EQUALS_RET_FAIL( extracted_map["first"], "what" );
    TEST_EQUALS_RET_FAIL( extracted_map["second"], "hi" );
    TEST_EQUALS_RET_FAIL( extracted_map["RANDOM TEXT"], "7" );
    TEST_EQUALS_RET_FAIL( extracted_map["f"], "6" );
    TEST_EQUALS_RET_FAIL( extracted_map["2"], "xkcd" );

    return true;
}

bool call_message_append_extract_iterator_map_string_variant() {
    DBus::Variant var1( 0x12345678 );
    DBus::Variant var2( "hi" );
    std::map<std::string, DBus::Variant> themap;
    std::map<std::string, DBus::Variant> extracted_map;

    themap[ "one" ] = var1;
    themap[ "xcom" ] = var2;

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << themap;

    DBus::MessageIterator iter2( msg );
    extracted_map = ( std::map<std::string, DBus::Variant> )iter2;

    TEST_EQUALS_RET_FAIL( extracted_map["one"].to_int32(), 0x12345678 );
    TEST_EQUALS_RET_FAIL( extracted_map["xcom"].to_string(), "hi" );

    return true;
}

bool call_message_append_extract_iterator_map_string_variant_many() {
    DBus::Variant var1( 0x12345678 );
    DBus::Variant var2( "hi" );
    DBus::Variant var3( static_cast<uint16_t>( 45 ) );
    DBus::Variant var4( "m" );
    DBus::Variant var5( 5.1234 );
    std::map<std::string, DBus::Variant> themap;
    std::map<std::string, DBus::Variant> extracted_map;

    themap[ "one" ] = var1;
    themap[ "xcom" ] = var2;
    themap[ "longstring" ] = var3;
    themap[ "a" ] = var4;
    themap[ "abcd" ] = var5;

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << themap;

    DBus::MessageIterator iter2( msg );
    extracted_map = ( std::map<std::string, DBus::Variant> )iter2;

    TEST_EQUALS_RET_FAIL( extracted_map["one"].to_int32(), 0x12345678 );
    TEST_EQUALS_RET_FAIL( extracted_map["xcom"].to_string(), "hi" );
    TEST_EQUALS_RET_FAIL( extracted_map["longstring"].to_uint16(), 45 );
    TEST_EQUALS_RET_FAIL( extracted_map["a"].to_string(), "m" );
    TEST_EQUALS_RET_FAIL( extracted_map["abcd"].to_double(), 5.1234 );

    return true;
}

bool call_message_append_extract_iterator_correct_variant_signature() {
    DBus::Variant var1( 0x12345678 );
    DBus::Variant var2( "hi" );
    std::map<std::string, DBus::Variant> themap;
    std::map<std::string, DBus::Variant> extracted_map;

    themap[ "one" ] = var1;
    themap[ "xcom" ] = var2;

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << themap;

    TEST_EQUALS_RET_FAIL( msg->signature(), "a{sv}" );

    return true;
}

bool call_message_append_extract_iterator_array_array_bytes(){
    std::vector<std::vector<uint8_t>> sv, sv2;
    std::vector<uint8_t> v, v2;
    const int outer_array_size = 6;
    const int inner_array_size = 35;

    for( int y = 0; y < outer_array_size; y++ ) {
        v.clear();

        for( int i = 0; i < inner_array_size; i++ ) {
            v.push_back( 200 + i );
        }

        sv.push_back( v );
    }

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << sv;

    DBus::MessageIterator iter2( msg );
    iter2 >> sv2;

    TEST_EQUALS_RET_FAIL( sv.size(), sv2.size() );

    for( int y = 0; y < outer_array_size; y++ ) {
        for( int i = 0; i < inner_array_size; i++ ) {
            TEST_EQUALS_RET_FAIL( ( sv[ y ] )[i], ( sv2[y] )[i] );
        }
    }

    return true;
}

bool call_message_append_extract_iterator_array_array_int(){
    std::vector<std::vector<int32_t>> sv, sv2;
    std::vector<int32_t> v, v2;
    const int outer_array_size = 6;
    const int inner_array_size = 35;

    for( int y = 0; y < outer_array_size; y++ ) {
        v.clear();

        for( int i = 0; i < inner_array_size; i++ ) {
            v.push_back( 200 + i * y );
        }

        sv.push_back( v );
    }

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << sv;

    DBus::MessageIterator iter2( msg );
    iter2 >> sv2;

    TEST_EQUALS_RET_FAIL( sv.size(), sv2.size() );

    for( int y = 0; y < outer_array_size; y++ ) {
        for( int i = 0; i < inner_array_size; i++ ) {
            TEST_EQUALS_RET_FAIL( ( sv[ y ] )[i], ( sv2[y] )[i] );
        }
    }

    return true;
}

template <typename T>
bool test_numeric_call_message_iterator_insertion_extraction_operator( T v ) {
    T v2 = 0;

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << v;

    DBus::MessageIterator iter2( msg );
    iter2 >> v2;

    return TEST_EQUALS( v, v2 );
}

bool call_message_iterator_insertion_extraction_operator_bool() {
    bool v = true;
    return test_numeric_call_message_iterator_insertion_extraction_operator( v );
}

bool call_message_iterator_insertion_extraction_operator_byte() {
    uint8_t v = 67;
    return test_numeric_call_message_iterator_insertion_extraction_operator( v );
}

bool call_message_iterator_insertion_extraction_operator_int16() {
    int16_t v = -16782;
    return test_numeric_call_message_iterator_insertion_extraction_operator( v );
}

bool call_message_iterator_insertion_extraction_operator_uint16() {
    uint16_t v = 16794;
    return test_numeric_call_message_iterator_insertion_extraction_operator( v );
}

bool call_message_iterator_insertion_extraction_operator_int32() {
    int32_t v = -47892;
    return test_numeric_call_message_iterator_insertion_extraction_operator( v );
}

bool call_message_iterator_insertion_extraction_operator_uint32() {
    uint32_t v = 53938;
    return test_numeric_call_message_iterator_insertion_extraction_operator( v );
}

bool call_message_iterator_insertion_extraction_operator_int64() {
    int64_t v = -378983;
    return test_numeric_call_message_iterator_insertion_extraction_operator( v );
}

bool call_message_iterator_insertion_extraction_operator_uint64() {
    uint64_t v = 4924953;
    return test_numeric_call_message_iterator_insertion_extraction_operator( v );
}

bool call_message_iterator_insertion_extraction_operator_double() {
    double v = 3.141592654;
    return test_numeric_call_message_iterator_insertion_extraction_operator( v );
}

bool call_message_iterator_insertion_extraction_operator_cstring() {
    const char* v = "Hello World";
    std::string v2;

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << v;

    DBus::MessageIterator iter2( msg );
    iter2 >> v2;

    return TEST_EQUALS( 0, strcmp( v, v2.c_str() ) );
}

bool call_message_iterator_insertion_extraction_operator_string() {
    std::string v( "Hello World" );
    std::string v2;

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << v;

    DBus::MessageIterator iter2( msg );
    iter2 >> v2;

    return TEST_EQUALS( v, v2 );
}

bool call_message_iterator_insertion_extraction_operator_signature() {
    DBus::Signature v( "a{iq}" );
    DBus::Signature v2;

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << v;

    DBus::MessageIterator iter2( msg );
    iter2 >> v2;

    return TEST_EQUALS( v, v2 );
}

bool call_message_iterator_insertion_extraction_operator_variant() {
    DBus::Variant var1( 99 );
    DBus::Variant var2;

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << var1;

    DBus::MessageIterator iter2( msg );
    iter2 >> var2;

    TEST_EQUALS_RET_FAIL( var2.to_int32(), 99 );

    return true;
}

bool call_message_iterator_insertion_extraction_operator_array_string() {
    std::vector<std::string> v, v2;

    v.push_back( "strign1" );
    v.push_back( "string2" );

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );

    iter1 << v;

    DBus::MessageIterator iter2( msg );
    iter2 >> v2;

    TEST_EQUALS_RET_FAIL( v.size(), v2.size() );

    for( int i = 0; i < 2; i++ ) {
        TEST_EQUALS_RET_FAIL( v[i], v2[i] );
    }

    return true;
}

bool call_message_iterator_insertion_extraction_operator_array_int( ) {
    std::vector<int> v, v2;

    for( int i = 0; i < 35; i++ ) {
        v.push_back( rand() );
    }

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << v;

    DBus::MessageIterator iter2( msg );
    iter2 >> v2;

    TEST_EQUALS_RET_FAIL( v.size(), v2.size() );

    for( int i = 0; i < 35; i++ ) {
        TEST_EQUALS_RET_FAIL( v[i], v2[i] );
    }

    return true;
}

bool call_message_append_extract_iterator_multiple( ) {
    bool            b_1    = false, b_2    = true;
    uint8_t         ui8_1  = 67, ui8_2  = 0;
    int16_t         i16_1  = -16782, i16_2  = 0;
    uint16_t        ui16_1 = 16794, ui16_2 = 0;
    int32_t         i32_1  = -47892, i32_2  = 0;
    uint32_t        ui32_1 = 53938, ui32_2 = 0;
    int64_t         i64_1  = -378983, i64_2  = 0;
    uint64_t        ui64_1 = 4924953, ui64_2 = 0;
    double          d_1    = 3.141592654, d_2    = 0.00;
    const char*      cs_1 = "Hello World"; std::string cs_2;
    std::string     s_1( "Hello World" ), s_2( "" );
    DBus::Signature sig_1( "a{iq}" ), sig_2;
    uint8_t            c_1    = 'a', c_2    = '\0';
    uint8_t          i8_1   = 119, i8_2   = 0;
    double           f_1    = -222.43212, f_2    = 0.00;
    std::vector<double> ad_1, ad_2;

    for( int i = 0; i < 35; i++ ) {
        ad_1.push_back( ( double )rand() / ( double )( rand() ) );
    }


    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << b_1;
    iter1 << ui8_1;
    iter1 << i16_1;
    iter1 << ui16_1;
    iter1 << i32_1;
    iter1 << ui32_1;
    iter1 << i64_1;
    iter1 << ui64_1;
    iter1 << d_1;
    iter1 << cs_1;
    iter1 << s_1;
    iter1 << sig_1;
    iter1 << c_1;
    iter1 << i8_1;
    iter1 << f_1;
    iter1 << ad_1;

    DBus::MessageIterator iter2( msg );
    b_2    = ( bool )iter2;              iter2.next();
    ui8_2  = ( uint8_t )iter2;           iter2.next();
    i16_2  = ( int16_t )iter2;           iter2.next();
    ui16_2 = ( uint16_t )iter2;          iter2.next();
    i32_2  = ( int32_t )iter2;           iter2.next();
    ui32_2 = ( uint32_t )iter2;          iter2.next();
    i64_2  = ( int64_t )iter2;           iter2.next();
    ui64_2 = ( uint64_t )iter2;          iter2.next();
    d_2    = ( double )iter2;            iter2.next();
    cs_2   = ( std::string )iter2;       iter2.next();
    s_2    = ( std::string )iter2;       iter2.next();
    sig_2  = ( DBus::Signature )iter2;   iter2.next();
    c_2    = ( uint8_t )iter2;              iter2.next();
    i8_2   = ( uint8_t )iter2;            iter2.next();
    f_2    = ( double )iter2;             iter2.next();
    iter2 >> ad_2;
    /*iter2.get_array(ad_2);*/             iter2.next();

    TEST_EQUALS_RET_FAIL( b_1, b_2 );
    TEST_EQUALS_RET_FAIL( ui8_1, ui8_2 );
    TEST_EQUALS_RET_FAIL( i16_1, i16_2 );
    TEST_EQUALS_RET_FAIL( ui16_1, ui16_2 );
    TEST_EQUALS_RET_FAIL( i32_1, i32_2 );
    TEST_EQUALS_RET_FAIL( ui32_1, ui32_2 );
    TEST_EQUALS_RET_FAIL( i64_1, i64_2 );
    TEST_EQUALS_RET_FAIL( ui64_1, ui64_2 );
    TEST_EQUALS_RET_FAIL( d_1, d_2 );
    TEST_EQUALS_RET_FAIL( 0, strcmp( cs_1, cs_2.c_str() ) );
    TEST_EQUALS_RET_FAIL( s_1, s_2 );
    TEST_EQUALS_RET_FAIL( sig_1, sig_2 );
    TEST_EQUALS_RET_FAIL( c_1, c_2 );
    TEST_EQUALS_RET_FAIL( i8_1, i8_2 );
    TEST_EQUALS_RET_FAIL( f_1, f_2 );

    TEST_EQUALS_RET_FAIL( ad_1.size(), ad_2.size() );

    for( int i = 0; i < 35; i++ ) {
        TEST_EQUALS_RET_FAIL( ad_1[i], ad_2[i] );
    }

    return true;
}

bool call_message_iterator_insertion_extraction_operator_multiple( ) {
    bool            b_1    = false, b_2    = true;
    uint8_t         ui8_1  = 67, ui8_2  = 0;
    int16_t         i16_1  = -16782, i16_2  = 0;
    uint16_t        ui16_1 = 16794, ui16_2 = 0;
    int32_t         i32_1  = -47892, i32_2  = 0;
    uint32_t        ui32_1 = 53938, ui32_2 = 0;
    int64_t         i64_1  = -378983, i64_2  = 0;
    uint64_t        ui64_1 = 4924953, ui64_2 = 0;
    double          d_1    = 3.141592654, d_2    = 0.00;
    const char*      cs_1 = "Hello World"; std::string cs_2;
    std::string     s_1( "Hello World" ), s_2( "" );
    DBus::Signature sig_1( "a{iq}" ), sig_2;
    uint8_t            c_1    = 'a', c_2    = '\0';
    uint8_t          i8_1   = 119, i8_2   = 0;
    double           f_1    = -222.43212, f_2    = 0.00;
    std::vector<double> ad_1, ad_2;

    for( int i = 0; i < 35; i++ ) {
        ad_1.push_back( ( double )rand() / ( double )( rand() ) );
    }

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << b_1 << ui8_1 << i16_1 << ui16_1 << i32_1 << ui32_1 << i64_1 << ui64_1 << d_1 << cs_1 << s_1 << sig_1 << c_1 << i8_1 << f_1 << ad_1;

    DBus::MessageIterator iter2( msg );
    iter2 >> b_2 >> ui8_2 >> i16_2 >> ui16_2 >> i32_2 >> ui32_2 >> i64_2 >> ui64_2 >> d_2 >> cs_2 >> s_2 >> sig_2 >> c_2 >> i8_2 >> f_2 >> ad_2;

    TEST_EQUALS_RET_FAIL( b_1, b_2 );
    TEST_EQUALS_RET_FAIL( ui8_1, ui8_2 );
    TEST_EQUALS_RET_FAIL( i16_1, i16_2 );
    TEST_EQUALS_RET_FAIL( ui16_1, ui16_2 );
    TEST_EQUALS_RET_FAIL( i32_1, i32_2 );
    TEST_EQUALS_RET_FAIL( ui32_1, ui32_2 );
    TEST_EQUALS_RET_FAIL( i64_1, i64_2 );
    TEST_EQUALS_RET_FAIL( ui64_1, ui64_2 );
    TEST_EQUALS_RET_FAIL( d_1, d_2 );
    TEST_EQUALS_RET_FAIL( 0, strcmp( cs_1, cs_2.c_str() ) );
    TEST_EQUALS_RET_FAIL( s_1, s_2 );
    TEST_EQUALS_RET_FAIL( sig_1, sig_2 );
    TEST_EQUALS_RET_FAIL( c_1, c_2 );
    TEST_EQUALS_RET_FAIL( i8_1, i8_2 );
    TEST_EQUALS_RET_FAIL( f_1, f_2 );

    TEST_EQUALS_RET_FAIL( ad_1.size(), ad_2.size() );

    for( int i = 0; i < 35; i++ ) {
        TEST_EQUALS_RET_FAIL( ad_1[i], ad_2[i] );
    }

    return true;
}

bool call_message_iterator_insertion_extraction_operator_struct() {
    std::tuple<int, uint8_t> struct1 = std::make_tuple( 68, 'o' );
    std::tuple<int, uint8_t> struct2;

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << struct1;

    DBus::MessageIterator iter2( msg );
    struct2 = ( std::tuple<int, uint8_t> )iter2;

    TEST_EQUALS_RET_FAIL( std::get<int>( struct2 ), 68 );
    TEST_EQUALS_RET_FAIL( std::get<uint8_t>( struct2 ), 'o' );

    return true;
}

bool call_message_append_extract_iterator_complex_variants(){
    std::vector<DBus::Path> map1Keys = { "/foo/org/1" };
    std::vector<std::string> map2Keys = { "string1"};
    std::vector<std::string> map3Keys = { "inner3-1" };
    std::map<DBus::Path, std::map<std::string, std::map<std::string, DBus::Variant>>> serialize;
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

        serialize[ path ] = map2;
    }

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << serialize;

    std::vector<uint8_t> vec;
    msg->serialize_to_vector( &vec, 0 );

    DBus::MessageIterator iter2( msg );
    result = (std::map<DBus::Path, std::map<std::string, std::map<std::string, DBus::Variant>>>) iter2;

    std::map<int,int> complexVariantMap;
    DBus::Variant complexVariant = result[ "/foo/org/1" ][ "string1" ][ "complex-variant" ];

    complexVariantMap = complexVariant.to_map<int,int>();

    TEST_EQUALS_RET_FAIL( complexVariantMap[5], 6 );

    return true;

}

bool call_message_append_extract_iterator_complex_variants2(){
    // Complex message that contains a bunch of data
    // See: https://github.com/dbus-cxx/dbus-cxx/issues/105
    uint8_t packet_bytes[] = {
      0x6c, 0x02, 0x01, 0x01, 0x50, 0x05, 0x00, 0x00,
      0x51, 0x09, 0x00, 0x00, 0x3d, 0x00, 0x00, 0x00,
      0x06, 0x01, 0x73, 0x00, 0x06, 0x00, 0x00, 0x00,
      0x3a, 0x31, 0x2e, 0x33, 0x34, 0x37, 0x00, 0x00,
      0x05, 0x01, 0x75, 0x00, 0x06, 0x00, 0x00, 0x00,
      0x08, 0x01, 0x67, 0x00, 0x0d, 0x61, 0x7b, 0x6f,
      0x61, 0x7b, 0x73, 0x61, 0x7b, 0x73, 0x76, 0x7d,
      0x7d, 0x7d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x07, 0x01, 0x73, 0x00, 0x04, 0x00, 0x00, 0x00,
      0x3a, 0x31, 0x2e, 0x36, 0x00, 0x00, 0x00, 0x00,
      0x48, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x0a, 0x00, 0x00, 0x00, 0x2f, 0x6f, 0x72, 0x67,
      0x2f, 0x62, 0x6c, 0x75, 0x65, 0x7a, 0x00, 0x00,
      0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x23, 0x00, 0x00, 0x00, 0x6f, 0x72, 0x67, 0x2e,
      0x66, 0x72, 0x65, 0x65, 0x64, 0x65, 0x73, 0x6b,
      0x74, 0x6f, 0x70, 0x2e, 0x44, 0x42, 0x75, 0x73,
      0x2e, 0x49, 0x6e, 0x74, 0x72, 0x6f, 0x73, 0x70,
      0x65, 0x63, 0x74, 0x61, 0x62, 0x6c, 0x65, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x17, 0x00, 0x00, 0x00, 0x6f, 0x72, 0x67, 0x2e,
      0x62, 0x6c, 0x75, 0x65, 0x7a, 0x2e, 0x41, 0x67,
      0x65, 0x6e, 0x74, 0x4d, 0x61, 0x6e, 0x61, 0x67,
      0x65, 0x72, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x19, 0x00, 0x00, 0x00, 0x6f, 0x72, 0x67, 0x2e,
      0x62, 0x6c, 0x75, 0x65, 0x7a, 0x2e, 0x50, 0x72,
      0x6f, 0x66, 0x69, 0x6c, 0x65, 0x4d, 0x61, 0x6e,
      0x61, 0x67, 0x65, 0x72, 0x31, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x0f, 0x00, 0x00, 0x00, 0x2f, 0x6f, 0x72, 0x67,
      0x2f, 0x62, 0x6c, 0x75, 0x65, 0x7a, 0x2f, 0x68,
      0x63, 0x69, 0x30, 0x00, 0xa0, 0x04, 0x00, 0x00,
      0x23, 0x00, 0x00, 0x00, 0x6f, 0x72, 0x67, 0x2e,
      0x66, 0x72, 0x65, 0x65, 0x64, 0x65, 0x73, 0x6b,
      0x74, 0x6f, 0x70, 0x2e, 0x44, 0x42, 0x75, 0x73,
      0x2e, 0x49, 0x6e, 0x74, 0x72, 0x6f, 0x73, 0x70,
      0x65, 0x63, 0x74, 0x61, 0x62, 0x6c, 0x65, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x12, 0x00, 0x00, 0x00, 0x6f, 0x72, 0x67, 0x2e,
      0x62, 0x6c, 0x75, 0x65, 0x7a, 0x2e, 0x41, 0x64,
      0x61, 0x70, 0x74, 0x65, 0x72, 0x31, 0x00, 0x00,
      0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x07, 0x00, 0x00, 0x00, 0x41, 0x64, 0x64, 0x72,
      0x65, 0x73, 0x73, 0x00, 0x01, 0x73, 0x00, 0x00,
      0x11, 0x00, 0x00, 0x00, 0x30, 0x30, 0x3a, 0x31,
      0x41, 0x3a, 0x37, 0x44, 0x3a, 0x44, 0x41, 0x3a,
      0x37, 0x31, 0x3a, 0x31, 0x33, 0x00, 0x00, 0x00,
      0x0b, 0x00, 0x00, 0x00, 0x41, 0x64, 0x64, 0x72,
      0x65, 0x73, 0x73, 0x54, 0x79, 0x70, 0x65, 0x00,
      0x01, 0x73, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
      0x70, 0x75, 0x62, 0x6c, 0x69, 0x63, 0x00, 0x00,
      0x04, 0x00, 0x00, 0x00, 0x4e, 0x61, 0x6d, 0x65,
      0x00, 0x01, 0x73, 0x00, 0x0c, 0x00, 0x00, 0x00,
      0x64, 0x65, 0x6d, 0x6f, 0x6e, 0x73, 0x74, 0x72,
      0x61, 0x74, 0x6f, 0x73, 0x00, 0x00, 0x00, 0x00,
      0x05, 0x00, 0x00, 0x00, 0x41, 0x6c, 0x69, 0x61,
      0x73, 0x00, 0x01, 0x73, 0x00, 0x00, 0x00, 0x00,
      0x0c, 0x00, 0x00, 0x00, 0x64, 0x65, 0x6d, 0x6f,
      0x6e, 0x73, 0x74, 0x72, 0x61, 0x74, 0x6f, 0x73,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x05, 0x00, 0x00, 0x00, 0x43, 0x6c, 0x61, 0x73,
      0x73, 0x00, 0x01, 0x75, 0x00, 0x00, 0x00, 0x00,
      0x04, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x07, 0x00, 0x00, 0x00, 0x50, 0x6f, 0x77, 0x65,
      0x72, 0x65, 0x64, 0x00, 0x01, 0x62, 0x00, 0x00,
      0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x0c, 0x00, 0x00, 0x00, 0x44, 0x69, 0x73, 0x63,
      0x6f, 0x76, 0x65, 0x72, 0x61, 0x62, 0x6c, 0x65,
      0x00, 0x01, 0x62, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x13, 0x00, 0x00, 0x00, 0x44, 0x69, 0x73, 0x63,
      0x6f, 0x76, 0x65, 0x72, 0x61, 0x62, 0x6c, 0x65,
      0x54, 0x69, 0x6d, 0x65, 0x6f, 0x75, 0x74, 0x00,
      0x01, 0x75, 0x00, 0x00, 0xb4, 0x00, 0x00, 0x00,
      0x08, 0x00, 0x00, 0x00, 0x50, 0x61, 0x69, 0x72,
      0x61, 0x62, 0x6c, 0x65, 0x00, 0x01, 0x62, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x0f, 0x00, 0x00, 0x00, 0x50, 0x61, 0x69, 0x72,
      0x61, 0x62, 0x6c, 0x65, 0x54, 0x69, 0x6d, 0x65,
      0x6f, 0x75, 0x74, 0x00, 0x01, 0x75, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x0b, 0x00, 0x00, 0x00, 0x44, 0x69, 0x73, 0x63,
      0x6f, 0x76, 0x65, 0x72, 0x69, 0x6e, 0x67, 0x00,
      0x01, 0x62, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x05, 0x00, 0x00, 0x00, 0x55, 0x55, 0x49, 0x44,
      0x73, 0x00, 0x02, 0x61, 0x73, 0x00, 0x00, 0x00,
      0x89, 0x01, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00,
      0x30, 0x30, 0x30, 0x30, 0x31, 0x31, 0x31, 0x32,
      0x2d, 0x30, 0x30, 0x30, 0x30, 0x2d, 0x31, 0x30,
      0x30, 0x30, 0x2d, 0x38, 0x30, 0x30, 0x30, 0x2d,
      0x30, 0x30, 0x38, 0x30, 0x35, 0x66, 0x39, 0x62,
      0x33, 0x34, 0x66, 0x62, 0x00, 0x00, 0x00, 0x00,
      0x24, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30,
      0x31, 0x31, 0x30, 0x65, 0x2d, 0x30, 0x30, 0x30,
      0x30, 0x2d, 0x31, 0x30, 0x30, 0x30, 0x2d, 0x38,
      0x30, 0x30, 0x30, 0x2d, 0x30, 0x30, 0x38, 0x30,
      0x35, 0x66, 0x39, 0x62, 0x33, 0x34, 0x66, 0x62,
      0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00,
      0x30, 0x30, 0x30, 0x30, 0x31, 0x32, 0x30, 0x30,
      0x2d, 0x30, 0x30, 0x30, 0x30, 0x2d, 0x31, 0x30,
      0x30, 0x30, 0x2d, 0x38, 0x30, 0x30, 0x30, 0x2d,
      0x30, 0x30, 0x38, 0x30, 0x35, 0x66, 0x39, 0x62,
      0x33, 0x34, 0x66, 0x62, 0x00, 0x00, 0x00, 0x00,
      0x24, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30,
      0x31, 0x31, 0x30, 0x62, 0x2d, 0x30, 0x30, 0x30,
      0x30, 0x2d, 0x31, 0x30, 0x30, 0x30, 0x2d, 0x38,
      0x30, 0x30, 0x30, 0x2d, 0x30, 0x30, 0x38, 0x30,
      0x35, 0x66, 0x39, 0x62, 0x33, 0x34, 0x66, 0x62,
      0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00,
      0x30, 0x30, 0x30, 0x30, 0x31, 0x31, 0x30, 0x38,
      0x2d, 0x30, 0x30, 0x30, 0x30, 0x2d, 0x31, 0x30,
      0x30, 0x30, 0x2d, 0x38, 0x30, 0x30, 0x30, 0x2d,
      0x30, 0x30, 0x38, 0x30, 0x35, 0x66, 0x39, 0x62,
      0x33, 0x34, 0x66, 0x62, 0x00, 0x00, 0x00, 0x00,
      0x24, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30,
      0x31, 0x31, 0x30, 0x63, 0x2d, 0x30, 0x30, 0x30,
      0x30, 0x2d, 0x31, 0x30, 0x30, 0x30, 0x2d, 0x38,
      0x30, 0x30, 0x30, 0x2d, 0x30, 0x30, 0x38, 0x30,
      0x35, 0x66, 0x39, 0x62, 0x33, 0x34, 0x66, 0x62,
      0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00,
      0x30, 0x30, 0x30, 0x30, 0x31, 0x38, 0x30, 0x30,
      0x2d, 0x30, 0x30, 0x30, 0x30, 0x2d, 0x31, 0x30,
      0x30, 0x30, 0x2d, 0x38, 0x30, 0x30, 0x30, 0x2d,
      0x30, 0x30, 0x38, 0x30, 0x35, 0x66, 0x39, 0x62,
      0x33, 0x34, 0x66, 0x62, 0x00, 0x00, 0x00, 0x00,
      0x24, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30,
      0x31, 0x31, 0x30, 0x61, 0x2d, 0x30, 0x30, 0x30,
      0x30, 0x2d, 0x31, 0x30, 0x30, 0x30, 0x2d, 0x38,
      0x30, 0x30, 0x30, 0x2d, 0x30, 0x30, 0x38, 0x30,
      0x35, 0x66, 0x39, 0x62, 0x33, 0x34, 0x66, 0x62,
      0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00,
      0x30, 0x30, 0x30, 0x30, 0x31, 0x38, 0x30, 0x31,
      0x2d, 0x30, 0x30, 0x30, 0x30, 0x2d, 0x31, 0x30,
      0x30, 0x30, 0x2d, 0x38, 0x30, 0x30, 0x30, 0x2d,
      0x30, 0x30, 0x38, 0x30, 0x35, 0x66, 0x39, 0x62,
      0x33, 0x34, 0x66, 0x62, 0x00, 0x00, 0x00, 0x00,
      0x08, 0x00, 0x00, 0x00, 0x4d, 0x6f, 0x64, 0x61,
      0x6c, 0x69, 0x61, 0x73, 0x00, 0x01, 0x73, 0x00,
      0x13, 0x00, 0x00, 0x00, 0x75, 0x73, 0x62, 0x3a,
      0x76, 0x31, 0x44, 0x36, 0x42, 0x70, 0x30, 0x32,
      0x34, 0x36, 0x64, 0x30, 0x35, 0x33, 0x35, 0x00,
      0x1f, 0x00, 0x00, 0x00, 0x6f, 0x72, 0x67, 0x2e,
      0x66, 0x72, 0x65, 0x65, 0x64, 0x65, 0x73, 0x6b,
      0x74, 0x6f, 0x70, 0x2e, 0x44, 0x42, 0x75, 0x73,
      0x2e, 0x50, 0x72, 0x6f, 0x70, 0x65, 0x72, 0x74,
      0x69, 0x65, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x16, 0x00, 0x00, 0x00, 0x6f, 0x72, 0x67, 0x2e,
      0x62, 0x6c, 0x75, 0x65, 0x7a, 0x2e, 0x47, 0x61,
      0x74, 0x74, 0x4d, 0x61, 0x6e, 0x61, 0x67, 0x65,
      0x72, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x1f, 0x00, 0x00, 0x00, 0x6f, 0x72, 0x67, 0x2e,
      0x62, 0x6c, 0x75, 0x65, 0x7a, 0x2e, 0x4c, 0x45,
      0x41, 0x64, 0x76, 0x65, 0x72, 0x74, 0x69, 0x73,
      0x69, 0x6e, 0x67, 0x4d, 0x61, 0x6e, 0x61, 0x67,
      0x65, 0x72, 0x31, 0x00, 0x87, 0x00, 0x00, 0x00,
      0x0f, 0x00, 0x00, 0x00, 0x41, 0x63, 0x74, 0x69,
      0x76, 0x65, 0x49, 0x6e, 0x73, 0x74, 0x61, 0x6e,
      0x63, 0x65, 0x73, 0x00, 0x01, 0x79, 0x00, 0x00,
      0x12, 0x00, 0x00, 0x00, 0x53, 0x75, 0x70, 0x70,
      0x6f, 0x72, 0x74, 0x65, 0x64, 0x49, 0x6e, 0x73,
      0x74, 0x61, 0x6e, 0x63, 0x65, 0x73, 0x00, 0x01,
      0x79, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x11, 0x00, 0x00, 0x00, 0x53, 0x75, 0x70, 0x70,
      0x6f, 0x72, 0x74, 0x65, 0x64, 0x49, 0x6e, 0x63,
      0x6c, 0x75, 0x64, 0x65, 0x73, 0x00, 0x02, 0x61,
      0x73, 0x00, 0x00, 0x00, 0x2f, 0x00, 0x00, 0x00,
      0x08, 0x00, 0x00, 0x00, 0x74, 0x78, 0x2d, 0x70,
      0x6f, 0x77, 0x65, 0x72, 0x00, 0x00, 0x00, 0x00,
      0x0a, 0x00, 0x00, 0x00, 0x61, 0x70, 0x70, 0x65,
      0x61, 0x72, 0x61, 0x6e, 0x63, 0x65, 0x00, 0x00,
      0x0a, 0x00, 0x00, 0x00, 0x6c, 0x6f, 0x63, 0x61,
      0x6c, 0x2d, 0x6e, 0x61, 0x6d, 0x65, 0x00, 0x00,
      0x10, 0x00, 0x00, 0x00, 0x6f, 0x72, 0x67, 0x2e,
      0x62, 0x6c, 0x75, 0x65, 0x7a, 0x2e, 0x4d, 0x65,
      0x64, 0x69, 0x61, 0x31, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x18, 0x00, 0x00, 0x00, 0x6f, 0x72, 0x67, 0x2e,
      0x62, 0x6c, 0x75, 0x65, 0x7a, 0x2e, 0x4e, 0x65,
      0x74, 0x77, 0x6f, 0x72, 0x6b, 0x53, 0x65, 0x72,
      0x76, 0x65, 0x72, 0x31, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    std::shared_ptr<DBus::Message> message = DBus::Message::create_from_data( packet_bytes, sizeof(packet_bytes) );

    // The signature for this message is: a{oa{sa{sv}}}
    std::map<DBus::Path, std::map<std::string, std::map<std::string, DBus::Variant>>> result;
    DBus::MessageIterator iter2( message );
    result = (std::map<DBus::Path, std::map<std::string, std::map<std::string, DBus::Variant>>>) iter2;

    for (const auto & [path, path_map] : result){
        std::cout << path << "\n";
        for (const auto & [str, map2] : path_map){
            std::cout << "    " << str << "\n";
            for (const auto & [str2, variant] : map2){
                std::cout << "        " << str2 << ":" << variant << "\n";

            }
        }
    }

    // Test a few of the returned values, make sure that they make sense
    TEST_EQUALS_RET_FAIL( result.size(), 2 );
    if( !TEST_STREQUALS( result["/org/bluez/hci0"]["org.bluez.Adapter1"]["Address"].to_string(), "00:1A:7D:DA:71:13" ) ){
        return false;
    }
    std::vector<std::string> uuid_array = result["/org/bluez/hci0"]["org.bluez.Adapter1"]["UUIDs"];
    TEST_EQUALS_RET_FAIL( uuid_array.size(), 9 );

    return true;
}

bool call_message_append_extract_iterator_nested_map() {
    std::map<std::string, std::map<std::string, int>> objects1{
        {
            "/org/bluez",
            std::map< std::string, int >{
                {"org.bluez.AgentManager1", 1},
                {"org.bluez.ProfileManager1", 2},
                {"org.freedesktop.DBus.Introspectable", 3},
            },
        },
        {
            "/org/bluez/hci0",
            std::map< std::string, int >{
                {"org.bluez.Adapter1", 4},
                {"org.bluez.GattManager1", 5},
                {"org.bluez.LEAdvertisingManager1", 6},
                {"org.bluez.Media1", 7},
                {"org.bluez.NetworkServer1", 8},
                {"org.freedesktop.DBus.Introspectable", 9},
                {"org.freedesktop.DBus.Properties", 10},
            },
        },
        {
            "/org/bluez/hci0/dev_B3_FD_3F_52_43_2C",
            std::map< std::string, int >{
                {"org.bluez.Device1", 11},
                {"org.bluez.MediaControl1", 12},
                {"org.freedesktop.DBus.Introspectable", 13},
                {"org.freedesktop.DBus.Properties", 14},
            },
        },
    };
    std::map<std::string, std::map<std::string, int>> objects2;

    std::shared_ptr<DBus::CallMessage> msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
    DBus::MessageAppendIterator iter1( msg );
    iter1 << objects1;

    DBus::MessageIterator iter2( msg );
    objects2 = (std::map<std::string, std::map<std::string, int>>)iter2;

    TEST_EQUALS_RET_FAIL( objects1, objects2 );

    return true;
}

#define ADD_TEST(name) do{ if( test_name == STRINGIFY(name) ){ \
            ret = call_message_append_extract_iterator_##name();\
        } \
    } while( 0 )

#define ADD_TEST2(name) do{ if( test_name == STRINGIFY(name)"-2" ){ \
            ret = call_message_iterator_insertion_extraction_operator_##name();\
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

    ADD_TEST( bool );
    ADD_TEST( byte );
    ADD_TEST( int16 );
    ADD_TEST( uint16 );
    ADD_TEST( int32 );
    ADD_TEST( uint32 );
    ADD_TEST( int64 );
    ADD_TEST( uint64 );
    ADD_TEST( double );
    ADD_TEST( cstring );
    ADD_TEST( string );
    ADD_TEST( signature );
    ADD_TEST( array_int );
    ADD_TEST( array_string );
    ADD_TEST( array_array_string );
    ADD_TEST( filedescriptor );
    ADD_TEST( multiple );
    ADD_TEST( struct );
    ADD_TEST( variant );
    ADD_TEST( variant_vector );
    ADD_TEST( variant_map );
    ADD_TEST( variant_struct );
    ADD_TEST( map_string_variant );
    ADD_TEST( map_string_variant_many );
    ADD_TEST( map_string_string );
    ADD_TEST( map_string_string_many );
    ADD_TEST( correct_variant_signature );
    ADD_TEST( array_array_bytes );
    ADD_TEST( array_array_int );
    ADD_TEST( complex_variants );
    ADD_TEST( complex_variants2 );
    ADD_TEST( nested_map );

    ADD_TEST2( bool );
    ADD_TEST2( byte );
    ADD_TEST2( int16 );
    ADD_TEST2( uint16 );
    ADD_TEST2( int32 );
    ADD_TEST2( uint32 );
    ADD_TEST2( int64 );
    ADD_TEST2( uint64 );
    ADD_TEST2( double );
    ADD_TEST2( cstring );
    ADD_TEST2( string );
    ADD_TEST2( signature );
    ADD_TEST2( array_int );
    ADD_TEST2( array_string );
    ADD_TEST2( filedescriptor );
    ADD_TEST2( multiple );
    ADD_TEST2( struct );
    ADD_TEST2( variant );

    return !ret;
}

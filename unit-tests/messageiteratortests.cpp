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
    DBus::set_log_level( SL_DEBUG );

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

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
#ifndef MESSAGEITERATORTESTS_H
#define MESSAGEITERATORTESTS_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include <dbus-cxx.h>

class MessageIteratorTests : public  CppUnit::TestFixture {
  public:

    CPPUNIT_TEST_SUITE( MessageIteratorTests );

    CPPUNIT_TEST( call_message_append_extract_iterator_bool );
    CPPUNIT_TEST( call_message_append_extract_iterator_byte );
    CPPUNIT_TEST( call_message_append_extract_iterator_int16 );
    CPPUNIT_TEST( call_message_append_extract_iterator_uint16 );
    CPPUNIT_TEST( call_message_append_extract_iterator_int32 );
    CPPUNIT_TEST( call_message_append_extract_iterator_uint32 );
    CPPUNIT_TEST( call_message_append_extract_iterator_int64 );
    CPPUNIT_TEST( call_message_append_extract_iterator_uint64 );
    CPPUNIT_TEST( call_message_append_extract_iterator_double );
    CPPUNIT_TEST( call_message_append_extract_iterator_cstring );
    CPPUNIT_TEST( call_message_append_extract_iterator_string );
    CPPUNIT_TEST( call_message_append_extract_iterator_signature );
    CPPUNIT_TEST( call_message_append_extract_iterator_char );
    CPPUNIT_TEST( call_message_append_extract_iterator_int8 );
    CPPUNIT_TEST( call_message_append_extract_iterator_float );
    CPPUNIT_TEST( call_message_append_extract_iterator_long_int );
    CPPUNIT_TEST( call_message_append_extract_iterator_unsigned_long_int );
    CPPUNIT_TEST( call_message_append_extract_iterator_array_int );
    CPPUNIT_TEST( call_message_append_extract_iterator_array_string );
    CPPUNIT_TEST( call_message_append_extract_iterator_array_array_string );
    
    CPPUNIT_TEST( call_message_iterator_insertion_extraction_operator_bool );
    CPPUNIT_TEST( call_message_iterator_insertion_extraction_operator_byte );
    CPPUNIT_TEST( call_message_iterator_insertion_extraction_operator_int16 );
    CPPUNIT_TEST( call_message_iterator_insertion_extraction_operator_uint16 );
    CPPUNIT_TEST( call_message_iterator_insertion_extraction_operator_int32 );
    CPPUNIT_TEST( call_message_iterator_insertion_extraction_operator_uint32 );
    CPPUNIT_TEST( call_message_iterator_insertion_extraction_operator_int64 );
    CPPUNIT_TEST( call_message_iterator_insertion_extraction_operator_uint64 );
    CPPUNIT_TEST( call_message_iterator_insertion_extraction_operator_double );
    CPPUNIT_TEST( call_message_iterator_insertion_extraction_operator_cstring );
    CPPUNIT_TEST( call_message_iterator_insertion_extraction_operator_string );
    CPPUNIT_TEST( call_message_iterator_insertion_extraction_operator_signature );
    CPPUNIT_TEST( call_message_iterator_insertion_extraction_operator_char );
    CPPUNIT_TEST( call_message_iterator_insertion_extraction_operator_int8 );
    CPPUNIT_TEST( call_message_iterator_insertion_extraction_operator_float );
    CPPUNIT_TEST( call_message_iterator_insertion_extraction_operator_long_int );
    CPPUNIT_TEST( call_message_iterator_insertion_extraction_operator_unsigned_long_int );
    CPPUNIT_TEST( call_message_iterator_insertion_extraction_operator_array_string );

    CPPUNIT_TEST( call_message_append_extract_iterator_multiple );
    CPPUNIT_TEST( call_message_iterator_insertion_extraction_operator_multiple );
    CPPUNIT_TEST( call_message_iterator_insertion_extraction_operator_array_int );

    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp();

    void tearDown();

    void call_message_append_extract_iterator_bool();
    void call_message_append_extract_iterator_byte();
    void call_message_append_extract_iterator_int16();
    void call_message_append_extract_iterator_uint16();
    void call_message_append_extract_iterator_int32();
    void call_message_append_extract_iterator_uint32();
    void call_message_append_extract_iterator_int64();
    void call_message_append_extract_iterator_uint64();
    void call_message_append_extract_iterator_double();
    void call_message_append_extract_iterator_cstring();
    void call_message_append_extract_iterator_string();
    void call_message_append_extract_iterator_signature();
    void call_message_append_extract_iterator_char();
    void call_message_append_extract_iterator_int8();
    void call_message_append_extract_iterator_float();
    void call_message_append_extract_iterator_long_int();
    void call_message_append_extract_iterator_unsigned_long_int();
    void call_message_append_extract_iterator_array_int();
    void call_message_append_extract_iterator_array_string();
    void call_message_append_extract_iterator_array_array_string();
    
    void call_message_iterator_insertion_extraction_operator_bool();
    void call_message_iterator_insertion_extraction_operator_byte();
    void call_message_iterator_insertion_extraction_operator_int16();
    void call_message_iterator_insertion_extraction_operator_uint16();
    void call_message_iterator_insertion_extraction_operator_int32();
    void call_message_iterator_insertion_extraction_operator_uint32();
    void call_message_iterator_insertion_extraction_operator_int64();
    void call_message_iterator_insertion_extraction_operator_uint64();
    void call_message_iterator_insertion_extraction_operator_double();
    void call_message_iterator_insertion_extraction_operator_cstring();
    void call_message_iterator_insertion_extraction_operator_string();
    void call_message_iterator_insertion_extraction_operator_signature();
    void call_message_iterator_insertion_extraction_operator_char();
    void call_message_iterator_insertion_extraction_operator_int8();
    void call_message_iterator_insertion_extraction_operator_float();
    void call_message_iterator_insertion_extraction_operator_long_int();
    void call_message_iterator_insertion_extraction_operator_unsigned_long_int();
    void call_message_iterator_insertion_extraction_operator_array_int();
    void call_message_iterator_insertion_extraction_operator_array_string();

    void call_message_append_extract_iterator_multiple();
    void call_message_iterator_insertion_extraction_operator_multiple();
};

#endif

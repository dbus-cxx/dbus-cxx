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
#ifndef CALLMESSAGETESTS_H
#define CALLMESSAGETESTS_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include <dbus-cxx.h>

class CallMessageTests : public  CppUnit::TestFixture {
  public:

    CPPUNIT_TEST_SUITE( CallMessageTests );

    CPPUNIT_TEST( call_message_insertion_extraction_operator_bool );
    CPPUNIT_TEST( call_message_insertion_extraction_operator_byte );
    CPPUNIT_TEST( call_message_insertion_extraction_operator_int16 );
    CPPUNIT_TEST( call_message_insertion_extraction_operator_uint16 );
    CPPUNIT_TEST( call_message_insertion_extraction_operator_int32 );
    CPPUNIT_TEST( call_message_insertion_extraction_operator_uint32 );
    CPPUNIT_TEST( call_message_insertion_extraction_operator_int64 );
    CPPUNIT_TEST( call_message_insertion_extraction_operator_uint64 );
    CPPUNIT_TEST( call_message_insertion_extraction_operator_double );
    CPPUNIT_TEST( call_message_insertion_extraction_operator_cstring );
    CPPUNIT_TEST( call_message_insertion_extraction_operator_string );
    CPPUNIT_TEST( call_message_insertion_extraction_operator_array_double );
    CPPUNIT_TEST( call_message_insertion_extraction_operator_multiple );

    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp();

    void tearDown();

    void call_message_insertion_extraction_operator_bool();
    void call_message_insertion_extraction_operator_byte();
    void call_message_insertion_extraction_operator_int16();
    void call_message_insertion_extraction_operator_uint16();
    void call_message_insertion_extraction_operator_int32();
    void call_message_insertion_extraction_operator_uint32();
    void call_message_insertion_extraction_operator_int64();
    void call_message_insertion_extraction_operator_uint64();
    void call_message_insertion_extraction_operator_double();
    void call_message_insertion_extraction_operator_cstring();
    void call_message_insertion_extraction_operator_string();
    void call_message_insertion_extraction_operator_array_double();
    void call_message_insertion_extraction_operator_multiple();

};

#endif

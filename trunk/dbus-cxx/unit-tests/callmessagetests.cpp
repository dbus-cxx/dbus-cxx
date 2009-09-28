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
#include "callmessagetests.h"

#include <cstring>

void CallMessageTests::setUp()
{ }

void CallMessageTests::tearDown()
{ }

template <typename T>
void test_numeric_call_message_insertion_extraction_operator( T v )
{
  T v2 = 0;

  DBus::CallMessage::pointer msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );

  msg << v;

  msg >> v2;

  CPPUNIT_ASSERT_EQUAL( v, v2 );
}

void CallMessageTests::call_message_insertion_extraction_operator_bool()
{
  bool v = true;
  test_numeric_call_message_insertion_extraction_operator(v);
}

void CallMessageTests::call_message_insertion_extraction_operator_byte()
{
  uint8_t v = 67;
  test_numeric_call_message_insertion_extraction_operator(v);
}

void CallMessageTests::call_message_insertion_extraction_operator_int16()
{
  int16_t v = -16782;
  test_numeric_call_message_insertion_extraction_operator(v);
}

void CallMessageTests::call_message_insertion_extraction_operator_uint16()
{
  uint16_t v = 16794;
  test_numeric_call_message_insertion_extraction_operator(v);
}

void CallMessageTests::call_message_insertion_extraction_operator_int32()
{
  int32_t v = -47892;
  test_numeric_call_message_insertion_extraction_operator(v);
}

void CallMessageTests::call_message_insertion_extraction_operator_uint32()
{
  uint32_t v = 53938;
  test_numeric_call_message_insertion_extraction_operator(v);
}

void CallMessageTests::call_message_insertion_extraction_operator_int64()
{
  int64_t v = -378983;
  test_numeric_call_message_insertion_extraction_operator(v);
}

void CallMessageTests::call_message_insertion_extraction_operator_uint64()
{
  uint64_t v = 4924953;
  test_numeric_call_message_insertion_extraction_operator(v);
}

void CallMessageTests::call_message_insertion_extraction_operator_double()
{
  double v = 3.141592654;
  test_numeric_call_message_insertion_extraction_operator(v);
}

void CallMessageTests::call_message_insertion_extraction_operator_cstring()
{
  const char* v = "Hello World";
  const char* v2;

  DBus::CallMessage::pointer msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );

  msg << v;

  msg >> v2;

  CPPUNIT_ASSERT_EQUAL( 0, strcmp(v, v2) );
}

void CallMessageTests::call_message_insertion_extraction_operator_string()
{
  std::string v("Hello World");
  std::string v2;

  DBus::CallMessage::pointer msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );

  msg << v;

  msg >> v2;

  CPPUNIT_ASSERT_EQUAL( v, v2 );
}

void CallMessageTests::call_message_insertion_extraction_operator_array_double()
{
  std::vector<double> v, v2;
  
  for ( int i = 0; i < 35; i++ )
    v.push_back( (double)rand()/(double)rand() );

  DBus::CallMessage::pointer msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
  
  msg << v;
  
  msg >> v2;
  
  CPPUNIT_ASSERT_EQUAL( v.size(), v2.size() );
  
  for ( int i = 0; i < 35; i++ )
    CPPUNIT_ASSERT_EQUAL( v[i], v2[i] );
}

void CallMessageTests::call_message_insertion_extraction_operator_multiple( )
{
  bool        b_1    = false       , b_2    = true;
  uint8_t     ui8_1  = 67          , ui8_2  = 0;
  int16_t     i16_1  = -16782      , i16_2  = 0;
  uint16_t    ui16_1 = 16794       , ui16_2 = 0;
  int32_t     i32_1  = -47892      , i32_2  = 0;
  uint32_t    ui32_1 = 53938       , ui32_2 = 0;
  int64_t     i64_1  = -378983     , i64_2  = 0;
  uint64_t    ui64_1 = 4924953     , ui64_2 = 0;
  double      d_1    = 3.141592654 , d_2    = 0.00;
  const char  *cs_1 = "Hello World", *cs_2 = NULL;
  std::string s_1("Hello World")   , s_2("");
  std::vector<double> ad_1         , ad_2;
  
  for ( int i = 0; i < 35; i++ )
    ad_1.push_back( (double)rand()/(double)(rand()) );

  DBus::CallMessage::pointer msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
  msg << b_1 << ui8_1 << i16_1 << ui16_1 << i32_1 << ui32_1 << i64_1 << ui64_1 << d_1 << cs_1 << s_1 << ad_1;

  msg >> b_2 >> ui8_2 >> i16_2 >> ui16_2 >> i32_2 >> ui32_2 >> i64_2 >> ui64_2 >> d_2 >> cs_2 >> s_2 >> ad_2;

  CPPUNIT_ASSERT_EQUAL( b_1, b_2 );
  CPPUNIT_ASSERT_EQUAL( ui8_1, ui8_2 );
  CPPUNIT_ASSERT_EQUAL( i16_1, i16_2 );
  CPPUNIT_ASSERT_EQUAL( ui16_1, ui16_2 );
  CPPUNIT_ASSERT_EQUAL( i32_1, i32_2 );
  CPPUNIT_ASSERT_EQUAL( ui32_1, ui32_2 );
  CPPUNIT_ASSERT_EQUAL( i64_1, i64_2 );
  CPPUNIT_ASSERT_EQUAL( ui64_1, ui64_2 );
  CPPUNIT_ASSERT_EQUAL( d_1, d_2 );
  
  CPPUNIT_ASSERT_EQUAL( ad_1.size(), ad_2.size() );
  for ( int i=0; i < 35; i++ )
    CPPUNIT_ASSERT_EQUAL( ad_1[i], ad_2[i] );

}



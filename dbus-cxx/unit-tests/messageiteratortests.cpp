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
#include "messageiteratortests.h"

#include <cstring>

void MessageIteratorTests::setUp()
{ }

void MessageIteratorTests::tearDown()
{ }

template <typename T>
void test_numeric_call_message_append_extract_iterator( T v )
{
  T v2 = 0;

  DBus::CallMessage::pointer msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
  DBus::MessageAppendIterator iter1(msg);
  iter1.append( v );

  DBus::MessageIterator iter2(msg);
  v2 = (T)iter2;

  CPPUNIT_ASSERT_EQUAL( v, v2 );
}

void MessageIteratorTests::call_message_append_extract_iterator_bool()
{
  bool v = true;
  test_numeric_call_message_append_extract_iterator(v);
}

void MessageIteratorTests::call_message_append_extract_iterator_byte()
{
  uint8_t v = 67;
  test_numeric_call_message_append_extract_iterator(v);
}

void MessageIteratorTests::call_message_append_extract_iterator_int16()
{
  int16_t v = -16782;
  test_numeric_call_message_append_extract_iterator(v);
}

void MessageIteratorTests::call_message_append_extract_iterator_uint16()
{
  uint16_t v = 16794;
  test_numeric_call_message_append_extract_iterator(v);
}

void MessageIteratorTests::call_message_append_extract_iterator_int32()
{
  int32_t v = -47892;
  test_numeric_call_message_append_extract_iterator(v);
}

void MessageIteratorTests::call_message_append_extract_iterator_uint32()
{
  uint32_t v = 53938;
  test_numeric_call_message_append_extract_iterator(v);
}

void MessageIteratorTests::call_message_append_extract_iterator_int64()
{
  int64_t v = -378983;
  test_numeric_call_message_append_extract_iterator(v);
}

void MessageIteratorTests::call_message_append_extract_iterator_uint64()
{
  uint64_t v = 4924953;
  test_numeric_call_message_append_extract_iterator(v);
}

void MessageIteratorTests::call_message_append_extract_iterator_double()
{
  double v = 3.141592654;
  test_numeric_call_message_append_extract_iterator(v);
}

void MessageIteratorTests::call_message_append_extract_iterator_cstring()
{
  const char* v = "Hello World";
  const char* v2 = NULL;

  DBus::CallMessage::pointer msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
  DBus::MessageAppendIterator iter1(msg);
  iter1.append( v );

  DBus::MessageIterator iter2(msg);
  v2 = (const char*)iter2;

  CPPUNIT_ASSERT_EQUAL( 0, strcmp(v, v2) );
}

void MessageIteratorTests::call_message_append_extract_iterator_string()
{
  std::string v("Hello World");
  std::string v2;

  DBus::CallMessage::pointer msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
  DBus::MessageAppendIterator iter1(msg);
  iter1.append( v );

  DBus::MessageIterator iter2(msg);
  v2 = (const char*)iter2;

  CPPUNIT_ASSERT_EQUAL( v, v2 );
}

void MessageIteratorTests::call_message_append_extract_iterator_signature()
{
  DBus::Signature v("a{iq}");
  DBus::Signature v2;

  DBus::CallMessage::pointer msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
  DBus::MessageAppendIterator iter1(msg);
  iter1.append( v );

  DBus::MessageIterator iter2(msg);
  v2 = (DBus::Signature)iter2;

  CPPUNIT_ASSERT_EQUAL( v, v2 );
}

void MessageIteratorTests::call_message_append_extract_iterator_char()
{
  char v = 'a';
  test_numeric_call_message_append_extract_iterator(v);
}

void MessageIteratorTests::call_message_append_extract_iterator_int8()
{
  int8_t v = -120;
  test_numeric_call_message_append_extract_iterator(v);
}

void MessageIteratorTests::call_message_append_extract_iterator_float()
{
  float v = -2.8983;
  test_numeric_call_message_append_extract_iterator(v);
}

void MessageIteratorTests::call_message_append_extract_iterator_long_int()
{
  long int v = -48321;
  test_numeric_call_message_append_extract_iterator(v);
}

void MessageIteratorTests::call_message_append_extract_iterator_unsigned_long_int()
{
  unsigned long int v = 95382;
  test_numeric_call_message_append_extract_iterator(v);
}

void MessageIteratorTests::call_message_append_extract_iterator_array_int( )
{
  std::vector<int> v, v2;
  
  for ( int i = 0; i < 35; i++ )
    v.push_back( rand() );

  DBus::CallMessage::pointer msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
  DBus::MessageAppendIterator iter1(msg);
  iter1.append( v );

  DBus::MessageIterator iter2(msg);
  iter2 >> v2;

  CPPUNIT_ASSERT_EQUAL( v.size(), v2.size() );
  
  for ( int i = 0; i < 35; i++ )
    CPPUNIT_ASSERT_EQUAL( v[i], v2[i] );
}

void MessageIteratorTests::call_message_append_extract_iterator_array_string( )
{
  std::vector<std::string> v, v2;
  char string[ 30 ];
  
  for( int i = 0; i < 35; i++ ){
    snprintf( string, 30, "string%d", i );
    v.push_back( string );
  }

  DBus::CallMessage::pointer msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
  DBus::MessageAppendIterator iter1(msg);
  iter1.append( v );

  DBus::MessageIterator iter2(msg);
  iter2 >> v2;

  CPPUNIT_ASSERT_EQUAL( v.size(), v2.size() );
  
  for ( int i = 0; i < 35; i++ )
    CPPUNIT_ASSERT_EQUAL( v[i], v2[i] );
}

void MessageIteratorTests::call_message_append_extract_iterator_array_array_string( )
{
  std::vector<std::vector<std::string>> sv, sv2;
  std::vector<std::string> v, v2;
  char string[ 30 ];
  
  for( int y = 0; y < 6; y++ ){
    v.clear();
    for( int i = 0; i < 35; i++ ){
      snprintf( string, 30, "string%d-%d", i, y );
      v.push_back( string );
    }
    sv.push_back( v );
  }

  DBus::CallMessage::pointer msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
  DBus::MessageAppendIterator iter1(msg);
  iter1.append( sv );

  DBus::MessageIterator iter2(msg);
  iter2 >> sv2;

  CPPUNIT_ASSERT_EQUAL( sv.size(), sv2.size() );

  for( int y = 0; y < 6; y++ ){
    for( int i = 0; i < 35; i++ ){
      CPPUNIT_ASSERT_EQUAL( (sv[ y ])[i], (sv2[y])[i] );
    }
  }
  
}

template <typename T>
    void test_numeric_call_message_iterator_insertion_extraction_operator( T v )
{
  T v2 = 0;

  DBus::CallMessage::pointer msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
  DBus::MessageAppendIterator iter1(msg);
  iter1 << v;

  DBus::MessageIterator iter2(msg);
  iter2 >> v2;

  CPPUNIT_ASSERT_EQUAL( v, v2 );
}

void MessageIteratorTests::call_message_iterator_insertion_extraction_operator_bool()
{
  bool v = true;
  test_numeric_call_message_iterator_insertion_extraction_operator(v);
}

void MessageIteratorTests::call_message_iterator_insertion_extraction_operator_byte()
{
  uint8_t v = 67;
  test_numeric_call_message_iterator_insertion_extraction_operator(v);
}

void MessageIteratorTests::call_message_iterator_insertion_extraction_operator_int16()
{
  int16_t v = -16782;
  test_numeric_call_message_iterator_insertion_extraction_operator(v);
}

void MessageIteratorTests::call_message_iterator_insertion_extraction_operator_uint16()
{
  uint16_t v = 16794;
  test_numeric_call_message_iterator_insertion_extraction_operator(v);
}

void MessageIteratorTests::call_message_iterator_insertion_extraction_operator_int32()
{
  int32_t v = -47892;
  test_numeric_call_message_iterator_insertion_extraction_operator(v);
}

void MessageIteratorTests::call_message_iterator_insertion_extraction_operator_uint32()
{
  uint32_t v = 53938;
  test_numeric_call_message_iterator_insertion_extraction_operator(v);
}

void MessageIteratorTests::call_message_iterator_insertion_extraction_operator_int64()
{
  int64_t v = -378983;
  test_numeric_call_message_iterator_insertion_extraction_operator(v);
}

void MessageIteratorTests::call_message_iterator_insertion_extraction_operator_uint64()
{
  uint64_t v = 4924953;
  test_numeric_call_message_iterator_insertion_extraction_operator(v);
}

void MessageIteratorTests::call_message_iterator_insertion_extraction_operator_double()
{
  double v = 3.141592654;
  test_numeric_call_message_iterator_insertion_extraction_operator(v);
}

void MessageIteratorTests::call_message_iterator_insertion_extraction_operator_cstring()
{
  const char* v = "Hello World";
  const char* v2;

  DBus::CallMessage::pointer msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
  DBus::MessageAppendIterator iter1(msg);
  iter1 << v;

  DBus::MessageIterator iter2(msg);
  iter2 >> v2;

  CPPUNIT_ASSERT_EQUAL( 0, strcmp(v, v2) );
}

void MessageIteratorTests::call_message_iterator_insertion_extraction_operator_string()
{
  std::string v("Hello World");
  std::string v2;

  DBus::CallMessage::pointer msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
  DBus::MessageAppendIterator iter1(msg);
  iter1 << v;

  DBus::MessageIterator iter2(msg);
  iter2 >> v2;

  CPPUNIT_ASSERT_EQUAL( v, v2 );
}

void MessageIteratorTests::call_message_iterator_insertion_extraction_operator_signature()
{
  DBus::Signature v("a{iq}");
  DBus::Signature v2;

  DBus::CallMessage::pointer msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
  DBus::MessageAppendIterator iter1(msg);
  iter1 << v;

  DBus::MessageIterator iter2(msg);
  iter2 >> v2;

  CPPUNIT_ASSERT_EQUAL( v, v2 );
}

void MessageIteratorTests::call_message_iterator_insertion_extraction_operator_char()
{
  char v = 'a';
  test_numeric_call_message_iterator_insertion_extraction_operator(v);
}

void MessageIteratorTests::call_message_iterator_insertion_extraction_operator_int8()
{
  int8_t v = -112;
  test_numeric_call_message_iterator_insertion_extraction_operator(v);
}

void MessageIteratorTests::call_message_iterator_insertion_extraction_operator_float()
{
  float v = -2984.3933;
  test_numeric_call_message_iterator_insertion_extraction_operator(v);
}

void MessageIteratorTests::call_message_iterator_insertion_extraction_operator_long_int()
{
  long int v = -67445;
  test_numeric_call_message_iterator_insertion_extraction_operator(v);
}

void MessageIteratorTests::call_message_iterator_insertion_extraction_operator_unsigned_long_int()
{
  unsigned long int v = 343673;
  test_numeric_call_message_iterator_insertion_extraction_operator(v);
}

void MessageIteratorTests::call_message_iterator_insertion_extraction_operator_array_string()
{
  std::vector<std::string> v, v2;

  v.push_back( "strign1" );
  v.push_back( "string2" );

  DBus::CallMessage::pointer msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
  DBus::MessageAppendIterator iter1(msg);
  
  iter1 << v;

  DBus::MessageIterator iter2(msg);
  iter2 >> v2;

  CPPUNIT_ASSERT_EQUAL( v.size(), v2.size() );

  for( int i = 0; i < 2; i++ )
    CPPUNIT_ASSERT_EQUAL( v[i], v2[i] );
}

void MessageIteratorTests::call_message_iterator_insertion_extraction_operator_array_int( )
{
  std::vector<int> v, v2;
  
  for ( int i = 0; i < 35; i++ )
    v.push_back( rand() );

  DBus::CallMessage::pointer msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
  DBus::MessageAppendIterator iter1(msg);
  iter1 << v;

  DBus::MessageIterator iter2(msg);
  iter2 >> v2;

  CPPUNIT_ASSERT_EQUAL( v.size(), v2.size() );
  
  for ( int i = 0; i < 35; i++ )
    CPPUNIT_ASSERT_EQUAL( v[i], v2[i] );
}

void MessageIteratorTests::call_message_append_extract_iterator_multiple( )
{
  bool            b_1    = false       , b_2    = true;
  uint8_t         ui8_1  = 67          , ui8_2  = 0;
  int16_t         i16_1  = -16782      , i16_2  = 0;
  uint16_t        ui16_1 = 16794       , ui16_2 = 0;
  int32_t         i32_1  = -47892      , i32_2  = 0;
  uint32_t        ui32_1 = 53938       , ui32_2 = 0;
  int64_t         i64_1  = -378983     , i64_2  = 0;
  uint64_t        ui64_1 = 4924953     , ui64_2 = 0;
  double          d_1    = 3.141592654 , d_2    = 0.00;
  const char      *cs_1 = "Hello World", *cs_2 = NULL;
  std::string     s_1("Hello World")   , s_2("");
  DBus::Signature sig_1("a{iq}") , sig_2;
  char            c_1    = 'a'         , c_2    = '\0';
  int8_t          i8_1   = 119         , i8_2   = 0;
  float           f_1    = -222.43212  , f_2    = 0.00;
  long int        li_1   = -344223     , li_2   = 0;
  unsigned long int uli_1= 94321       , uli_2  = 0;
  std::vector<double> ad_1             , ad_2;
  
  for ( int i = 0; i < 35; i++ )
    ad_1.push_back( (double)rand()/(double)(rand()) );


  DBus::CallMessage::pointer msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
  DBus::MessageAppendIterator iter1(msg);
  iter1.append( b_1 );
  iter1.append( ui8_1 );
  iter1.append( i16_1 );
  iter1.append( ui16_1 );
  iter1.append( i32_1 );
  iter1.append( ui32_1 );
  iter1.append( i64_1 );
  iter1.append( ui64_1 );
  iter1.append( d_1 );
  iter1.append( cs_1 );
  iter1.append( s_1 );
  iter1.append( sig_1 );
  iter1.append( c_1 );
  iter1.append( i8_1 );
  iter1.append( f_1 );
  iter1.append( li_1 );
  iter1.append( uli_1 );
  iter1.append( ad_1 );

  DBus::MessageIterator iter2(msg);
  b_2    = (bool)iter2;              iter2.next();
  ui8_2  = (uint8_t)iter2;           iter2.next();
  i16_2  = (int16_t)iter2;           iter2.next();
  ui16_2 = (uint16_t)iter2;          iter2.next();
  i32_2  = (int32_t)iter2;           iter2.next();
  ui32_2 = (uint32_t)iter2;          iter2.next();
  i64_2  = (int64_t)iter2;           iter2.next();
  ui64_2 = (uint64_t)iter2;          iter2.next();
  d_2    = (double)iter2;            iter2.next();
  cs_2   = (const char*)iter2;       iter2.next();
  s_2    = (std::string)iter2;       iter2.next();
  sig_2  = (DBus::Signature)iter2;   iter2.next();
  c_2    = (char)iter2;              iter2.next();
  i8_2   = (int8_t)iter2;            iter2.next();
  f_2    = (float)iter2;             iter2.next();
  li_2   = (long int)iter2;          iter2.next();
  uli_2  = (unsigned long int)iter2; iter2.next();
  iter2 >> ad_2;
  /*iter2.get_array(ad_2);*/             iter2.next();

  CPPUNIT_ASSERT_EQUAL( b_1, b_2 );
  CPPUNIT_ASSERT_EQUAL( ui8_1, ui8_2 );
  CPPUNIT_ASSERT_EQUAL( i16_1, i16_2 );
  CPPUNIT_ASSERT_EQUAL( ui16_1, ui16_2 );
  CPPUNIT_ASSERT_EQUAL( i32_1, i32_2 );
  CPPUNIT_ASSERT_EQUAL( ui32_1, ui32_2 );
  CPPUNIT_ASSERT_EQUAL( i64_1, i64_2 );
  CPPUNIT_ASSERT_EQUAL( ui64_1, ui64_2 );
  CPPUNIT_ASSERT_EQUAL( d_1, d_2 );
  CPPUNIT_ASSERT_EQUAL( 0, strcmp( cs_1, cs_2 ) );
  CPPUNIT_ASSERT_EQUAL( s_1, s_2 );
  CPPUNIT_ASSERT_EQUAL( sig_1, sig_2 );
  CPPUNIT_ASSERT_EQUAL( c_1, c_2 );
  CPPUNIT_ASSERT_EQUAL( i8_1, i8_2 );
  CPPUNIT_ASSERT_EQUAL( f_1, f_2 );
  CPPUNIT_ASSERT_EQUAL( li_1, li_2 );
  CPPUNIT_ASSERT_EQUAL( uli_1, uli_2 );
  
  CPPUNIT_ASSERT_EQUAL( ad_1.size(), ad_2.size() );
  for ( int i=0; i < 35; i++ )
    CPPUNIT_ASSERT_EQUAL( ad_1[i], ad_2[i] );
}

void MessageIteratorTests::call_message_iterator_insertion_extraction_operator_multiple( )
{
  bool            b_1    = false       , b_2    = true;
  uint8_t         ui8_1  = 67          , ui8_2  = 0;
  int16_t         i16_1  = -16782      , i16_2  = 0;
  uint16_t        ui16_1 = 16794       , ui16_2 = 0;
  int32_t         i32_1  = -47892      , i32_2  = 0;
  uint32_t        ui32_1 = 53938       , ui32_2 = 0;
  int64_t         i64_1  = -378983     , i64_2  = 0;
  uint64_t        ui64_1 = 4924953     , ui64_2 = 0;
  double          d_1    = 3.141592654 , d_2    = 0.00;
  const char      *cs_1 = "Hello World", *cs_2 = NULL;
  std::string     s_1("Hello World")   , s_2("");
  DBus::Signature sig_1("a{iq}") , sig_2;
  char            c_1    = 'a'         , c_2    = '\0';
  int8_t          i8_1   = 119         , i8_2   = 0;
  float           f_1    = -222.43212  , f_2    = 0.00;
  long int        li_1   = -344223     , li_2   = 0;
  unsigned long int uli_1= 94321       , uli_2  = 0;
  std::vector<double> ad_1             , ad_2;

  for ( int i = 0; i < 35; i++ )
    ad_1.push_back( (double)rand()/(double)(rand()) );

  DBus::CallMessage::pointer msg = DBus::CallMessage::create( "/org/freedesktop/DBus", "method" );
  DBus::MessageAppendIterator iter1(msg);
  iter1 << b_1 << ui8_1 << i16_1 << ui16_1 << i32_1 << ui32_1 << i64_1 << ui64_1 << d_1 << cs_1 << s_1 << sig_1 << c_1 << i8_1 << f_1 << li_1 << uli_1 << ad_1;

  DBus::MessageIterator iter2(msg);
  iter2 >> b_2 >> ui8_2 >> i16_2 >> ui16_2 >> i32_2 >> ui32_2 >> i64_2 >> ui64_2 >> d_2 >> cs_2 >> s_2 >> sig_2 >> c_2 >> i8_2 >> f_2 >> li_2 >> uli_2 >> ad_2;

  CPPUNIT_ASSERT_EQUAL( b_1, b_2 );
  CPPUNIT_ASSERT_EQUAL( ui8_1, ui8_2 );
  CPPUNIT_ASSERT_EQUAL( i16_1, i16_2 );
  CPPUNIT_ASSERT_EQUAL( ui16_1, ui16_2 );
  CPPUNIT_ASSERT_EQUAL( i32_1, i32_2 );
  CPPUNIT_ASSERT_EQUAL( ui32_1, ui32_2 );
  CPPUNIT_ASSERT_EQUAL( i64_1, i64_2 );
  CPPUNIT_ASSERT_EQUAL( ui64_1, ui64_2 );
  CPPUNIT_ASSERT_EQUAL( d_1, d_2 );
  CPPUNIT_ASSERT_EQUAL( 0, strcmp( cs_1, cs_2 ) );
  CPPUNIT_ASSERT_EQUAL( s_1, s_2 );
  CPPUNIT_ASSERT_EQUAL( sig_1, sig_2 );
  CPPUNIT_ASSERT_EQUAL( c_1, c_2 );
  CPPUNIT_ASSERT_EQUAL( i8_1, i8_2 );
  CPPUNIT_ASSERT_EQUAL( f_1, f_2 );
  CPPUNIT_ASSERT_EQUAL( li_1, li_2 );
  CPPUNIT_ASSERT_EQUAL( uli_1, uli_2 );
  
  CPPUNIT_ASSERT_EQUAL( ad_1.size(), ad_2.size() );
  for ( int i=0; i < 35; i++ )
    CPPUNIT_ASSERT_EQUAL( ad_1[i], ad_2[i] );
}

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
#include "pathclasstests.h"

#include <cstring>

void PathClassTests::setUp()
{ }

void PathClassTests::tearDown()
{ }

void PathClassTests::path_test_valid()
{
  DBus::Path path = "/this/1/is/a/valid/path";
  CPPUNIT_ASSERT(path.is_valid() == true );
}

void PathClassTests::path_test_invalid_characters()
{
  DBus::Path path = "/this/is/an invalid/path";
  CPPUNIT_ASSERT(path.is_valid() == false );
}

void PathClassTests::path_test_invalid_trailing_slash()
{
  DBus::Path path = "/this/1/is/an/invalid/path/";
  CPPUNIT_ASSERT(path.is_valid() == false );
}

void PathClassTests::path_test_invalid_empty_path()
{
  DBus::Path path = "";
  CPPUNIT_ASSERT(path.is_valid() == false );
}

void PathClassTests::path_test_invalid_double_slash()
{
  DBus::Path path = "/this/1/is/an//invalid/path/";
  CPPUNIT_ASSERT(path.is_valid() == false );
}

void PathClassTests::path_test_invalid_missing_leading_slash()
{
  DBus::Path path = "this/1/is/an/invalid/path/";
  CPPUNIT_ASSERT(path.is_valid() == false );
}

void PathClassTests::path_decompose_valid()
{
  DBus::Path path = "/this/1/is/a/valid/path";
  DBus::Path::Decomposed decomposed = path.decomposed();
  CPPUNIT_ASSERT( decomposed.size() == 6 );
  CPPUNIT_ASSERT_EQUAL( std::string("this"),  decomposed[0] );
  CPPUNIT_ASSERT_EQUAL( std::string("1"),     decomposed[1] );
  CPPUNIT_ASSERT_EQUAL( std::string("is"),    decomposed[2] );
  CPPUNIT_ASSERT_EQUAL( std::string("a"),     decomposed[3] );
  CPPUNIT_ASSERT_EQUAL( std::string("valid"), decomposed[4] );
  CPPUNIT_ASSERT_EQUAL( std::string("path"),  decomposed[5] );
}

void PathClassTests::path_decompose_root()
{
  DBus::Path path = "/";
  DBus::Path::Decomposed decomposed = path.decomposed();
  CPPUNIT_ASSERT( decomposed.size() == 1 );
  CPPUNIT_ASSERT( decomposed[0].empty() );
}

void PathClassTests::path_decompose_invalid()
{
  DBus::Path path = "this/1/is/an/invalid/path/";
  DBus::Path::Decomposed decomposed = path.decomposed();
  CPPUNIT_ASSERT( decomposed.empty() );
}

void PathClassTests::path_decompose_empty()
{
  DBus::Path path = "";
  DBus::Path::Decomposed decomposed = path.decomposed();
  CPPUNIT_ASSERT( decomposed.empty() );
}

void PathClassTests::path_append_valid()
{
  DBus::Path path = "/this/1/is/a/valid/path";
  path.append_element("too");
  CPPUNIT_ASSERT_EQUAL(DBus::Path("/this/1/is/a/valid/path/too"), path);
}

void PathClassTests::path_append_valid_multiple()
{
  DBus::Path path = "/this/1/is/a/valid/path";
  path.append_element("as/well");
  CPPUNIT_ASSERT_EQUAL(DBus::Path("/this/1/is/a/valid/path/as/well"), path);
}

void PathClassTests::path_append_valid_leading_slash()
{
  DBus::Path path = "/this/1/is/a/valid/path";
  path.append_element("/too");
  CPPUNIT_ASSERT_EQUAL(DBus::Path("/this/1/is/a/valid/path/too"), path);
}

void PathClassTests::path_append_valid_trailing_slash()
{
  DBus::Path path = "/this/1/is/a/valid/path";
  path.append_element("too/");
  CPPUNIT_ASSERT_EQUAL(DBus::Path("/this/1/is/a/valid/path/too"), path);
}

void PathClassTests::path_append_valid_leading_trailing_slash()
{
  DBus::Path path = "/this/1/is/a/valid/path";
  path.append_element("/too/");
  CPPUNIT_ASSERT_EQUAL(DBus::Path("/this/1/is/a/valid/path/too"), path);
}

void PathClassTests::path_append_invalid_empty()
{
  DBus::Path path = "/this/1/is/a/valid/path";
  CPPUNIT_ASSERT( path.append_element("") == false );
  CPPUNIT_ASSERT_EQUAL(DBus::Path("/this/1/is/a/valid/path"), path);
}

void PathClassTests::path_append_invalid_chars()
{
  DBus::Path path = "/this/1/is/a/valid/path";
  CPPUNIT_ASSERT( path.append_element("as well") == false );
  CPPUNIT_ASSERT_EQUAL(DBus::Path("/this/1/is/a/valid/path"), path);
}

void PathClassTests::path_append_invalid_double_slash()
{
  DBus::Path path = "/this/1/is/a/valid/path";
  CPPUNIT_ASSERT( path.append_element("as//well") == false );
  CPPUNIT_ASSERT_EQUAL(DBus::Path("/this/1/is/a/valid/path"), path);
}

void PathClassTests::path_append_invalid_root()
{
  DBus::Path path = "/this/1/is/a/valid/path";
  CPPUNIT_ASSERT( path.append_element("/") == false );
  CPPUNIT_ASSERT_EQUAL(DBus::Path("/this/1/is/a/valid/path"), path);
}



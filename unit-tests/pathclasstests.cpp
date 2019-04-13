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
#include <dbus-cxx.h>

#include "test_macros.h"

bool path_test_valid()
{
  DBus::Path path = "/this/1/is/a/valid/path";
  TEST_ASSERT_RET_FAIL(path.is_valid() == true );
  return true;
}

bool path_test_invalid_characters()
{
  DBus::Path path = "/this/is/an invalid/path";
  TEST_ASSERT_RET_FAIL(path.is_valid() == false );
  return true;
}

bool path_test_invalid_trailing_slash()
{
  DBus::Path path = "/this/1/is/an/invalid/path/";
  TEST_ASSERT_RET_FAIL(path.is_valid() == false );
  return true;
}

bool path_test_invalid_empty_path()
{
  DBus::Path path = "";
  TEST_ASSERT_RET_FAIL(path.is_valid() == false );
  return true;
}

bool path_test_invalid_double_slash()
{
  DBus::Path path = "/this/1/is/an//invalid/path/";
  TEST_ASSERT_RET_FAIL(path.is_valid() == false );
  return true;
}

bool path_test_invalid_missing_leading_slash()
{
  DBus::Path path = "this/1/is/an/invalid/path/";
  TEST_ASSERT_RET_FAIL(path.is_valid() == false );
  return true;
}

bool path_decompose_valid()
{
  DBus::Path path = "/this/1/is/a/valid/path";
  DBus::Path::Decomposed decomposed = path.decomposed();
  TEST_ASSERT_RET_FAIL( decomposed.size() == 6 );
  TEST_EQUALS_RET_FAIL( std::string("this"),  decomposed[0] );
  TEST_EQUALS_RET_FAIL( std::string("1"),     decomposed[1] );
  TEST_EQUALS_RET_FAIL( std::string("is"),    decomposed[2] );
  TEST_EQUALS_RET_FAIL( std::string("a"),     decomposed[3] );
  TEST_EQUALS_RET_FAIL( std::string("valid"), decomposed[4] );
  TEST_EQUALS_RET_FAIL( std::string("path"),  decomposed[5] );

  return true;
}

bool path_decompose_root()
{
  DBus::Path path = "/";
  DBus::Path::Decomposed decomposed = path.decomposed();
  TEST_EQUALS_RET_FAIL( decomposed.size(), 1 );
  TEST_EQUALS_RET_FAIL( decomposed[0].empty(), true );

  return true;
}

bool path_decompose_invalid()
{
  DBus::Path path = "this/1/is/an/invalid/path/";
  DBus::Path::Decomposed decomposed = path.decomposed();
  TEST_ASSERT_RET_FAIL( decomposed.empty() );
  return true;
}

bool path_decompose_empty()
{
  DBus::Path path = "";
  DBus::Path::Decomposed decomposed = path.decomposed();
  TEST_ASSERT_RET_FAIL( decomposed.empty() );
  return true;
}

bool path_append_valid()
{
  DBus::Path path = "/this/1/is/a/valid/path";
  path.append_element("too");
  return TEST_EQUALS(DBus::Path("/this/1/is/a/valid/path/too"), path);
}

bool path_append_valid_multiple()
{
  DBus::Path path = "/this/1/is/a/valid/path";
  path.append_element("as/well");
  return TEST_EQUALS(DBus::Path("/this/1/is/a/valid/path/as/well"), path);
}

bool path_append_valid_leading_slash()
{
  DBus::Path path = "/this/1/is/a/valid/path";
  path.append_element("/too");
  return TEST_EQUALS(DBus::Path("/this/1/is/a/valid/path/too"), path);
}

bool path_append_valid_trailing_slash()
{
  DBus::Path path = "/this/1/is/a/valid/path";
  path.append_element("too/");
  return TEST_EQUALS(DBus::Path("/this/1/is/a/valid/path/too"), path);
}

bool path_append_valid_leading_trailing_slash()
{
  DBus::Path path = "/this/1/is/a/valid/path";
  path.append_element("/too/");
  return TEST_EQUALS(DBus::Path("/this/1/is/a/valid/path/too"), path);
}

bool path_append_invalid_empty()
{
  DBus::Path path = "/this/1/is/a/valid/path";
  TEST_ASSERT_RET_FAIL( path.append_element("") == false );
  return TEST_EQUALS(DBus::Path("/this/1/is/a/valid/path"), path);
}

bool path_append_invalid_chars()
{
  DBus::Path path = "/this/1/is/a/valid/path";
  TEST_ASSERT_RET_FAIL( path.append_element("as well") == false );
  return TEST_EQUALS(DBus::Path("/this/1/is/a/valid/path"), path);
}

bool path_append_invalid_double_slash()
{
  DBus::Path path = "/this/1/is/a/valid/path";
  TEST_ASSERT_RET_FAIL( path.append_element("as//well") == false );
  return TEST_EQUALS(DBus::Path("/this/1/is/a/valid/path"), path);
}

bool path_append_invalid_root()
{
  DBus::Path path = "/this/1/is/a/valid/path";
  TEST_ASSERT_RET_FAIL( path.append_element("/") == false );
  return TEST_EQUALS(DBus::Path("/this/1/is/a/valid/path"), path);
}

#define ADD_TEST(name) do{ if( test_name == STRINGIFY(name) ){ \
  ret = path_##name();\
} \
} while( 0 )

int main(int argc, char** argv){
  if(argc < 1)
    return 1;

  std::string test_name = argv[1];
  bool ret = false;

  ADD_TEST(test_valid);
  ADD_TEST(test_invalid_characters);
  ADD_TEST(test_invalid_trailing_slash);
  ADD_TEST(test_invalid_empty_path);
  ADD_TEST(test_invalid_double_slash);
  ADD_TEST(test_invalid_missing_leading_slash);
  ADD_TEST(decompose_valid);
  ADD_TEST(decompose_root);
  ADD_TEST(decompose_invalid);
  ADD_TEST(decompose_empty);
  ADD_TEST(append_valid);
  ADD_TEST(append_valid_multiple);
  ADD_TEST(append_valid_leading_slash);
  ADD_TEST(append_valid_trailing_slash);
  ADD_TEST(append_invalid_empty);
  ADD_TEST(append_invalid_chars);
  ADD_TEST(append_invalid_double_slash);
  ADD_TEST(append_invalid_root);

  return !ret;
}

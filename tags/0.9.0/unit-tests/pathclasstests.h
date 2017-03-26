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
#ifndef PATHCLASSTESTS_H
#define PATHCLASSTESTS_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include <dbus-cxx.h>

class PathClassTests : public  CppUnit::TestFixture {
  public:

    CPPUNIT_TEST_SUITE( PathClassTests );

    CPPUNIT_TEST( path_test_valid );
    CPPUNIT_TEST( path_test_invalid_characters );
    CPPUNIT_TEST( path_test_invalid_trailing_slash );
    CPPUNIT_TEST( path_test_invalid_empty_path );
    CPPUNIT_TEST( path_test_invalid_double_slash );
    CPPUNIT_TEST( path_test_invalid_missing_leading_slash );
    
    CPPUNIT_TEST( path_decompose_valid );
    CPPUNIT_TEST( path_decompose_root );
    CPPUNIT_TEST( path_decompose_invalid );
    CPPUNIT_TEST( path_decompose_empty );

    CPPUNIT_TEST( path_append_valid );
    CPPUNIT_TEST( path_append_valid_multiple );
    CPPUNIT_TEST( path_append_valid_leading_slash );
    CPPUNIT_TEST( path_append_valid_trailing_slash );
    CPPUNIT_TEST( path_append_valid_leading_trailing_slash );
    CPPUNIT_TEST( path_append_invalid_empty );
    CPPUNIT_TEST( path_append_invalid_chars );
    CPPUNIT_TEST( path_append_invalid_double_slash );
    CPPUNIT_TEST( path_append_invalid_root );

    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp();

    void tearDown();

    void path_test_valid();
    void path_test_invalid_characters();
    void path_test_invalid_trailing_slash();
    void path_test_invalid_empty_path();
    void path_test_invalid_double_slash();
    void path_test_invalid_missing_leading_slash();

    void path_decompose_valid();
    void path_decompose_root();
    void path_decompose_invalid();
    void path_decompose_empty();

    void path_append_valid();
    void path_append_valid_multiple();
    void path_append_valid_leading_slash();
    void path_append_valid_trailing_slash();
    void path_append_valid_leading_trailing_slash();
    void path_append_invalid_empty();
    void path_append_invalid_chars();
    void path_append_invalid_double_slash();
    void path_append_invalid_root();
};

#endif

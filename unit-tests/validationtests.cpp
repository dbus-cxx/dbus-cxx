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
#include <dbus-cxx.h>
#include <dbus-cxx/validator.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>

#include "test_macros.h"

bool validate_one_section_bus_name() {
    return !DBus::Validator::validate_bus_name( "dbuscxx" );
}

bool validate_two_section_bus_name() {
    return DBus::Validator::validate_bus_name( "dbuscxx.test" );
}

bool validate_three_section_bus_name() {
    return DBus::Validator::validate_bus_name( "dbuscxx.test.foo" );
}

/*
 * Signature validation.  Each test checks a list of signatures, and prints
 * out any that are not validated as expected.
 */
static bool check_signatures( std::vector<std::string> signatures, bool expected ) {
    bool ok = true;

    for( const std::string& sig : signatures ) {
        if( DBus::Validator::validate_signature( sig ) != expected ) {
            std::cerr << "Signature '" << sig << "' should be "
                << ( expected ? "valid" : "invalid" ) << std::endl;
            ok = false;
        }
    }

    return ok;
}

bool validate_signature_empty() {
    return check_signatures( { "" }, true );
}

bool validate_signature_basic_types() {
    return check_signatures( { "y", "b", "n", "q", "i", "u", "x", "t", "d",
                               "s", "o", "g", "h", "ybnqiuxtdsogh" }, true );
}

bool validate_signature_variant() {
    return check_signatures( { "v", "av", "(v)", "a{sv}" }, true );
}

bool validate_signature_array() {
    return check_signatures( { "ai", "as", "aai", "aas", "a(ii)", "aa{sv}" }, true );
}

bool validate_signature_struct() {
    return check_signatures( { "(i)", "(ii)", "((i))", "(sa(us))", "(i(id)ss)" }, true );
}

bool validate_signature_dict() {
    return check_signatures( { "a{sv}", "a{yv}", "a{hv}", "a{s(ii)}",
                               "a{sa{sv}}", "a{oa{sa{sv}}}" }, true );
}

bool validate_signature_multiple_types() {
    return check_signatures( { "a{sv}s(ii)v", "a{sv}as", "ia{sv}i" }, true );
}

bool validate_signature_invalid_type_code() {
    return check_signatures( { "z", "I", "r", "e", "m", "*", "?", "@", "&", "^",
                               std::string( 1, '\0' ), "i z" }, false );
}

bool validate_signature_empty_struct() {
    return check_signatures( { "()", "a()", "(())", "(i())" }, false );
}

bool validate_signature_dict_outside_array() {
    return check_signatures( { "{sv}", "(i{sv})", "v{sv}", "a{sv}{sv}" }, false );
}

bool validate_signature_dict_member_count() {
    return check_signatures( { "a{}", "a{}sv", "a{s}", "a{sss}" }, false );
}

bool validate_signature_dict_key_not_basic() {
    return check_signatures( { "a{vs}", "a{(i)s}", "a{ais}", "a{a{sv}s}" }, false );
}

bool validate_signature_unbalanced() {
    return check_signatures( { "(", ")", "(s", "s)", "(i))", "((i)",
                               "a{sv", "a{sv}}", "}", "a)", "a" , "aa" }, false );
}

bool validate_signature_max_array_depth() {
    std::string max_depth = std::string( 32, 'a' ) + "i";
    std::string too_deep = std::string( 33, 'a' ) + "i";

    return check_signatures( { max_depth }, true ) &&
        check_signatures( { too_deep }, false );
}

bool validate_signature_max_struct_depth() {
    std::string max_depth = std::string( 32, '(' ) + "i" + std::string( 32, ')' );
    std::string too_deep = std::string( 33, '(' ) + "i" + std::string( 33, ')' );

    return check_signatures( { max_depth }, true ) &&
        check_signatures( { too_deep }, false );
}

bool validate_signature_max_length() {
    std::string max_length( 255, 'i' );
    std::string too_long( 256, 'i' );

    return check_signatures( { max_length }, true ) &&
        check_signatures( { too_long }, false );
}

#define ADD_TEST(name) do{ if( test_name == STRINGIFY(name) ){ \
            ret = validate_##name();\
        } \
    } while( 0 )

int main( int argc, char** argv ) {
    if( argc < 1 ) {
        return 1;
    }

    std::string test_name = argv[1];
    bool ret = false;

    ADD_TEST( one_section_bus_name );
    ADD_TEST( two_section_bus_name );
    ADD_TEST( three_section_bus_name );
    ADD_TEST( signature_empty );
    ADD_TEST( signature_basic_types );
    ADD_TEST( signature_variant );
    ADD_TEST( signature_array );
    ADD_TEST( signature_struct );
    ADD_TEST( signature_dict );
    ADD_TEST( signature_multiple_types );
    ADD_TEST( signature_invalid_type_code );
    ADD_TEST( signature_empty_struct );
    ADD_TEST( signature_dict_outside_array );
    ADD_TEST( signature_dict_member_count );
    ADD_TEST( signature_dict_key_not_basic );
    ADD_TEST( signature_unbalanced );
    ADD_TEST( signature_max_array_depth );
    ADD_TEST( signature_max_struct_depth );
    ADD_TEST( signature_max_length );

    return !ret;
}

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
#include <unistd.h>
#include <iostream>

#include "test_macros.h"

bool signature_create() {
    DBus::Signature sig;

    return true;
}

bool signature_iterate_int32() {
    DBus::Signature sig( "i" );

    DBus::SignatureIterator it = sig.begin();

    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::INT32 );

    // Check the output signature is the same as the input
    TEST_EQUALS_RET_FAIL(sig.str(), sig.begin().signature());

    return true;
}

bool signature_iterate_arrayint() {
    DBus::Signature sig( "ai" );

    DBus::SignatureIterator it = sig.begin();

    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::ARRAY );
    TEST_EQUALS_RET_FAIL( it.is_array(), true );
    TEST_EQUALS_RET_FAIL( it.is_container(), true );
    TEST_EQUALS_RET_FAIL( it.is_dict(), false );

    it.next();

    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::INVALID );

    // Check the output signature is the same as the input
    TEST_EQUALS_RET_FAIL(sig.str(), sig.begin().signature());

    return true;
}

bool signature_iterate_arrayint2() {
    DBus::Signature sig( "ai" );

    DBus::SignatureIterator it = sig.begin();
    DBus::SignatureIterator subit = it.recurse();

    TEST_EQUALS_RET_FAIL( subit.type(), DBus::DataType::INT32 );

    subit.next();

    TEST_EQUALS_RET_FAIL( subit.type(), DBus::DataType::INVALID );

    // Check the output signature is the same as the input
    TEST_EQUALS_RET_FAIL(sig.str(), sig.begin().signature());

    return true;
}

bool signature_iterate_nested_array() {
    DBus::Signature sig( "aai" );

    DBus::SignatureIterator it = sig.begin();
    DBus::SignatureIterator subit = it.recurse();

    TEST_EQUALS_RET_FAIL( subit.type(), DBus::DataType::ARRAY );

    DBus::SignatureIterator subit2 = subit.recurse();

    TEST_EQUALS_RET_FAIL( subit2.type(), DBus::DataType::INT32 );

    subit2.next();

    TEST_EQUALS_RET_FAIL( subit2.type(), DBus::DataType::INVALID );

    // Check the output signature is the same as the input
    TEST_EQUALS_RET_FAIL(sig.str(), sig.begin().signature());

    return true;
}

bool signature_iterate_multiple_types() {
    DBus::Signature sig( "idyb" );

    DBus::SignatureIterator it = sig.begin();

    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::INT32 );
    it.next();
    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::DOUBLE );
    it.next();
    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::BYTE );
    it.next();
    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::BOOLEAN );

    // Check the output signature is the same as the input
    TEST_EQUALS_RET_FAIL(sig.str(), sig.begin().signature());

    return true;
}

bool signature_iterate_dictionary() {
    DBus::Signature sig( "a{yv}ii" );

    DBus::SignatureIterator it = sig.begin();

    TEST_EQUALS_RET_FAIL( it.is_dict(), true );
    TEST_EQUALS_RET_FAIL(it.type(), DBus::DataType::ARRAY);

    DBus::SignatureIterator subit = it.recurse();
    TEST_EQUALS_RET_FAIL( subit.type(), DBus::DataType::DICT_ENTRY );

    DBus::SignatureIterator subsubit = subit.recurse();
    TEST_EQUALS_RET_FAIL( subsubit.type(), DBus::DataType::BYTE );

    subsubit.next();
    TEST_EQUALS_RET_FAIL( subsubit.type(), DBus::DataType::VARIANT );

    subsubit.next();
    TEST_EQUALS_RET_FAIL( subsubit.type(), DBus::DataType::INVALID );

    it.next();
    TEST_EQUALS_RET_FAIL(it.type(), DBus::DataType::INT32);

    it.next();
    TEST_EQUALS_RET_FAIL(it.type(), DBus::DataType::INT32);

    it.next();
    TEST_EQUALS_RET_FAIL(it.type(), DBus::DataType::INVALID);

    // Check the output signature is the same as the input
    TEST_EQUALS_RET_FAIL(sig.str(), sig.begin().signature());

    return true;
}

bool signature_iterate_struct() {
    DBus::Signature sig( "(bdi)" );

    DBus::SignatureIterator it = sig.begin();
    TEST_EQUALS_RET_FAIL( it.is_container(), true );
    TEST_EQUALS_RET_FAIL( it.is_array(), false );
    TEST_EQUALS_RET_FAIL( it.is_dict(), false );
    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::STRUCT );

    DBus::SignatureIterator subit = it.recurse();
    TEST_EQUALS_RET_FAIL( subit.type(), DBus::DataType::BOOLEAN );
    subit.next();
    TEST_EQUALS_RET_FAIL( subit.type(), DBus::DataType::DOUBLE );
    subit.next();
    TEST_EQUALS_RET_FAIL( subit.type(), DBus::DataType::INT32 );
    subit.next();
    TEST_EQUALS_RET_FAIL( subit.type(), DBus::DataType::INVALID );

    // Check the output signature is the same as the input
    TEST_EQUALS_RET_FAIL(sig.str(), sig.begin().signature());

    return true;
}

bool signature_iterate_nested_with_more_data() {
    DBus::Signature sig( "aaidd" );

    DBus::SignatureIterator it = sig.begin();

    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::ARRAY );
    it.next();
    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::DOUBLE );
    it.next();
    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::DOUBLE );
    it.next();
    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::INVALID );

    // Check the output signature is the same as the input
    TEST_EQUALS_RET_FAIL(sig.str(), sig.begin().signature());

    return true;
}

bool signature_iterate_dict_and_data() {
    DBus::Signature sig( "a{bs}ix" );

    DBus::SignatureIterator it = sig.begin();

    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::ARRAY );

    DBus::SignatureIterator subit = it.recurse();

    it.next();
    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::INT32 );
    it.next();
    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::INT64 );
    it.next();
    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::INVALID );

    // Check the output signature is the same as the input
    std::string test =
        sig.begin().signature();

    TEST_EQUALS_RET_FAIL(sig.str(), sig.begin().signature());

    return true;
}

bool signature_iterate_struct_and_data() {
    DBus::Signature sig( "(bs)ix" );

    DBus::SignatureIterator it = sig.begin();

    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::STRUCT );
    it.next();
    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::INT32 );
    it.next();
    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::INT64 );
    it.next();
    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::INVALID );

    // Check the output signature is the same as the input
    TEST_EQUALS_RET_FAIL(sig.str(), sig.begin().signature());

    return true;
}

bool signature_unbalanced_struct() {
    DBus::Signature sig( "(b" );

    TEST_EQUALS_RET_FAIL( sig.is_valid(), false );

    return true;
}

bool signature_iterate_nested_struct() {
    DBus::Signature signature( "(i(id)ss)" );

    TEST_EQUALS_RET_FAIL(signature.is_valid(), true);

    DBus::SignatureIterator it = signature.begin();

    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::STRUCT );

    DBus::SignatureIterator subit = it.recurse();
    TEST_EQUALS_RET_FAIL( subit.type(), DBus::DataType::INT32 );
    subit.next();
    TEST_EQUALS_RET_FAIL( subit.type(), DBus::DataType::STRUCT );

    DBus::SignatureIterator subit2 = subit.recurse();
    TEST_EQUALS_RET_FAIL( subit2.type(), DBus::DataType::INT32 );
    subit2.next();
    TEST_EQUALS_RET_FAIL( subit2.type(), DBus::DataType::DOUBLE );
    subit2.next();
    TEST_EQUALS_RET_FAIL( subit2.type(), DBus::DataType::INVALID );

    subit.next();
    TEST_EQUALS_RET_FAIL( subit.type(), DBus::DataType::STRING);

    subit.next();
    TEST_EQUALS_RET_FAIL( subit.type(), DBus::DataType::STRING);

    subit.next();
    TEST_EQUALS_RET_FAIL( subit.type(), DBus::DataType::INVALID );

    it.next();
    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::INVALID);

    // Check the output signature is the same as the input
    TEST_EQUALS_RET_FAIL(signature.str(), signature.begin().signature());

    return true;
}

bool signature_iterate_nested_dict()
{
    DBus::Signature sig( "a{b{bs}}ix" );

    DBus::SignatureIterator it = sig.begin();

    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::ARRAY );

    DBus::SignatureIterator subit = it.recurse();

    it.next();
    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::INT32 );
    it.next();
    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::INT64 );
    it.next();
    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::INVALID );

    // Check the output signature is the same as the input
    TEST_EQUALS_RET_FAIL(sig.str(), sig.begin().signature());

    return true;
}

bool signature_single_bool() {
    DBus::Signature sig( "b" );

    DBus::SignatureIterator it = sig.begin();

    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::BOOLEAN );

    // Check the output signature is the same as the input
    TEST_EQUALS_RET_FAIL(sig.str(), sig.begin().signature());

    return true;
}

bool signature_create_from_struct_in_array() {
    std::vector<std::tuple<int32_t, uint64_t>> vector_type;

    std::string sig_output = DBus::signature( vector_type );

    return sig_output == "a(it)";
}

bool signature_single_type1() {
    DBus::Signature sig( "i" );

    TEST_EQUALS_RET_FAIL( sig.is_singleton(), true );

    return true;
}

bool signature_single_type2() {
    DBus::Signature sig( "ai" );

    TEST_EQUALS_RET_FAIL( sig.is_singleton(), true );

    return true;
}

bool signature_double_struct() {
    DBus::Signature sig( "(sa(us))" );

    DBus::SignatureIterator it = sig.begin();

    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::STRUCT );
    auto subit = it.recurse();
    TEST_EQUALS_RET_FAIL( subit.type(), DBus::DataType::STRING );
    subit.next();
    TEST_EQUALS_RET_FAIL( subit.type(), DBus::DataType::ARRAY );
    auto subit2 = subit.recurse();
    TEST_EQUALS_RET_FAIL( subit2.type(), DBus::DataType::STRUCT );
    auto subit3 = subit2.recurse();
    TEST_EQUALS_RET_FAIL( subit3.type(), DBus::DataType::UINT32 );
    subit3.next();
    TEST_EQUALS_RET_FAIL( subit3.type(), DBus::DataType::STRING );
    subit3.next();
    TEST_EQUALS_RET_FAIL( subit3.type(), DBus::DataType::INVALID );

    subit2.next();
    TEST_EQUALS_RET_FAIL( subit2.type(), DBus::DataType::INVALID );

    subit.next();
    TEST_EQUALS_RET_FAIL( subit.type(), DBus::DataType::INVALID );

    it.next();
    TEST_EQUALS_RET_FAIL( it.type(), DBus::DataType::INVALID );
    // Check the output signature is the same as the input
    TEST_EQUALS_RET_FAIL( sig.str(), sig.begin().signature() );

    return true;
}

#define ADD_TEST(name) do{ if( test_name == STRINGIFY(name) ){ \
            ret = signature_##name();\
        } \
    } while( 0 )

int main( int argc, char** argv ) {
    if( argc < 2 ) {
        return 1;
    }

    std::string test_name(argv[1]);
    bool ret = false;

    ADD_TEST( create );
    ADD_TEST( iterate_arrayint );
    ADD_TEST( iterate_arrayint2 );
    ADD_TEST( iterate_nested_array );
    ADD_TEST( iterate_multiple_types );
    ADD_TEST( iterate_dictionary );
    ADD_TEST( iterate_struct );
    ADD_TEST( iterate_nested_with_more_data );
    ADD_TEST( iterate_dict_and_data );
    ADD_TEST( iterate_struct_and_data );
    ADD_TEST( iterate_nested_struct );
    ADD_TEST( iterate_nested_dict );
    ADD_TEST( unbalanced_struct );
    ADD_TEST( single_bool );

    ADD_TEST( create_from_struct_in_array );
    ADD_TEST( single_type1 );
    ADD_TEST( single_type2 );
    ADD_TEST( double_struct );

    std::cout << "Test case \"" + test_name + "\" " + (ret ? "PASSED" : "FAIL") << std::endl;
    return !ret;
}

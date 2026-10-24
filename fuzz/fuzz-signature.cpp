// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
/***************************************************************************
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
#include <dbus-cxx/signature.h>
#include <dbus-cxx/signatureiterator.h>
#include <dbus-cxx/validator.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>

/*
 * Report a broken invariant as a crash so that libFuzzer saves the input.
 */
static void invariant( bool condition, const char* what, const std::string& sig ) {
    if( !condition ) {
        fprintf( stderr, "Invariant failed for signature '%s': %s\n", sig.c_str(), what );
        abort();
    }
}

/*
 * Visit every node of the signature tree, calling all of the accessors.
 */
static void walk_signature( DBus::SignatureIterator iter ) {
    while( iter.is_valid() ) {
        iter.type();
        iter.element_type();
        iter.is_basic();
        iter.is_fixed();
        iter.is_array();
        iter.is_dict();
        iter.signature();

        if( iter.is_container() ) {
            walk_signature( iter.recurse() );
        }

        if( !iter.next() ) {
            break;
        }
    }
}

/*
 * Describe the shape of the parsed signature tree, e.g. "a<r<is>>i" for
 * "a(is)i": each node's type code, with the children of arrays, structs and
 * dict entries in angle brackets.
 */
static std::string describe_tree( DBus::SignatureIterator iter ) {
    std::string shape;

    while( iter.is_valid() ) {
        DBus::DataType type = iter.type();
        shape += static_cast<char>( type );

        if( type == DBus::DataType::ARRAY ||
            type == DBus::DataType::STRUCT ||
            type == DBus::DataType::DICT_ENTRY ) {
            shape += "<" + describe_tree( iter.recurse() ) + ">";
        }

        if( !iter.next() ) {
            break;
        }
    }

    return shape;
}

/*
 * Independent reference parser for a spec-valid signature: produces the
 * shape of one complete type starting at pos, in the same format as
 * describe_tree().
 */
static std::string reference_shape( const std::string& sig, size_t& pos ) {
    char c = sig[ pos++ ];

    if( c == 'a' ) {
        if( sig[ pos ] == '{' ) {
            pos++;
            std::string key( 1, sig[ pos++ ] );
            std::string value = reference_shape( sig, pos );
            pos++; // '}'
            return "a<e<" + key + value + ">>";
        }

        return "a<" + reference_shape( sig, pos ) + ">";
    }

    if( c == '(' ) {
        std::string members;

        while( sig[ pos ] != ')' ) {
            members += reference_shape( sig, pos );
        }

        pos++; // ')'
        return "r<" + members + ">";
    }

    return std::string( 1, c );
}

static std::string reference_shape( const std::string& sig ) {
    std::string shape;
    size_t pos = 0;

    while( pos < sig.size() ) {
        shape += reference_shape( sig, pos );
    }

    return shape;
}

/*
 * libFuzzer entry point: treat the fuzz input as a signature string.
 */
extern "C" int LLVMFuzzerTestOneInput( const uint8_t* data, size_t size ) {
    std::string str( reinterpret_cast<const char*>( data ), size );

    bool spec_valid = DBus::Validator::validate_signature( str );
    DBus::Signature sig( str );

    sig.is_singleton();

    std::ostringstream tree;
    sig.print_tree( &tree );

    walk_signature( sig.begin() );

    if( spec_valid ) {
        // Signature::is_valid() is allowed to be more lenient than the
        // specification, but must accept everything that the spec allows.
        invariant( sig.is_valid(), "valid per spec, but Signature::is_valid() is false", str );

        // Walking the parsed tree must give back the original signature.
        invariant( sig.begin().signature() == str, "parsed signature does not match the input", str );

        // The parsed tree must have the structure that the spec defines.
        invariant( describe_tree( sig.begin() ) == reference_shape( str ),
            "parsed tree has the wrong structure", str );
    }

    return 0;
}

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
#include <dbus-cxx/message.h>
#include <dbus-cxx/messageiterator.h>
#include <dbus-cxx/variant.h>
#include <dbus-cxx/error.h>

#include <cstddef>
#include <cstdint>
#include <sstream>
#include <vector>

/*
 * Extract every value from the iterator according to the message signature,
 * recursing into containers, the same way that application code would.
 */
static void walk_iterator( DBus::MessageIterator& iter ) {
    while( iter.is_valid() ) {
        switch( iter.arg_type() ) {
        case DBus::DataType::BYTE:
            iter.get_uint8();
            break;

        case DBus::DataType::BOOLEAN:
            iter.get_bool();
            break;

        case DBus::DataType::INT16:
            iter.get_int16();
            break;

        case DBus::DataType::UINT16:
            iter.get_uint16();
            break;

        case DBus::DataType::INT32:
            iter.get_int32();
            break;

        case DBus::DataType::UINT32:
            iter.get_uint32();
            break;

        case DBus::DataType::INT64:
            iter.get_int64();
            break;

        case DBus::DataType::UINT64:
            iter.get_uint64();
            break;

        case DBus::DataType::DOUBLE:
            iter.get_double();
            break;

        case DBus::DataType::STRING:
        case DBus::DataType::OBJECT_PATH:
        case DBus::DataType::SIGNATURE:
            iter.get_string();
            break;

        case DBus::DataType::UNIX_FD:
            iter.get_filedescriptor();
            break;

        case DBus::DataType::VARIANT: {
            // Printing the variant walks all of its contents
            std::ostringstream out;
            out << iter.get_variant();
            break;
        }

        case DBus::DataType::ARRAY:
        case DBus::DataType::STRUCT:
        case DBus::DataType::DICT_ENTRY: {
            DBus::MessageIterator subiter = iter.recurse();
            walk_iterator( subiter );
            break;
        }

        case DBus::DataType::INVALID:
            return;
        }

        if( !iter.next() ) {
            break;
        }
    }
}

/*
 * libFuzzer entry point: parse the fuzz input as a raw message, and then
 * extract all of the arguments from the body of the message.
 */
extern "C" int LLVMFuzzerTestOneInput( const uint8_t* data, size_t size ) {
    if( size > UINT32_MAX ) {
        return -1;
    }

    // Copy into a buffer of exactly the input size so that ASAN catches any
    // read past the end of the data.
    std::vector<uint8_t> buffer( data, data + size );
    std::shared_ptr<DBus::Message> msg;

    try {
        msg = DBus::Message::create_from_data( buffer.data(), static_cast<uint32_t>( buffer.size() ) );
    } catch( const DBus::ErrorInvalidMessage& ) {
        // Not a valid message; the other fuzzer covers this
        return -1;
    }

    try {
        DBus::MessageIterator iter( msg );
        walk_iterator( iter );
    } catch( const DBus::ErrorInvalidMessage& ) {
        // Rejecting a malformed body is the expected failure mode.
    } catch( const DBus::ErrorMessageNestedTooDeep& ) {
    } catch( const DBus::ErrorUnableToParse& ) {
    }

    return 0;
}

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
#include <dbus-cxx/error.h>

#include <cstddef>
#include <cstdint>
#include <vector>

/*
 * libFuzzer entry point: treat the fuzz input as a raw message received off
 * the wire and attempt to parse it, the same way the transports do.
 */
extern "C" int LLVMFuzzerTestOneInput( const uint8_t* data, size_t size ) {
    if( size > UINT32_MAX ) {
        return -1;
    }

    // Copy into a buffer of exactly the input size so that ASAN catches any
    // read past the end of the data.
    std::vector<uint8_t> buffer( data, data + size );

    try {
        std::shared_ptr<DBus::Message> msg =
            DBus::Message::create_from_data( buffer.data(), static_cast<uint32_t>( buffer.size() ) );
    } catch( const DBus::ErrorInvalidMessage& ) {
        // Rejecting malformed input is the expected failure mode.
    }

    return 0;
}

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
#ifndef DBUSCXX_DEMARSHALING_H
#define DBUSCXX_DEMARSHALING_H

#include <stdint.h>
#include <dbus-cxx/variant.h>
#include <dbus-cxx/path.h>
#include <dbus-cxx/signature.h>
#include <dbus-cxx/enums.h>

namespace DBus{

/**
 * Routines for demarshaling data.
 *
 * All demarshal*() methods will advance the internal data pointer the correct
 * number of bytes to read the next piece of data.
 */
class Demarshaling {
public:
    Demarshaling();

    /**
     * Create a new Demarshaling class that operates on the given
     * C array of data, with the given endianess.
     */
    Demarshaling( const uint8_t* data, uint32_t dataLen, Endianess endian );

    /**
     * Set the data C array to marshal/demarshal.  This also has
     * the side effect of resetting the data offset to 0, in
     * cases where you are demarshaling data.
     *
     * @param data
     */
    void setData( const uint8_t* data, uint32_t dataLen );

    void setEndianess( Endianess endian );

    /**
     * Set the offset in the data to demarshal.
     *
     * @param offset
     */
    void setDataOffset( uint32_t offset );

    void align( int alignment );

    uint32_t current_offset() const;

    uint8_t demarshal_uint8_t();
    bool demarshal_boolean();
    int16_t demarshal_int16_t();
    uint16_t demarshal_uint16_t();
    int32_t demarshal_int32_t();
    uint32_t demarshal_uint32_t();
    int64_t demarshal_int64_t();
    uint64_t demarshal_uint64_t();
    double demarshal_double();
    std::string demarshal_string();
    Path demarshal_path();
    Signature demarshal_signature();
    Variant demarshal_variant();

private:
    /**
     * Checks to make sure that we're not overruing any array via an assertion.
     *
     * @param numBytesWanted The number of bytes that we want to pull out of the array.
     */
    void is_valid( uint32_t numBytesWanted );
    int16_t demarshalShortBig();
    int16_t demarshalShortLittle();
    int32_t demarshalIntBig();
    int32_t demarshalIntLittle();
    int64_t demarshalLongBig();
    int64_t demarshalLongLittle();

private:
    const uint8_t* m_data;
    uint32_t m_dataLen;
    uint32_t m_dataPos;
    Endianess m_endian;
};

}

#endif /* DBUSCXX_DEMARSHALING_H */
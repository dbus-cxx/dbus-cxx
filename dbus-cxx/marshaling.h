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
#ifndef DBUSCXX_MARSHALING_H
#define DBUSCXX_MARSHALING_H

#include <stdint.h>
#include <vector>
#include <dbus-cxx/path.h>
#include <dbus-cxx/signature.h>
#include <dbus-cxx/enums.h>
#include <dbus-cxx/dbus-cxx-config.h>

namespace DBus{

class Variant;

/**
 * Implements the marshaling algorithms on a given vector of data.
 *
 * Note that all marshal() methods will always append to the given buffer.
 */
class Marshaling {
public:
	Marshaling();

	/**
	 * Create a new Marshaling class that operates on the given
	 * vector of data, with the given endianess.
	 */
	Marshaling( std::vector<uint8_t>* data, Endianess endian );

    ~Marshaling();

    /**
     * Set the data vector to marshal/demarshal.
     *
     * @param data
     */
	void setData( std::vector<uint8_t>* data );

    void setEndianess( Endianess endian );

	void marshal( bool v );
	void marshal( uint8_t v );
	void marshal( int16_t v );
	void marshal( uint16_t v );
	void marshal( int32_t v );
	void marshal( uint32_t v );
	void marshal( int64_t v );
	void marshal( uint64_t v );
	void marshal( double v );
	void marshal( std::string v );
	void marshal( Path v );
	void marshal( Signature v );
	void marshal( const Variant& v );

    void align( int alignment );

    /**
     * Marshal a uint32_t value at the given offset.  This is
     * only used to update the length of a marshaled array.
     *
     * @param offset The location to set the value
     * @param value The actual value to set.
     */
    void marshalAtOffset( uint32_t offset, uint32_t value );

    uint32_t currentOffset() const;

private:
	void marshalShortBig( uint16_t toMarshal );
	void marshalIntBig( uint32_t toMarshal );
	void marshalLongBig( uint64_t toMarshal );
	void marshalShortLittle( uint16_t toMarshal );
	void marshalIntLittle( uint32_t toMarshal );
	void marshalLongLittle( uint64_t toMarshal );

private:
    class priv_data;

    std::shared_ptr<priv_data> m_priv;
};

}

#endif

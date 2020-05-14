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
#ifndef DBUSCXX_VARIANT_H
#define DBUSCXX_VARIANT_H

#include <dbus-cxx/enums.h>
#include <dbus-cxx/path.h>
#include <dbus-cxx/signature.h>
#include <dbus-cxx/dbus-cxx-config.h>
#include <string>
#include <any>
#include <stdint.h>
#include <ostream>

namespace DBus {

class MessageIterator;
class FileDescriptor;

/**
 * A Variant is a type-safe union for DBus operations.
 * Note that due to limitations in how dbus-cxx handles types,
 * it is not possible to store maps or vectors inside of the
 * variant.
 */
class Variant {
  public:
    Variant();
    Variant( uint8_t byte );
    Variant( bool b );
    Variant( int16_t i );
    Variant( uint16_t i );
    Variant( int32_t i );
    Variant( uint32_t i );
    Variant( int64_t i );
    Variant( uint64_t i );
    Variant( double i );
    Variant( const char* cstr );
    Variant( std::string str );
    explicit Variant( Signature sig );
    explicit Variant( Path path );
    explicit Variant( std::shared_ptr<FileDescriptor> fd );
    Variant( const Variant& other );
    ~Variant();

    Signature signature() const;

    DataType currentType() const;

    std::any value() const;

    const std::vector<uint8_t>* marshaled() const;

    int data_alignment() const;

    bool operator==( const Variant& other ) const;

    Variant& operator=( const Variant& other );

    static Variant createFromMessage( MessageIterator iter );

  private:
    class priv_data;

    DBUS_CXX_PROPAGATE_CONST(std::unique_ptr<priv_data>) m_priv;

    friend std::ostream& operator<<( std::ostream& os, const Variant& var );
};

} /* namespace DBus */

#endif /* DBUSCXX_VARIANT_H */

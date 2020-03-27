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
#include <dbus-cxx/variant.h>
#include <dbus-cxx/messageiterator.h>
#include <dbus-cxx/marshaling.h>
#include <dbus-cxx/dbus-cxx-private.h>
#include <stdint.h>
#include <dbus/dbus.h>
#include "enums.h"
#include "path.h"
#include "signature.h"

namespace DBus { class FileDescriptor; }

using DBus::Variant;

Variant::Variant() :
    m_currentType( DataType::INVALID )
{}

Variant::Variant( uint8_t byte ) :
    m_currentType( DataType::BYTE ),
    m_signature( DBus::signature( byte ) ),
    m_data( byte ),
    m_dataAlignment( 1 )
{
	Marshaling marshal( &m_marshaled, Endianess::Big );
	marshal.marshal( byte );
}

Variant::Variant( bool b ) :
    m_currentType( DataType::BOOLEAN ),
    m_signature( DBus::signature( b ) ),
    m_data( b ),
    m_dataAlignment( 4 )
{
	Marshaling marshal( &m_marshaled, Endianess::Big );
	marshal.marshal( b );
}

Variant::Variant( int16_t i ) :
    m_currentType( DataType::INT16 ),
    m_signature( DBus::signature( i ) ),
    m_data( i ),
    m_dataAlignment( 2 )
{
	Marshaling marshal( &m_marshaled, Endianess::Big );
	marshal.marshal( i );
}

Variant::Variant( uint16_t i ) :
    m_currentType( DataType::UINT16 ),
    m_signature( DBus::signature( i ) ),
    m_data( i ),
    m_dataAlignment( 2 )
{
	Marshaling marshal( &m_marshaled, Endianess::Big );
	marshal.marshal( i );
}

Variant::Variant( int32_t i ) :
    m_currentType( DataType::INT32 ),
    m_signature( DBus::signature( i ) ),
    m_data( i ),
    m_dataAlignment( 4 )
{
	Marshaling marshal( &m_marshaled, Endianess::Big );
	marshal.marshal( i );
}

Variant::Variant( uint32_t i ) :
    m_currentType( DataType::UINT32 ),
    m_signature( DBus::signature( i ) ),
    m_data( i ),
    m_dataAlignment( 4 )
{
	Marshaling marshal( &m_marshaled, Endianess::Big );
	marshal.marshal( i );
}

Variant::Variant( int64_t i ) :
    m_currentType( DataType::INT64 ),
    m_signature( DBus::signature( i ) ),
    m_data( i ),
    m_dataAlignment( 8 )
{
	Marshaling marshal( &m_marshaled, Endianess::Big );
	marshal.marshal( i );
}

Variant::Variant( uint64_t i ) :
    m_currentType( DataType::UINT64 ),
    m_signature( DBus::signature( i ) ),
    m_data( i ),
    m_dataAlignment( 8 )
{
	Marshaling marshal( &m_marshaled, Endianess::Big );
	marshal.marshal( i );
}

Variant::Variant( double i ) :
    m_currentType( DataType::DOUBLE ),
    m_signature( DBus::signature( i ) ),
    m_data( i ),
    m_dataAlignment( 8 )
{
	Marshaling marshal( &m_marshaled, Endianess::Big );
	marshal.marshal( i );
}

Variant::Variant( const char* cstr ) :
    Variant( std::string( cstr ) ){}

Variant::Variant( std::string str ) :
    m_currentType( DataType::STRING ),
    m_signature( DBus::signature( str ) ),
    m_data( str ),
    m_dataAlignment( 4 )
{
	Marshaling marshal( &m_marshaled, Endianess::Big );
	marshal.marshal( str );
}

Variant::Variant( DBus::Signature sig ) :
    m_currentType( DataType::SIGNATURE ),
    m_signature( DBus::signature( sig ) ),
    m_data( sig ),
    m_dataAlignment( 1 )
{
	Marshaling marshal( &m_marshaled, Endianess::Big );
	marshal.marshal( sig );
}

Variant::Variant( DBus::Path path ) :
    m_currentType( DataType::OBJECT_PATH ),
    m_signature( DBus::signature( path ) ),
    m_data( path ),
    m_dataAlignment( 4 )
{
	Marshaling marshal( &m_marshaled, Endianess::Big );
	marshal.marshal( path );

    std::ostringstream debug_msg;
    debug_msg << "marshaled path in variant.  size: " << m_marshaled.size();
    SIMPLELOGGER_DEBUG( "DBus.Variant", debug_msg.str() );
}

Variant::Variant( std::shared_ptr<FileDescriptor> fd ) :
    m_currentType( DataType::UNIX_FD ),
    m_signature( DBus::signature( fd ) ),
    m_data( fd ),
    m_dataAlignment( 4 )
{}

Variant::Variant( const Variant& other ) :
    m_signature( other.m_signature ),
    m_currentType( other.m_currentType ),
    m_data( other.m_data ),
    m_marshaled( other.m_marshaled ),
    m_dataAlignment( other.m_dataAlignment ) {
}

DBus::Signature Variant::signature() const {
    return m_signature;
}

DBus::DataType Variant::currentType() const {
    return m_currentType;
}

std::any Variant::value() const {
    return m_data;
}

DBus::Variant Variant::createFromMessage( MessageIterator iter ){
    std::string signature = iter.signature();
    if( signature == DBUS_TYPE_BYTE_AS_STRING ){
        return Variant( (uint8_t)iter );
    }
    if( signature == DBUS_TYPE_BOOLEAN_AS_STRING ){
        return Variant( (bool)iter );
    }
    if( signature == DBUS_TYPE_INT16_AS_STRING ){
        return Variant( (int16_t)iter );
    }
    if( signature == DBUS_TYPE_UINT16_AS_STRING ){
        return Variant( (uint16_t)iter );
    }
    if( signature == DBUS_TYPE_INT32_AS_STRING ){
        return Variant( (int32_t)iter );
    }
    if( signature == DBUS_TYPE_UINT32_AS_STRING ){
        return Variant( (uint32_t)iter );
    }
    if( signature == DBUS_TYPE_INT64_AS_STRING ){
        return Variant( (int64_t)iter );
    }
    if( signature == DBUS_TYPE_UINT64_AS_STRING ){
        return Variant( (uint64_t)iter );
    }
    if( signature == DBUS_TYPE_DOUBLE_AS_STRING ){
        return Variant( (double)iter );
    }
    if( signature == DBUS_TYPE_STRING_AS_STRING ){
        return Variant( (std::string)iter );
    }
    if( signature == DBUS_TYPE_SIGNATURE_AS_STRING ){
        return Variant( (Signature)iter );
    }
    if( signature == DBUS_TYPE_OBJECT_PATH_AS_STRING ){
        return Variant( (Path)iter );
    }
    if( signature == DBUS_TYPE_VARIANT_AS_STRING ){
        return DBUSCXX_MESSAGEITERATOR_OPERATOR_VARIANT( iter );
    }
    if( signature == DBUS_TYPE_UNIX_FD_AS_STRING ){
        return Variant( (std::shared_ptr<FileDescriptor>)iter );
    }

    return Variant();
}

const std::vector<uint8_t>* Variant::marshaled() const {
    return &m_marshaled;
}

int Variant::data_alignment() const {
    return m_dataAlignment;
}

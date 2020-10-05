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
#include <dbus-cxx/signatureiterator.h>
#include <stdint.h>
#include "enums.h"
#include "path.h"
#include "signature.h"

static const char* LOGGER_NAME = "DBus.Variant";

namespace DBus { class FileDescriptor; }

using DBus::Variant;

Variant::Variant():
    m_currentType( DataType::INVALID )
{}

Variant::Variant( uint8_t byte ) :
    m_currentType( DataType::BYTE ),
    m_signature( DBus::signature( byte ) ),
    m_dataAlignment( 1 ) {
    Marshaling marshal( &m_marshaled, Endianess::Big );
    marshal.marshal( byte );
}

Variant::Variant( bool b ) :
    m_currentType( DataType::BOOLEAN ),
    m_signature( DBus::signature( b ) ),
    m_dataAlignment( 4 ) {
    Marshaling marshal( &m_marshaled, Endianess::Big );
    marshal.marshal( b );
}

Variant::Variant( int16_t i ) :
    m_currentType( DataType::INT16 ),
    m_signature( DBus::signature( i ) ),
    m_dataAlignment( 2 ) {
    Marshaling marshal( &m_marshaled, Endianess::Big );
    marshal.marshal( i );
}

Variant::Variant( uint16_t i ):
    m_currentType( DataType::UINT16 ),
    m_signature( DBus::signature( i ) ),
    m_dataAlignment( 2 ) {
    Marshaling marshal( &m_marshaled, Endianess::Big );
    marshal.marshal( i );
}

Variant::Variant( int32_t i ) :
    m_currentType( DataType::INT32 ),
    m_signature( DBus::signature( i ) ),
    m_dataAlignment( 4 ) {
    Marshaling marshal( &m_marshaled, Endianess::Big );
    marshal.marshal( i );
}

Variant::Variant( uint32_t i ) :
    m_currentType( DataType::UINT32 ),
    m_signature( DBus::signature( i ) ),
    m_dataAlignment( 4 ) {
    Marshaling marshal( &m_marshaled, Endianess::Big );
    marshal.marshal( i );
}

Variant::Variant( int64_t i ) :
    m_currentType( DataType::INT64 ),
    m_signature( DBus::signature( i ) ),
    m_dataAlignment( 8 ) {
    Marshaling marshal( &m_marshaled, Endianess::Big );
    marshal.marshal( i );
}

Variant::Variant( uint64_t i ) :
    m_currentType( DataType::UINT64 ),
    m_signature( DBus::signature( i ) ),
    m_dataAlignment( 8 ) {
    Marshaling marshal( &m_marshaled, Endianess::Big );
    marshal.marshal( i );
}

Variant::Variant( double i ) :
    m_currentType( DataType::DOUBLE ),
    m_signature( DBus::signature( i ) ),
    m_dataAlignment( 8 ) {
    Marshaling marshal( &m_marshaled, Endianess::Big );
    marshal.marshal( i );
}

Variant::Variant( const char* cstr ) :
    Variant( std::string( cstr ) ) {}

Variant::Variant( std::string str ) :
    m_currentType( DataType::STRING ),
    m_signature( DBus::signature( str ) ),
    m_dataAlignment( 4 ) {
    Marshaling marshal( &m_marshaled, Endianess::Big );
    marshal.marshal( str );
}

Variant::Variant( DBus::Signature sig ) :
    m_currentType( DataType::SIGNATURE ),
    m_signature( DBus::signature( sig ) ),
    m_dataAlignment( 1 ) {
    Marshaling marshal( &m_marshaled, Endianess::Big );
    marshal.marshal( sig );
}

Variant::Variant( DBus::Path path )  :
    m_currentType( DataType::OBJECT_PATH ),
    m_signature( DBus::signature( path ) ),
    m_dataAlignment( 4 ) {
    Marshaling marshal( &m_marshaled, Endianess::Big );
    marshal.marshal( path );
}

Variant::Variant( const Variant& other ) :
    m_currentType( other.m_currentType ),
    m_signature( other.m_signature ),
    m_marshaled( other.m_marshaled ),
    m_dataAlignment( other.m_dataAlignment )
{}

Variant::~Variant() {}

DBus::Signature Variant::signature() const {
    return m_signature;
}

DBus::DataType Variant::currentType() const {
    return m_currentType;
}

DBus::Variant Variant::createFromMessage( MessageIterator iter ) {
    Variant v;
    DBus::DataType dt = iter.signature_iterator().type();
    TypeInfo ti( dt );
    Marshaling marshal( &v.m_marshaled, Endianess::Big );

    v.m_signature = DBus::Signature( iter.signature() );
    v.m_currentType = dt;
    v.m_dataAlignment = ti.alignment();
    iter.align( ti.alignment() );

    switch( dt ) {
    case DataType::BYTE:
        marshal.marshal( iter.get_uint8() );
        break;

    case  DataType::BOOLEAN:
        marshal.marshal( iter.get_bool() );
        break;

    case  DataType::INT16:
        marshal.marshal( iter.get_int16() );
        break;

    case  DataType::UINT16:
        marshal.marshal( iter.get_uint16() );
        break;

    case  DataType::INT32:
        marshal.marshal( iter.get_int32() );
        break;

    case  DataType::UINT32:
        marshal.marshal( iter.get_uint32() );
        break;

    case  DataType::INT64:
        marshal.marshal( iter.get_int64() );
        break;

    case  DataType::UINT64:
        marshal.marshal( iter.get_uint64() );
        break;

    case  DataType::DOUBLE:
        marshal.marshal( iter.get_double() );
        break;

    case  DataType::STRING:
        marshal.marshal( iter.get_string() );
        break;

    case  DataType::OBJECT_PATH:
        marshal.marshal( iter.get_string() );
        break;

    case  DataType::SIGNATURE:
        marshal.marshal( iter.get_signature() );
        break;

    case  DataType::ARRAY:
        v.recurseArray( iter.recurse(), &marshal );
        break;

    case  DataType::VARIANT:
        break;

    case  DataType::STRUCT:
        v.recurseStruct( iter.recurse(), &marshal );
        break;

    case  DataType::DICT_ENTRY:
    case  DataType::UNIX_FD:
    case  DataType::INVALID:
        break;
    }

    return v;
}

void Variant::recurseArray( MessageIterator iter, Marshaling* marshal ) {
    DataType dt = iter.signature_iterator().type();
    TypeInfo ti( dt );
    std::vector<uint8_t> workingData;
    Marshaling workingMarshal( &workingData, Endianess::Big );

    while( iter.is_valid() ) {
        switch( dt ) {
        case DataType::BYTE:
            workingMarshal.marshal( iter.get_uint8() );
            break;

        case  DataType::BOOLEAN:
            workingMarshal.marshal( iter.get_bool() );
            break;

        case  DataType::INT16:
            workingMarshal.marshal( iter.get_int16() );
            break;

        case  DataType::UINT16:
            workingMarshal.marshal( iter.get_uint16() );
            break;

        case  DataType::INT32:
            workingMarshal.marshal( iter.get_int32() );
            break;

        case  DataType::UINT32:
            workingMarshal.marshal( iter.get_uint32() );
            break;

        case  DataType::INT64:
            workingMarshal.marshal( iter.get_int64() );
            break;

        case  DataType::UINT64:
            workingMarshal.marshal( iter.get_uint64() );
            break;

        case  DataType::DOUBLE:
            workingMarshal.marshal( iter.get_double() );
            break;

        case  DataType::STRING:
            workingMarshal.marshal( iter.get_string() );
            break;

        case  DataType::OBJECT_PATH:
            workingMarshal.marshal( iter.get_string() );
            break;

        case  DataType::SIGNATURE:
            workingMarshal.marshal( iter.get_signature() );
            break;

        case DataType::ARRAY:
            recurseArray( iter.recurse(), &workingMarshal );
            break;

        case DataType::DICT_ENTRY:
            recurseDictEntry( iter.recurse(), &workingMarshal );
            break;

        case DataType::STRUCT:
            recurseStruct( iter.recurse(), &workingMarshal );
            break;
        }

        iter++;
    }

    marshal->marshal( static_cast<uint32_t>( workingData.size() ) );
    marshal->align( ti.alignment() );

    for( uint8_t byte : workingData ) {
        marshal->marshal( byte );
    }
}

void Variant::recurseDictEntry( MessageIterator iter, Marshaling* marshal ) {
    marshal->align( 8 );
    iter.align( 8 );
    SignatureIterator sigit = iter.signature_iterator();

    while( sigit.is_valid() ) {
        DataType dt = sigit.type();
        TypeInfo ti( dt );

        switch( dt ) {
        case DataType::BYTE:
            marshal->marshal( iter.get_uint8() );
            break;

        case  DataType::BOOLEAN:
            marshal->marshal( iter.get_bool() );
            break;

        case  DataType::INT16:
            marshal->marshal( iter.get_int16() );
            break;

        case  DataType::UINT16:
            marshal->marshal( iter.get_uint16() );
            break;

        case  DataType::INT32:
            marshal->marshal( iter.get_int32() );
            break;

        case  DataType::UINT32:
            marshal->marshal( iter.get_uint32() );
            break;

        case  DataType::INT64:
            marshal->marshal( iter.get_int64() );
            break;

        case  DataType::UINT64:
            marshal->marshal( iter.get_uint64() );
            break;

        case  DataType::DOUBLE:
            marshal->marshal( iter.get_double() );
            break;

        case  DataType::STRING:
            marshal->marshal( iter.get_string() );
            break;

        case  DataType::OBJECT_PATH:
            marshal->marshal( iter.get_string() );
            break;

        case  DataType::SIGNATURE:
            marshal->marshal( iter.get_signature() );
            break;

        case DataType::ARRAY:
            recurseArray( iter.recurse(), marshal );
            break;

        case DataType::DICT_ENTRY:
            recurseDictEntry( iter.recurse(), marshal );
            break;
        }

        sigit++;
        iter++;
    }
}

void Variant::recurseStruct( MessageIterator iter, Marshaling* marshal ) {
    marshal->align( 8 );
    iter.align( 8 );
    SignatureIterator sigit = iter.signature_iterator();

    while( sigit.is_valid() ) {
        DataType dt = sigit.type();
        TypeInfo ti( dt );

        switch( dt ) {
        case DataType::BYTE:
            marshal->marshal( iter.get_uint8() );
            break;

        case  DataType::BOOLEAN:
            marshal->marshal( iter.get_bool() );
            break;

        case  DataType::INT16:
            marshal->marshal( iter.get_int16() );
            break;

        case  DataType::UINT16:
            marshal->marshal( iter.get_uint16() );
            break;

        case  DataType::INT32:
            marshal->marshal( iter.get_int32() );
            break;

        case  DataType::UINT32:
            marshal->marshal( iter.get_uint32() );
            break;

        case  DataType::INT64:
            marshal->marshal( iter.get_int64() );
            break;

        case  DataType::UINT64:
            marshal->marshal( iter.get_uint64() );
            break;

        case  DataType::DOUBLE:
            marshal->marshal( iter.get_double() );
            break;

        case  DataType::STRING:
            marshal->marshal( iter.get_string() );
            break;

        case  DataType::OBJECT_PATH:
            marshal->marshal( iter.get_string() );
            break;

        case  DataType::SIGNATURE:
            marshal->marshal( iter.get_signature() );
            break;

        case DataType::ARRAY:
            recurseArray( iter.recurse(), marshal );
            break;

        case DataType::DICT_ENTRY:
            recurseDictEntry( iter.recurse(), marshal );
            break;
        }

        sigit++;
        iter++;
    }
}

const std::vector<uint8_t>* Variant::marshaled() const {
    return &m_marshaled;
}

int Variant::data_alignment() const {
    return m_dataAlignment;
}

bool Variant::operator==( const Variant& other ) const {
    bool sameType = other.currentType() == currentType();
    bool vectorsEqual = false;

    if( sameType ) {
        vectorsEqual = other.m_marshaled == m_marshaled;
    }

    return sameType && vectorsEqual;
}

Variant& Variant::operator=( const Variant& other ) {
    m_currentType = other.m_currentType;
    m_signature = other.m_signature;
    m_marshaled = other.m_marshaled;
    m_dataAlignment = other.m_dataAlignment;

    return *this;
}

bool Variant::to_bool() const {
    if( m_currentType != DataType::BOOLEAN ) {
        throw ErrorBadVariantCast();
    }

    priv::VariantIterator vi( this );
    return vi.get_bool();
}

uint8_t Variant::to_uint8() const {
    if( m_currentType != DataType::BYTE ) {
        throw ErrorBadVariantCast();
    }

    priv::VariantIterator vi( this );
    return vi.get_uint8();
}

uint16_t Variant::to_uint16() const {
    if( m_currentType != DataType::UINT16 ) {
        throw ErrorBadVariantCast();
    }

    priv::VariantIterator vi( this );
    return vi.get_uint16();
}

int16_t Variant::to_int16() const {
    if( m_currentType != DataType::INT16 ) {
        throw ErrorBadVariantCast();
    }

    priv::VariantIterator vi( this );
    return vi.get_int16();
}

uint32_t Variant::to_uint32() const {
    if( m_currentType != DataType::UINT32 ) {
        throw ErrorBadVariantCast();
    }

    priv::VariantIterator vi( this );
    return vi.get_uint32();
}

int32_t Variant::to_int32() const {
    if( m_currentType != DataType::INT32 ) {
        throw ErrorBadVariantCast();
    }

    priv::VariantIterator vi( this );
    return vi.get_int32();
}

uint64_t Variant::to_uint64() const {
    if( m_currentType != DataType::UINT64 ) {
        throw ErrorBadVariantCast();
    }

    priv::VariantIterator vi( this );
    return vi.get_uint64();
}

int64_t Variant::to_int64() const {
    if( m_currentType != DataType::INT64 ) {
        throw ErrorBadVariantCast();
    }

    priv::VariantIterator vi( this );
    return vi.get_int64();
}

double Variant::to_double() const {
    if( m_currentType != DataType::DOUBLE ) {
        throw ErrorBadVariantCast();
    }

    priv::VariantIterator vi( this );
    return vi.get_double();
}

std::string Variant::to_string() const {
    if( m_currentType != DataType::STRING ) {
        throw ErrorBadVariantCast();
    }

    priv::VariantIterator vi( this );
    return vi.get_string();
}

DBus::Path Variant::to_path() const {
    if( m_currentType != DataType::OBJECT_PATH ) {
        throw ErrorBadVariantCast();
    }

    priv::VariantIterator vi( this );
    return vi.get_string();
}

DBus::Signature Variant::to_signature() const {
    if( m_currentType != DataType::SIGNATURE ) {
        throw ErrorBadVariantCast();
    }

    priv::VariantIterator vi( this );
    return vi.get_signature();
}

namespace DBus {

std::ostream& operator<<( std::ostream& os, const Variant& var ) {
    os << "DBus::Variant[" << var.currentType() << "=";

    switch( var.currentType() ) {
    case DataType::BYTE:
        os << var.to_uint8();
        break;

    case DataType::BOOLEAN:
        os << var.to_bool();
        break;

    case DataType::INT16:
        os << var.to_int16();
        break;

    case DataType::UINT16:
        os << var.to_uint16();
        break;

    case DataType::INT32:
        os << var.to_int32();
        break;

    case DataType::UINT32:
        os << var.to_uint32();
        break;

    case DataType::INT64:
        os << var.to_int64();
        break;

    case DataType::UINT64:
        os << var.to_uint64();
        break;

    case DataType::DOUBLE:
        os << var.to_double();
        break;

    case DataType::STRING:
        os << var.to_string();
        break;

    case DataType::OBJECT_PATH:
        os << var.to_path();
        break;

    case DataType::SIGNATURE:
        os << var.to_signature();
        break;

    case DataType::ARRAY:
        os << "array";
        break;

    case DataType::VARIANT:
        os << "variant";
        break;

    case DataType::STRUCT:
        os << "struct";
        break;

    case DataType::DICT_ENTRY:
        os << "dict_entry";
        break;

    case DataType::UNIX_FD:
        os << "unix_fd";
        break;

    case DataType::INVALID:
        break;
    }

    os << "]";
    return os;
}

}

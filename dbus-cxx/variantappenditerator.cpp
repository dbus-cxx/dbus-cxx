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
#include "variantappenditerator.h"
#include "variant.h"
#include "marshaling.h"
#include <dbus-cxx/types.h>
#include <dbus-cxx/marshaling.h>
#include <dbus-cxx/validator.h>

using DBus::priv::VariantAppendIterator;

class VariantAppendIterator::priv_data {
public:
    priv_data( Variant* variant ) :
        m_variant( variant ),
        m_subiter( nullptr ),
        m_currentContainer( ContainerType::None ),
        m_arrayAlignment( 0 )
    {}

    Variant* m_variant;
    VariantAppendIterator* m_subiter;
    ContainerType m_currentContainer;
    std::vector<uint8_t> m_workingBuffer;
    int32_t m_arrayAlignment;
    Marshaling m_marshaling;
};

VariantAppendIterator::VariantAppendIterator( Variant* variant ):
    m_priv( std::make_shared<priv_data>( variant ) ) {
    m_priv->m_marshaling = Marshaling( &variant->m_marshaled, Endianess::Big );
}

VariantAppendIterator::VariantAppendIterator( Variant* variant, ContainerType t ) :
    m_priv( std::make_shared<priv_data>( variant ) ) {
    m_priv->m_currentContainer = t;
    m_priv->m_marshaling = Marshaling( &m_priv->m_workingBuffer, Endianess::Big );
}

VariantAppendIterator::~VariantAppendIterator() {
    delete m_priv->m_subiter;
}

VariantAppendIterator& VariantAppendIterator::operator<<( const bool& v ) {
    m_priv->m_marshaling.marshal( v );

    return *this;
}

VariantAppendIterator& VariantAppendIterator::operator<<( const uint8_t& v ) {
    m_priv->m_marshaling.marshal( v );

    return *this;
}

VariantAppendIterator& VariantAppendIterator::operator<<( const int16_t& v ) {
    m_priv->m_marshaling.marshal( v );

    return *this;
}

VariantAppendIterator& VariantAppendIterator::operator<<( const uint16_t& v ) {
    m_priv->m_marshaling.marshal( v );

    return *this;
}

VariantAppendIterator& VariantAppendIterator::operator<<( const int32_t& v ) {
    m_priv->m_marshaling.marshal( v );

    return *this;
}

VariantAppendIterator& VariantAppendIterator::operator<<( const uint32_t& v ) {
    m_priv->m_marshaling.marshal( v );

    return *this;
}

VariantAppendIterator& VariantAppendIterator::operator<<( const int64_t& v ) {
    m_priv->m_marshaling.marshal( v );

    return *this;
}

VariantAppendIterator& VariantAppendIterator::operator<<( const uint64_t& v ) {
    m_priv->m_marshaling.marshal( v );

    return *this;
}

VariantAppendIterator& VariantAppendIterator::operator<<( const double& v ) {
    m_priv->m_marshaling.marshal( v );

    return *this;
}

VariantAppendIterator& VariantAppendIterator::operator<<( const char* v ) {
    m_priv->m_marshaling.marshal( v );

    return *this;
}

VariantAppendIterator& VariantAppendIterator::operator<<( const std::string& v ) {
    m_priv->m_marshaling.marshal( v );

    return *this;
}

VariantAppendIterator& VariantAppendIterator::operator<<( const Signature& v ) {
    m_priv->m_marshaling.marshal( v );

    return *this;
}

VariantAppendIterator& VariantAppendIterator::operator<<( const Path& v ) {
    m_priv->m_marshaling.marshal( v );

    return *this;
}

VariantAppendIterator& VariantAppendIterator::operator<<( const Variant& v ){
    if( m_priv->m_subiter ) { this->close_container(); }

    // The variant should already be correctly marshaled at this point, so just copy the bytes?
    const std::vector<uint8_t>* marshaled = v.marshaled();
    for( uint8_t byte : *marshaled ){
        m_priv->m_marshaling.marshal( byte );
    }

    return *this;
}

bool VariantAppendIterator::open_container( ContainerType t, const std::string& sig ) {
    std::string signature;
    int32_t array_align = 0;

    if( m_priv->m_subiter ) { this->close_container(); }

    if( t == ContainerType::ARRAY ) {
        Signature tmpSig( sig );
        SignatureIterator tmpSigIter = tmpSig.begin();
        TypeInfo ti( tmpSigIter.type() );
        array_align = ti.alignment();
    }

    m_priv->m_subiter = new VariantAppendIterator( m_priv->m_variant, t );
    m_priv->m_subiter->m_priv->m_arrayAlignment = array_align;

    return true;
}

bool VariantAppendIterator::close_container( ) {
    if( ! m_priv->m_subiter ) { return false; }

    switch( m_priv->m_subiter->m_priv->m_currentContainer ) {
    case ContainerType::None: return false;

    case ContainerType::ARRAY: {
        uint32_t arraySize = static_cast<uint32_t>( m_priv->m_subiter->m_priv->m_workingBuffer.size() );

        if( arraySize > Validator::maximum_array_size() ) {
            return true;
        }

        m_priv->m_marshaling.marshal( arraySize );
        m_priv->m_marshaling.align( m_priv->m_subiter->m_priv->m_arrayAlignment );
    }
    break;

    case ContainerType::DICT_ENTRY:
    case ContainerType::STRUCT:
        m_priv->m_marshaling.align( 8 );
        break;

    case ContainerType::VARIANT:
        break;
    }

    for( const uint8_t& dataByte : m_priv->m_subiter->m_priv->m_workingBuffer ) {
        m_priv->m_marshaling.marshal( dataByte );
    }

    delete m_priv->m_subiter;
    m_priv->m_subiter = nullptr;
    return true;
}

VariantAppendIterator* VariantAppendIterator::sub_iterator() {
    return m_priv->m_subiter;
}

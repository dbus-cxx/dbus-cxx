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

using DBus::VariantAppendIterator;

class VariantAppendIterator::priv_data {
public:
    priv_data( Variant* variant ) :
        m_variant( variant ),
        m_subiter( nullptr ),
        m_type( ContainerType::None )
    {}

    Variant* m_variant;
    VariantAppendIterator* m_subiter;
    ContainerType m_type;
};

VariantAppendIterator::VariantAppendIterator( Variant* variant ):
    m_priv( std::make_shared<priv_data>( variant ) ){}

VariantAppendIterator::VariantAppendIterator( Variant* variant, ContainerType t ) :
    m_priv( std::make_shared<priv_data>( variant ) ){
    m_priv->m_type = t;
}

VariantAppendIterator::~VariantAppendIterator(){
    delete m_priv->m_subiter;
}

void VariantAppendIterator::operator<<( const bool& v ){
    Marshaling marshal( &m_priv->m_variant->m_marshaled, Endianess::Big );
    marshal.marshal( v );
}

void VariantAppendIterator::operator<<( const uint8_t& v ){
    Marshaling marshal( &m_priv->m_variant->m_marshaled, Endianess::Big );
    marshal.marshal( v );
}

void VariantAppendIterator::operator<<( const int16_t& v ){
    Marshaling marshal( &m_priv->m_variant->m_marshaled, Endianess::Big );
    marshal.marshal( v );
}

void VariantAppendIterator::operator<<( const uint16_t& v ){
    Marshaling marshal( &m_priv->m_variant->m_marshaled, Endianess::Big );
    marshal.marshal( v );
}

void VariantAppendIterator::operator<<( const int32_t& v ){
    Marshaling marshal( &m_priv->m_variant->m_marshaled, Endianess::Big );
    marshal.marshal( v );
}

void VariantAppendIterator::operator<<( const uint32_t& v ){
    Marshaling marshal( &m_priv->m_variant->m_marshaled, Endianess::Big );
    marshal.marshal( v );
}

void VariantAppendIterator::operator<<( const int64_t& v ){
    Marshaling marshal( &m_priv->m_variant->m_marshaled, Endianess::Big );
    marshal.marshal( v );
}

void VariantAppendIterator::operator<<( const uint64_t& v ){
    Marshaling marshal( &m_priv->m_variant->m_marshaled, Endianess::Big );
    marshal.marshal( v );
}

void VariantAppendIterator::operator<<( const double& v ){
    Marshaling marshal( &m_priv->m_variant->m_marshaled, Endianess::Big );
    marshal.marshal( v );
}

void VariantAppendIterator::operator<<( const char* v ){
    Marshaling marshal( &m_priv->m_variant->m_marshaled, Endianess::Big );
    marshal.marshal( v );
}

void VariantAppendIterator::operator<<( const std::string& v ){
    Marshaling marshal( &m_priv->m_variant->m_marshaled, Endianess::Big );
    marshal.marshal( v );
}

void VariantAppendIterator::operator<<( const Signature& v ){
    Marshaling marshal( &m_priv->m_variant->m_marshaled, Endianess::Big );
    marshal.marshal( v );
}

void VariantAppendIterator::operator<<( const Path& v ){
    Marshaling marshal( &m_priv->m_variant->m_marshaled, Endianess::Big );
    marshal.marshal( v );
}

bool VariantAppendIterator::open_container( ContainerType t, const std::string& sig ){
    if( t == ContainerType::ARRAY ) {
        Signature tmpSig( sig );
        SignatureIterator tmpSigIter = tmpSig.begin();
        TypeInfo ti( tmpSigIter.type() );
        int array_align = ti.alignment();
        Marshaling marshal( &m_priv->m_variant->m_marshaled, Endianess::Big );
        marshal.marshal( static_cast<uint32_t>( 0 ) );
        marshal.align( array_align );
    }

    if ( m_priv->m_subiter ) this->close_container();

    m_priv->m_subiter = new VariantAppendIterator( m_priv->m_variant, t );

    return true;
}

bool VariantAppendIterator::close_container( ){
    if( !m_priv->m_subiter ){
        return false;
    }

    if( m_priv->m_subiter->m_priv->m_type == ContainerType::ARRAY ){
        Marshaling marshal( &m_priv->m_variant->m_marshaled, Endianess::Big );
        marshal.marshalAtOffset( 0, m_priv->m_variant->m_marshaled.size() - 4 );
    }

    delete m_priv->m_subiter;
    m_priv->m_subiter = nullptr;

    return true;
}

VariantAppendIterator* VariantAppendIterator::sub_iterator(){
    return m_priv->m_subiter;
}

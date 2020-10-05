/***************************************************************************
 *   Copyright (C) 2009 by Rick L. Vinyard, Jr.                            *
 *   rvinyard@cs.nmsu.edu                                                  *
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
#include "signatureiterator.h"
#include <cstring>
#include <algorithm>
#include <iterator>
#include "enums.h"
#include "types.h"

namespace DBus {

class SignatureIterator::priv_data {
public:
    priv_data() :
        m_valid( false )
    {}

    priv_data( std::shared_ptr<priv::SignatureNode> startnode ) :
        m_valid( startnode != nullptr ),
        m_current( startnode ),
        m_first( startnode )
    {}

    bool m_valid;
    std::shared_ptr<priv::SignatureNode> m_current;
    std::shared_ptr<priv::SignatureNode> m_first;
};

SignatureIterator::SignatureIterator():
    m_priv( std::make_unique<priv_data>() ) {
}

SignatureIterator::SignatureIterator( const SignatureIterator& other ) :
    m_priv( std::make_unique<priv_data>() ) {
    *m_priv = *other.m_priv;
}

SignatureIterator::SignatureIterator( std::shared_ptr<priv::SignatureNode> startnode ) :
    m_priv( std::make_unique<priv_data>( startnode ) ) {
}

SignatureIterator::~SignatureIterator() {}

void SignatureIterator::invalidate() {
    m_priv->m_valid = false;
}

bool SignatureIterator::is_valid() const {
    return ( m_priv->m_valid && this->type() != DataType::INVALID );
}

SignatureIterator::operator bool() const {
    return this->is_valid();
}

bool SignatureIterator::next() {
    if( !this->is_valid() ) { return false; }

    if( m_priv->m_current->m_next == nullptr ) {
        m_priv->m_current = nullptr;
        m_priv->m_valid = false;
        return false;
    }

    m_priv->m_current = m_priv->m_current->m_next;

    return true;
}

SignatureIterator& SignatureIterator::operator ++() {
    this->next();
    return ( *this );
}

SignatureIterator SignatureIterator::operator ++( int ) {
    SignatureIterator temp_copy;
    temp_copy = *this;
    ++( *this );
    return temp_copy;
}

bool SignatureIterator::operator==( const SignatureIterator& other ) {
    return m_priv->m_current == other.m_priv->m_current;
}

DataType SignatureIterator::type() const {
    if( !m_priv->m_valid ) { return DataType::INVALID; }

    return m_priv->m_current->m_dataType;
}

DataType SignatureIterator::element_type() const {
    if( this->type() != DataType::ARRAY ) { return DataType::INVALID; }

    SignatureIterator subit( m_priv->m_current->m_sub );
    return subit.type();
}

bool SignatureIterator::is_basic() const {
    TypeInfo t( type() );
    return t.is_basic();
}

bool SignatureIterator::is_fixed() const {
    TypeInfo t( type() );
    return t.is_fixed();
}

bool SignatureIterator::is_container() const {
    TypeInfo t( type() );
    return t.is_container();
}

bool SignatureIterator::is_array() const {
    return this->type() == DataType::ARRAY;
}

bool SignatureIterator::is_dict() const {
    return this->is_array() && this->element_type() == DataType::DICT_ENTRY;
}

SignatureIterator SignatureIterator::recurse() {
    std::string substr;

    if( !this->is_container() ) { return SignatureIterator(); }

    SignatureIterator subiter = SignatureIterator( m_priv->m_current->m_sub );

    return subiter;
}

std::string SignatureIterator::signature() const {
    std::shared_ptr<priv::SignatureNode> tmpCurrent;
    std::string signature;

    if( m_priv->m_first == nullptr ) {
        return "";
    }

    TypeInfo ti( m_priv->m_first->m_dataType );
    char dbusChar = ti.to_dbus_char();

    if( dbusChar ) {
        signature += dbusChar;
    }

    signature += iterate_over_subsig( m_priv->m_first->m_sub );

    for( tmpCurrent = m_priv->m_first->m_next;
        tmpCurrent != nullptr;
        tmpCurrent = tmpCurrent->m_next ) {
        TypeInfo ti( tmpCurrent->m_dataType );
        dbusChar = ti.to_dbus_char();

        if( dbusChar ) {
            signature += dbusChar;
        }

        signature += iterate_over_subsig( tmpCurrent->m_sub );
    }

    return signature;
}

std::string SignatureIterator::iterate_over_subsig( std::shared_ptr<priv::SignatureNode> start ) const {
    std::string retval;

    if( start == nullptr ) {
        return "";
    }

    if( start->m_dataType == DataType::DICT_ENTRY ) {
        retval += "{";
    }

    for( std::shared_ptr<priv::SignatureNode> current = start;
        current != nullptr;
        current = current->m_next ) {
        TypeInfo ti( current->m_dataType );
        char dbusChar = ti.to_dbus_char();

        if( dbusChar ) {
            retval += dbusChar;
        }

        retval += iterate_over_subsig( current->m_sub );
    }

    if( start->m_dataType == DataType::DICT_ENTRY ) {
        retval += "}";
    }

    return retval;
}

SignatureIterator& SignatureIterator::operator=( const SignatureIterator& other ) {
    if( this != &other ) {
        m_priv->m_valid = other.m_priv->m_valid;
        m_priv->m_current = other.m_priv->m_current;
        m_priv->m_first = other.m_priv->m_first;
    }

    return *this;
}

bool SignatureIterator::has_next() const {
    return m_priv->m_current->m_next != nullptr;
}

}


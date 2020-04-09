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

namespace DBus
{

  SignatureIterator::SignatureIterator():
      m_valid(false)
  {
  }

  SignatureIterator::SignatureIterator( const SignatureIterator& other ) :
      m_valid( other.m_valid ),
      m_current( other.m_current ),
      m_first( other.m_first )
  {}

  SignatureIterator::SignatureIterator( std::shared_ptr<priv::SignatureNode> startnode ) :
    m_valid( startnode != nullptr ),
    m_current( startnode ),
    m_first( startnode ) {

  }

  void SignatureIterator::invalidate()
  {
    m_valid = false;
  }

  bool SignatureIterator::is_valid() const
  {
    return ( m_valid and this->type() != DataType::INVALID );
  }

  SignatureIterator::operator bool() const
  {
    return this->is_valid();
  }

  bool SignatureIterator::next()
  {
    if ( not this->is_valid() ) return false;

    if( m_current->m_next == nullptr ){
        m_current = nullptr;
        m_valid = false;
        return false;
    }

    m_current = m_current->m_next;

    return true;
  }

  SignatureIterator& SignatureIterator::operator ++()
  {
    this->next();
    return ( *this );
  }

  SignatureIterator SignatureIterator::operator ++( int )
  {
    SignatureIterator temp_copy;
    temp_copy = *this;
    ++( *this );
    return temp_copy;
  }

  bool SignatureIterator::operator==( const SignatureIterator& other )
  {
    return m_current == other.m_current;
  }

  DataType SignatureIterator::type() const
  {
    if ( not m_valid ) return DataType::INVALID;
    
    return m_current->m_dataType;
  }

  DataType SignatureIterator::element_type() const
  {
    if ( this->type() != DataType::ARRAY ) return DataType::INVALID;
    SignatureIterator subit( m_current->m_sub );
    return subit.type();
  }

  bool SignatureIterator::is_basic() const
  {
      TypeInfo t( type() );
    return t.is_basic();
  }

  bool SignatureIterator::is_fixed() const
  {
      TypeInfo t( type() );
    return t.is_fixed();
  }

  bool SignatureIterator::is_container() const
  {
      TypeInfo t( type() );
    return t.is_container();
  }

  bool SignatureIterator::is_array() const
  {
    return this->type() == DataType::ARRAY;
  }

  bool SignatureIterator::is_dict() const
  {
    return this->is_array() && this->element_type() == DataType::DICT_ENTRY;
  }

  SignatureIterator SignatureIterator::recurse()
  {
    std::string substr;

    if ( not this->is_container() ) return SignatureIterator();

    SignatureIterator subiter = SignatureIterator( m_current->m_sub );
    
    return subiter;
  }

  std::string SignatureIterator::signature() const
  {
    std::shared_ptr<priv::SignatureNode> tmpCurrent;
    std::string signature;

    if( m_first == nullptr ){
        return "";
    }

    TypeInfo ti( m_first->m_dataType );
    signature += ti.to_dbus_char();
    signature += iterate_over_subsig( m_first->m_sub );

    for( tmpCurrent = m_first->m_next;
         tmpCurrent != nullptr;
         tmpCurrent = tmpCurrent->m_next ){
        TypeInfo ti( tmpCurrent->m_dataType );
        signature += ti.to_dbus_char();
        signature += iterate_over_subsig( tmpCurrent->m_sub );
    }

    return signature;
  }

  std::string SignatureIterator::iterate_over_subsig( std::shared_ptr<priv::SignatureNode> start ) const {
      std::string retval;

      if( start == nullptr ){
          return "";
      }

      for( std::shared_ptr<priv::SignatureNode> current = start;
           current != nullptr;
           current = current->m_next ){
          TypeInfo ti( current->m_dataType );
          retval += ti.to_dbus_char();
          retval += iterate_over_subsig( current->m_sub );
      }

      return retval;
  }

SignatureIterator& SignatureIterator::operator=( const SignatureIterator& other ){
    if( this != &other ){
        m_valid = other.m_valid;
        m_current = other.m_current;
        m_first = other.m_first;
    }

    return *this;
}

bool SignatureIterator::has_next() const {
    return m_current->m_next != nullptr;
}

}


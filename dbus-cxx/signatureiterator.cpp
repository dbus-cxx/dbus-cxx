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

  SignatureIterator::SignatureIterator( const std::string& signature ):
      m_valid( true ),
      m_signature( signature ),
      m_it( m_signature.begin() )
  {
  }

  SignatureIterator::SignatureIterator( const SignatureIterator& other ) :
      m_valid( other.m_valid ),
      m_signature( other.m_signature ){
      m_it = m_signature.begin() + ( other.m_it - other.m_signature.begin() );
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

    if( this->is_container() ){
        m_it = std::get<2>( recurse_into_container( m_it ) );
    }

    m_it++;

    if ( this->type() == DataType::INVALID )
    {
      this->invalidate();
      return false;
    }

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
    return other.m_signature == m_signature;
  }

  DataType SignatureIterator::type() const
  {
    if ( not m_valid ) return DataType::INVALID;
    
    return char_to_dbus_type( *m_it );
  }

  DataType SignatureIterator::element_type() const
  {
    if ( this->type() != DataType::ARRAY ) return DataType::INVALID;
    return char_to_dbus_type( *(m_it + 1) );
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

    std::tuple<std::string,std::string::iterator,std::string::iterator> tupval = recurse_into_container( m_it );
    SignatureIterator subiter = SignatureIterator( std::get<0>( tupval ) );
    
    return subiter;
  }

  std::string SignatureIterator::signature() const
  {
    return m_signature;
  }

std::tuple<std::string,std::string::iterator,std::string::iterator> SignatureIterator::recurse_into_container( std::string::iterator current_pos ){
    std::string signature;
    std::string::iterator start = current_pos + 1;
    std::string::iterator end = start;
    DataType type;
    bool iterateUntilEndOfContainer = false;
    char containerEnd = '\0';

    if( *start == '{' ){
        /* Dict entry; must always come after 'a' */
        containerEnd = '}';
        iterateUntilEndOfContainer = true;
        start++;
    }else if( *current_pos == '(' ){
        containerEnd = ')';
        iterateUntilEndOfContainer = true;
    }

    if( start == m_signature.end() ){
        return std::make_tuple( signature, start, start );
    }

    do{
        signature += *start;
        type = char_to_dbus_type( *start );

        TypeInfo info(type);
        if( info.is_container() && type != DataType::VARIANT ){
            std::tuple<std::string,std::string::iterator,std::string::iterator> value = recurse_into_container( start );
            signature += std::get<0>( value );
            end = std::get<2>( value );
        }

        start++;
        if( *start == containerEnd ){
            iterateUntilEndOfContainer = false;
        }
    }while( iterateUntilEndOfContainer );

    return std::make_tuple( signature, start, end );
}

SignatureIterator& SignatureIterator::operator=( const SignatureIterator& other ){
    if( this != &other ){
        m_valid = other.m_valid;
        m_signature = other.m_signature;
        m_it = m_signature.begin() + ( other.m_it - other.m_signature.begin() );
    }

    return *this;
}

bool SignatureIterator::has_next() const {
    if( m_it != m_signature.end() ){
        std::string::iterator newit = m_it + 1;
        if( newit != m_signature.end() ){
            return true;
        }
    }

    return false;
}

}


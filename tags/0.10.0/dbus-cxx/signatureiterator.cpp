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

#include "types.h"

#include <cstring>

namespace DBus
{

  SignatureIterator::SignatureIterator():
      m_valid(false)
  {
    memset( &m_cobj, 0x00, sizeof( DBusSignatureIter ) );
  }

  SignatureIterator::SignatureIterator( const std::string& signature ):
      m_valid( false )
  {
    memset( &m_cobj, 0x00, sizeof( DBusSignatureIter ) );
    this->init(signature);
  }

  DBusSignatureIter* SignatureIterator::cobj()
  {
    return &m_cobj;
  }

  bool SignatureIterator::init(const std::string& signature)
  {
    bool valid_check = dbus_signature_validate( signature.c_str(), NULL );

    if ( valid_check )
    {
      dbus_signature_iter_init( &m_cobj, signature.c_str() );
      m_valid = true;
    }
    else
    {
      memset( &m_cobj, 0x00, sizeof( DBusSignatureIter ) );
      m_valid = false;
    }

    return m_valid;
  }

  void SignatureIterator::invalidate()
  {
    memset( &m_cobj, 0x00, sizeof( DBusSignatureIter ) );
    m_valid = false;
  }

  bool SignatureIterator::is_valid() const
  {
    return ( m_valid and this->type() != TYPE_INVALID );
  }

  SignatureIterator::operator bool() const
  {
    return this->is_valid();
  }

  bool SignatureIterator::next()
  {
    if ( not this->is_valid() ) return false;

    bool result;

    result = dbus_signature_iter_next( & m_cobj );

    if ( not result or this->type() == TYPE_INVALID )
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
    return ( memcmp( &m_cobj, &( other.m_cobj ), sizeof( DBusSignatureIter ) ) == 0 );
  }

  Type SignatureIterator::type() const
  {
    if ( not m_valid ) return TYPE_INVALID;
    
    return checked_type_cast( dbus_signature_iter_get_current_type(& m_cobj) );
  }

  Type SignatureIterator::element_type() const
  {
    if ( this->type() != TYPE_ARRAY ) return TYPE_INVALID;
    return checked_type_cast( dbus_signature_iter_get_element_type(&m_cobj) );
  }

  bool SignatureIterator::is_basic() const
  {
    return dbus_type_is_basic( this->type() );
  }

  bool SignatureIterator::is_fixed() const
  {
    return dbus_type_is_fixed( this->element_type() );
  }

  bool SignatureIterator::is_container() const
  {
    return dbus_type_is_container( this->type() );
  }

  bool SignatureIterator::is_array() const
  {
    return this->type() == TYPE_ARRAY;
  }

  bool SignatureIterator::is_dict() const
  {
    return this->is_array() && this->element_type() == TYPE_DICT_ENTRY;
  }

  SignatureIterator SignatureIterator::recurse()
  {
    SignatureIterator subiter;

    if ( not this->is_container() ) return subiter;
    
    dbus_signature_iter_recurse( & m_cobj, & ( subiter.m_cobj ) );

    subiter.m_valid = true;
    
    return subiter;
  }

  std::string SignatureIterator::signature() const
  {
    char* sig;
    std::string retsig;

    sig = dbus_signature_iter_get_signature( & m_cobj );

    retsig = sig;

    dbus_free( sig );

    return retsig;
  }

}


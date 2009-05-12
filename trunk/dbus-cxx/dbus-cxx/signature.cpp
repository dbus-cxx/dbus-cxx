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
#include "signature.h"

namespace DBus
{

  const Signature::size_type npos = std::string::npos;

  Signature::Signature()
  {
  }

  Signature::Signature( const std::string & s, size_type pos, size_type n ):
      m_signature( s, pos, n )
  {
  }

  Signature::Signature( const char *s ):
      m_signature( s )
  {
  }

  Signature::Signature( const char * s, size_type n ):
      m_signature( s,n )
  {
  }

  Signature::Signature( size_type n, char c ):
      m_signature( n,c )
  {
  }

  Signature::~Signature()
  {
  }

  Signature::operator const std::string &() const
  {
    return m_signature;
  }

  const std::string& Signature::str() const
  {
    return m_signature;
  }

  Signature & Signature::operator =(const std::string & s)
  {
    m_signature = s;
    return *this;
  }

  Signature & Signature::operator =(const char* s)
  {
    m_signature = s;
    return *this;
  }

  Signature::iterator Signature::begin()
  {
    return iterator(m_signature);
  }

  Signature::const_iterator Signature::begin() const
  {
    return const_iterator(m_signature);
  }

  Signature::iterator Signature::end()
  {
    return iterator();
  }

  Signature::const_iterator Signature::end() const
  {
    return const_iterator();
  }

  bool Signature::is_valid() const
  {
    if ( m_signature.empty() ) return false;
    return dbus_signature_validate( m_signature.c_str(), NULL );
  }

  bool Signature::is_singleton() const
  {
    if ( m_signature.empty() ) return false;
    return dbus_signature_validate_single( m_signature.c_str(), NULL );
  }

}


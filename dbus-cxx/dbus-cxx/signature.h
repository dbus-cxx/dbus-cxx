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
#ifndef DBUSSIGNATURE_H
#define DBUSSIGNATURE_H

#include <ostream>
#include <string>
#include <dbus-cxx/signatureiterator.h>

namespace DBus
{

  /**
   @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
  */
  class Signature
  {
    public:

      typedef SignatureIterator iterator;

      typedef const SignatureIterator const_iterator;

      typedef std::string::size_type size_type;

      static const size_type npos;

      Signature();

      Signature( const std::string& s, size_type pos = 0, size_type n = npos );

      Signature( const char* );

      Signature( const char* s, size_type n );

      Signature( size_type n, char c );

      template<class InputIterator>
      Signature( InputIterator first, InputIterator last ): m_signature( first, last ) { }

      ~Signature();

      operator const std::string&() const;

      const std::string& str() const;

      Signature& operator=(const std::string& s);

      Signature& operator=(const char* s);

      bool operator==(const std::string& s) const { return m_signature == s; }

      iterator begin();

      const_iterator begin() const;

      iterator end();

      const_iterator end() const;

      /** True if the signature is a valid DBus signature */
      bool is_valid() const;

      /** True if the signature is a valid DBus signature and contains exactly one complete type */
      bool is_singleton() const;

    protected:

      std::string m_signature;

  };

}

inline
std::ostream& operator<<(std::ostream& sout, const DBus::Signature& sig)
{
  sout << sig.str();
  return sout;
}

#endif

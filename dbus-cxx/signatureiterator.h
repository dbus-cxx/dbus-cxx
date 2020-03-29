/***************************************************************************
 *   Copyright (C) 2009,2010 by Rick L. Vinyard, Jr.                       *
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
#include <dbus-cxx/enums.h>
#include <dbus/dbus.h>
#include <string>

#ifndef DBUSCXX_SIGNATUREITERATOR_H
#define DBUSCXX_SIGNATUREITERATOR_H

namespace DBus
{

  /**
   * Signature iterator allowing type signatures to be parsed
   * 
   * @ingroup core
   *
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class SignatureIterator
  {
    public:

      SignatureIterator();

      SignatureIterator( const std::string& signature );

      SignatureIterator( const SignatureIterator& other );

      /** Invalidates the iterator */
      void invalidate();

      /** True if this is a valid iterator */
      bool is_valid() const;

      /** True if the iterator is valid and initialized, false otherwise */
      operator bool() const;

      /**
       * Moves the iterator to the next field and invalidates it if it moves beyond the last field
       * @return true if the iterator moves forward, false if there is no next field and the iterator
       *         has been invalidated
       */
      bool next();

      /**
       * See if this iterator can advance.
       *
       * @return
       */
      bool has_next() const;

      SignatureIterator& operator ++();

      SignatureIterator operator ++( int );

      SignatureIterator& operator=( const SignatureIterator& other );

      bool operator==( const SignatureIterator& other );

      /** Returns the argument type that the iterator points to */
      DataType type() const;

      /**
       * Returns the element type of the array that the iterator points to
       *
       * If the iterator does not point to an array TYPE_INVALID is returned.
       */
      DataType element_type() const;

      /** True if the element type is a basic type */
      bool is_basic() const;

      /** True if the element type is a fixed type */
      bool is_fixed() const;

      /** True if the iterator points to a container */
      bool is_container() const;

      /** True if the iterator points to an array */
      bool is_array() const;

      /** True if the iterator points to a dictionary */
      bool is_dict() const;

      /**
       * If the iterator points to a container recurses into the container returning a sub-iterator.
       *
       * If the iterator does not point to a container returns an empty (invalid) iterator.
       */
      SignatureIterator recurse();

      /** Returns the current signature of the iterator */
      std::string signature() const;

    private:
      /**
       * Recurse into the current container.
       *
       * @return A tuple which contains:
       * - The substring that contains the container signature
       * - An iterator to the first character in the contained signature
       * - An interator to the last character in the contained signature
       */
      std::tuple<std::string,std::string::iterator,std::string::iterator> recurse_into_container( std::string::iterator current_pos );

    protected:
      bool m_valid;
      std::string m_signature;
      std::string::iterator m_it;
  };

}

#endif

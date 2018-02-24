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
#include <string>
#include <vector>

#ifndef DBUSCXX_PATH_H
#define DBUSCXX_PATH_H

namespace DBus
{

  /**
   @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
  */
  class Path: public std::string
  {
    public:

      typedef std::vector<std::string> Decomposed;

      Path();

      Path( const std::string& s, size_type pos = 0, size_type n = npos );

      Path( const char* );

      Path( const char* s, size_type n );

      Path( size_type n, char c );

      template<class InputIterator>
      Path( InputIterator first, InputIterator last ): std::string( first, last ) { }

      ~Path();

      operator bool() const;

      bool is_valid() const;

      /**
       * Decomposes the string into its component elements
       *
       * If the path is invalid or non-existant an empty vector will be
       * returned.
       *
       * If the path is the root path a vector containing a single empty
       * string will be returned. In this way a valid root path is
       * differentiated from an invalid or empty path.
       */
      Decomposed decomposed() const;

      /**
       * Adds the element string to the path ensuring delimeters are properly
       * taken care of.
       *
       * An element that contains invalid characters will not be added to the
       * path.
       * 
       * @return True if the element was succesfully added, false otherwise.
       */
      bool append_element(const std::string& element);

  };

}

#endif

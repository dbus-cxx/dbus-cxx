/***************************************************************************
 *   Copyright (C) 2007,2008,2009,2010 by Rick L. Vinyard, Jr.             *
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
#include <stdint.h>
#include <typeinfo>
#include <string>

#include <sigc++/sigc++.h>

#include <dbus-cxx/enums.h>
#include <dbus-cxx/path.h>
#include <dbus-cxx/signature.h>
#include <dbus-cxx/filedescriptor.h>

#ifndef DBUSCXX_TYPES_H
#define DBUSCXX_TYPES_H

namespace DBus
{

  inline int typeToDBusType( DataType t ){
      return static_cast<int>( t );
  }

  inline int typeToDBusContainerType( ContainerType t ){
      return static_cast<int>( t );
  }

  inline DataType type( const uint8_t& )            { return DataType::BYTE; }
  inline DataType type( const bool& )               { return DataType::BOOLEAN; }
  inline DataType type( const int16_t& )            { return DataType::INT16; }
  inline DataType type( const uint16_t& )           { return DataType::UINT16; }
  inline DataType type( const int32_t& )            { return DataType::INT32; }
  inline DataType type( const uint32_t& )           { return DataType::UINT32; }
  inline DataType type( const int64_t& )            { return DataType::INT64; }
  inline DataType type( const uint64_t& )           { return DataType::UINT64; }
  inline DataType type( const double& )             { return DataType::DOUBLE; }
  inline DataType type( const std::string& ) { return DataType::STRING; }
  inline DataType type( const char* )        { return DataType::STRING; }
  inline DataType type( const Path& )               { return DataType::OBJECT_PATH; }
  inline DataType type( const Signature& )          { return DataType::SIGNATURE; }
  template <typename... args>
  inline DataType type( const std::variant<args...>& )            { return DataType::VARIANT; }
  inline DataType type( const FileDescriptor& )     { return DataType::UNIX_FD; }
  
  inline DataType type( const char& )               { return DataType::BYTE; }
  inline DataType type( const int8_t& )             { return DataType::BYTE; }

#if DBUS_CXX_SIZEOF_LONG_INT == 4
  inline DataType type( const long int& )            { return DataType::INT32; }
  inline DataType type( const long unsigned int& )   { return DataType::UINT32; }
#endif
  
  inline DataType type( const float& )               { return DataType::DOUBLE; }

  template <typename T> 
  inline DataType type(const std::vector<T>&) { return DataType::ARRAY; }

  template <typename ...T>
  inline DataType type(const std::tuple<T...>&) { return DataType::STRUCT; }

  inline
  DataType checked_type_cast(int n)
  {
    return (DataType)(n);
  }

  /**
   * Contains useful data about the type in order for
   * code generation to happen in an easy manner
   */
  class TypeInfo {
  public:
    /**
     * The type to get info for.
     */
    TypeInfo( DataType d );

    /**
     * Returns true if this type is templated.
     */
    bool isTemplated() const;

    /**
     * Returns the needed includes in order to use this type.
     */
    std::vector<std::string> includeFilesForType() const;

    /**
     * Returns the C++ type of this DataType.
     */
    std::string cppType() const;

  private:
    DataType m_type;
  };

}

#endif

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

  inline int typeToDBusType( Type t ){
      return static_cast<int>( t );
  }

  inline int typeToDBusContainerType( ContainerType t ){
      return static_cast<int>( t );
  }

  inline Type type( const uint8_t& )            { return Type::BYTE; }
  inline Type type( const bool& )               { return Type::BOOLEAN; }
  inline Type type( const int16_t& )            { return Type::INT16; }
  inline Type type( const uint16_t& )           { return Type::UINT16; }
  inline Type type( const int32_t& )            { return Type::INT32; }
  inline Type type( const uint32_t& )           { return Type::UINT32; }
  inline Type type( const int64_t& )            { return Type::INT64; }
  inline Type type( const uint64_t& )           { return Type::UINT64; }
  inline Type type( const double& )             { return Type::DOUBLE; }
  inline Type type( const std::string& ) { return Type::STRING; }
  inline Type type( const char* )        { return Type::STRING; }
  inline Type type( const Path& )               { return Type::OBJECT_PATH; }
  inline Type type( const Signature& )          { return Type::SIGNATURE; }
  template <typename... args>
  inline Type type( const std::variant<args...>& )            { return Type::VARIANT; }
  inline Type type( const FileDescriptor& )     { return Type::UNIX_FD; }
  
  inline Type type( const char& )               { return Type::BYTE; }
  inline Type type( const int8_t& )             { return Type::BYTE; }

#if DBUS_CXX_SIZEOF_LONG_INT == 4
  inline Type type( const long int& )            { return Type::INT32; }
  inline Type type( const long unsigned int& )   { return Type::UINT32; }
#endif
  
  inline Type type( const float& )               { return Type::DOUBLE; }

  template <typename T> 
  inline Type type(const std::vector<T>&) { return Type::ARRAY; }

//   template <typename T> inline Type type(const std::vector<T>&) { return Type::ARRAY; }

//   template <typename T0>
//   inline Type type( const Struct<T0>& ) { return Type::STRUCT; }
// 
//   template <typename T0, typename T1>
//   inline Type type( const Struct<T0,T1>& ) { return Type::STRUCT; }
// 
//   template <typename T0, typename T1, typename T2>
//   inline Type type( const Struct<T0,T1,T2>& ) { return Type::STRUCT; }
// 
//   template <typename T0, typename T1, typename T2, typename T3>
//   inline Type type( const Struct<T0,T1,T2,T3>& ) { return Type::STRUCT; }
// 
//   template <typename T0, typename T1, typename T2, typename T3, typename T4>
//   inline Type type( const Struct<T0,T1,T2,T3,T4>& ) { return Type::STRUCT; }
// 
//   template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
//   inline Type type( const Struct<T0,T1,T2,T3,T4,T5>& ) { return Type::STRUCT; }
// 
//   template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
//   inline Type type( const Struct<T0,T1,T2,T3,T4,T5,T6>& ) { return Type::STRUCT; }

  inline std::string type_string( const uint8_t& )     { return "byte"; }
  inline std::string type_string( const int8_t& )      { return "byte"; }
  inline std::string type_string( const bool& )        { return "bool"; }
  inline std::string type_string( const int16_t& )     { return "int16_t"; }
  inline std::string type_string( const uint16_t& )    { return "uint16_t"; }
  inline std::string type_string( const int32_t& )     { return "int32_t"; }
  inline std::string type_string( const uint32_t& )    { return "uint32_t"; }
  inline std::string type_string( const int64_t& )     { return "int64_t"; }
  inline std::string type_string( const uint64_t& )    { return "uint64_t"; }
  inline std::string type_string( const double& )      { return "double"; }
  inline std::string type_string( const float& )       { return "double"; }
  inline std::string type_string( const std::string& ) { return "std::string"; }
  inline std::string type_string( const Path& )        { return "Path"; }
  inline std::string type_string( const Signature& )   { return "Signature"; }
template <class T>
  inline std::string type_string( const std::variant<T>& )     { return "Variant"; }
template <class T>
  inline std::string type_string( const std::vector<T>& ) { return "Array"; }
  inline std::string type_string( const FileDescriptor& ) { return "FileDescriptor"; }
//  template <typename T> inline std::string type_string()   { return 1; /* This is invalid; you must use one of the specializations only */}
/*  template<> inline std::string type_string<uint8_t>()     { return "byte"; }
  template<> inline std::string type_string<int8_t>()      { return "byte"; }
  template<> inline std::string type_string<bool>()        { return "boolean"; }
  template<> inline std::string type_string<int16_t>()     { return "int16_t"; }
  template<> inline std::string type_string<uint16_t>()    { return "uint16_t"; }
  template<> inline std::string type_string<int32_t>()     { return "int32_t"; }
  template<> inline std::string type_string<uint32_t>()    { return "uint32_t"; }
  template<> inline std::string type_string<int64_t>()     { return "int64_t"; }
  template<> inline std::string type_string<uint64_t>()    { return "uint64_t"; }
  template<> inline std::string type_string<double>()      { return "double"; }
  template<> inline std::string type_string<float>()       { return "double"; }
  template<> inline std::string type_string<std::string>() { return "std::string"; }
  template<> inline std::string type_string<Path>()        { return "Path"; }
  template<> inline std::string type_string<Signature>()   { return "Signature"; }
  template<> inline std::string type_string<Variant<class T>>()     { return "Variant"; }
*/
  inline std::string type_string_from_code( DBus::Type t )
  {
    switch ( t )
    {
      case Type::BYTE:
        return "uint8_t";
      case Type::BOOLEAN:
        return "bool";
      case Type::INT16:
        return "int16_t";
      case Type::UINT16:
        return "uint16_t";
      case Type::INT32:
        return "int32_t";
      case Type::UINT32:
        return "uint32_t";
      case Type::INT64:
        return "int64_t";
      case Type::UINT64:
        return "uint64_t";
      case Type::DOUBLE:
        return "double";
      case Type::STRING:
        return "std::string";
      case Type::OBJECT_PATH:
        return "DBus::Path";
      case Type::SIGNATURE:
        return "DBus::Signature";
      case Type::ARRAY:
        return "std::vector";
      case Type::VARIANT:
        return "DBus::Variant<>";
      case Type::STRUCT:
        return "Struct";
      case Type::DICT_ENTRY:
        return "std::pair<>";
      case Type::UNIX_FD:
        return "FileDescriptor";
      default:
        break;
    }
    return std::string("Invalid");
  }

  inline std::string include_file_for_type( DBus::Type t ){
    switch ( t )
    {
      case Type::BYTE:
      case Type::INT16:
      case Type::UINT16:
      case Type::INT32:
      case Type::UINT32:
      case Type::INT64:
      case Type::UINT64:
        return "stdint.h";
      case Type::STRING:
        return "string";
      case Type::ARRAY:
        return "vector";
      case Type::DICT_ENTRY:
        return "map";
    }
    return std::string("");
  }

  inline bool type_is_templated( DBus::Type t ){
    switch( t )
    {
        case Type::ARRAY:
          return true;
        case Type::DICT_ENTRY:
          return true;
    }
    return false;
  }

  inline
  Type checked_type_cast(int n)
  {
    return (Type)(n);
  }


}

#endif

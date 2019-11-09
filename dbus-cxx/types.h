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

//   template <typename T> inline DataType type(const std::vector<T>&) { return DataType::ARRAY; }

//   template <typename T0>
//   inline DataType type( const Struct<T0>& ) { return DataType::STRUCT; }
// 
//   template <typename T0, typename T1>
//   inline DataType type( const Struct<T0,T1>& ) { return DataType::STRUCT; }
// 
//   template <typename T0, typename T1, typename T2>
//   inline DataType type( const Struct<T0,T1,T2>& ) { return DataType::STRUCT; }
// 
//   template <typename T0, typename T1, typename T2, typename T3>
//   inline DataType type( const Struct<T0,T1,T2,T3>& ) { return DataType::STRUCT; }
// 
//   template <typename T0, typename T1, typename T2, typename T3, typename T4>
//   inline DataType type( const Struct<T0,T1,T2,T3,T4>& ) { return DataType::STRUCT; }
// 
//   template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
//   inline DataType type( const Struct<T0,T1,T2,T3,T4,T5>& ) { return DataType::STRUCT; }
// 
//   template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
//   inline DataType type( const Struct<T0,T1,T2,T3,T4,T5,T6>& ) { return DataType::STRUCT; }

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
  inline std::string type_string_from_code( DBus::DataType t )
  {
    switch ( t )
    {
      case DataType::BYTE:
        return "uint8_t";
      case DataType::BOOLEAN:
        return "bool";
      case DataType::INT16:
        return "int16_t";
      case DataType::UINT16:
        return "uint16_t";
      case DataType::INT32:
        return "int32_t";
      case DataType::UINT32:
        return "uint32_t";
      case DataType::INT64:
        return "int64_t";
      case DataType::UINT64:
        return "uint64_t";
      case DataType::DOUBLE:
        return "double";
      case DataType::STRING:
        return "std::string";
      case DataType::OBJECT_PATH:
        return "DBus::Path";
      case DataType::SIGNATURE:
        return "DBus::Signature";
      case DataType::ARRAY:
        return "std::vector";
      case DataType::VARIANT:
        return "DBus::Variant<>";
      case DataType::STRUCT:
        return "Struct";
      case DataType::DICT_ENTRY:
        return "std::pair<>";
      case DataType::UNIX_FD:
        return "FileDescriptor";
      default:
        break;
    }
    return std::string("Invalid");
  }

  inline std::string include_file_for_type( DBus::DataType t ){
    switch ( t )
    {
      case DataType::BYTE:
      case DataType::INT16:
      case DataType::UINT16:
      case DataType::INT32:
      case DataType::UINT32:
      case DataType::INT64:
      case DataType::UINT64:
        return "stdint.h";
      case DataType::STRING:
        return "string";
      case DataType::ARRAY:
        return "vector";
      case DataType::DICT_ENTRY:
        return "map";
    }
    return std::string("");
  }

  inline bool type_is_templated( DBus::DataType t ){
    switch( t )
    {
        case DataType::ARRAY:
          return true;
        case DataType::DICT_ENTRY:
          return true;
    }
    return false;
  }

  inline
  DataType checked_type_cast(int n)
  {
    return (DataType)(n);
  }


}

#endif

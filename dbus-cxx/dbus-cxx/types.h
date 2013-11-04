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

#ifndef DBUSCXX_TYPES_H
#define DBUSCXX_TYPES_H

namespace DBus
{

  typedef sigc::nil nil;

  inline Type type( uint8_t )            { return TYPE_BYTE; }
  inline Type type( bool )               { return TYPE_BOOLEAN; }
  inline Type type( int16_t )            { return TYPE_INT16; }
  inline Type type( uint16_t )           { return TYPE_UINT16; }
  inline Type type( int32_t )            { return TYPE_INT32; }
  inline Type type( uint32_t )           { return TYPE_UINT32; }
  inline Type type( int64_t )            { return TYPE_INT64; }
  inline Type type( uint64_t )           { return TYPE_UINT64; }
  inline Type type( double )             { return TYPE_DOUBLE; }
  inline Type type( const std::string& ) { return TYPE_STRING; }
  inline Type type( const char* )        { return TYPE_STRING; }
  inline Type type( Path )               { return TYPE_OBJECT_PATH; }
  inline Type type( Signature )          { return TYPE_SIGNATURE; }
template <class T>
  inline Type type( Variant<T> )            { return TYPE_VARIANT; }
  
  inline Type type( char )               { return TYPE_BYTE; }
  inline Type type( int8_t )             { return TYPE_BYTE; }

#if DBUS_CXX_SIZEOF_LONG_INT == 4
  inline Type type( long int )            { return TYPE_INT32; }
  inline Type type( long unsigned int )   { return TYPE_UINT32; }
#endif
  
  inline Type type( float )               { return TYPE_DOUBLE; }

  template <typename T> 
  inline Type type(const std::vector<T>&) { return TYPE_ARRAY; }

//   template <typename T> inline Type type(const std::vector<T>&) { return TYPE_ARRAY; }

//   template <typename T0>
//   inline Type type( const Struct<T0>& ) { return TYPE_STRUCT; }
// 
//   template <typename T0, typename T1>
//   inline Type type( const Struct<T0,T1>& ) { return TYPE_STRUCT; }
// 
//   template <typename T0, typename T1, typename T2>
//   inline Type type( const Struct<T0,T1,T2>& ) { return TYPE_STRUCT; }
// 
//   template <typename T0, typename T1, typename T2, typename T3>
//   inline Type type( const Struct<T0,T1,T2,T3>& ) { return TYPE_STRUCT; }
// 
//   template <typename T0, typename T1, typename T2, typename T3, typename T4>
//   inline Type type( const Struct<T0,T1,T2,T3,T4>& ) { return TYPE_STRUCT; }
// 
//   template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
//   inline Type type( const Struct<T0,T1,T2,T3,T4,T5>& ) { return TYPE_STRUCT; }
// 
//   template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
//   inline Type type( const Struct<T0,T1,T2,T3,T4,T5,T6>& ) { return TYPE_STRUCT; }

  inline std::string type_string( uint8_t& )     { return "byte"; }
  inline std::string type_string( int8_t& )      { return "byte"; }
  inline std::string type_string( bool& )        { return "boolean"; }
  inline std::string type_string( int16_t& )     { return "int16_t"; }
  inline std::string type_string( uint16_t& )    { return "uint16_t"; }
  inline std::string type_string( int32_t& )     { return "int32_t"; }
  inline std::string type_string( uint32_t& )    { return "uint32_t"; }
  inline std::string type_string( int64_t& )     { return "int64_t"; }
  inline std::string type_string( uint64_t& )    { return "uint64_t"; }
  inline std::string type_string( double& )      { return "double"; }
  inline std::string type_string( float& )       { return "double"; }
  inline std::string type_string( std::string& ) { return "std::string"; }
  inline std::string type_string( Path& )        { return "Path"; }
  inline std::string type_string( Signature& )   { return "Signature"; }
template <class T>
  inline std::string type_string( Variant<T>& )     { return "Variant"; }
template <class T>
  inline std::string type_string( std::vector<T>& ) { return "Array"; }
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
      case TYPE_BYTE:
        return "uint8_t";
      case TYPE_BOOLEAN:
        return "boolean";
      case TYPE_INT16:
        return "int16_t";
      case TYPE_UINT16:
        return "uint16_t";
      case TYPE_INT32:
        return "int32_t";
      case TYPE_UINT32:
        return "uint32_t";
      case TYPE_INT64:
        return "int64_t";
      case TYPE_UINT64:
        return "uint64_t";
      case TYPE_DOUBLE:
        return "double";
      case TYPE_STRING:
        return "std::string";
      case TYPE_OBJECT_PATH:
        return "DBus::Path";
      case TYPE_SIGNATURE:
        return "DBus::Signature";
      case TYPE_ARRAY:
        return "std::vector";
      case TYPE_VARIANT:
        return "DBus::Variant";
      case TYPE_STRUCT:
        return "Struct";
      case TYPE_DICT_ENTRY:
        return "Dict Entry";
      default:
        break;
    }
    return std::string("Invalid");
  }

  inline
  Type checked_type_cast(int n)
  {
    return (Type)(n);
  }


}

#endif

/***************************************************************************
 *   Copyright (C) 2007,2008,2009 by Rick L. Vinyard, Jr.                  *
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

#ifndef DBUSUTILITY_H
#define DBUSUTILITY_H

#include <typeinfo>
#include <string>

#include <sigc++/sigc++.h>

#include <dbus-cxx/dbus-cxx-config.h>
#include <dbus-cxx/enums.h>
#include <dbus-cxx/pointer.h>
#include <dbus-cxx/path.h>
#include <dbus-cxx/signature.h>

#define DBUS_CXX_PARAM_LIMIT 7

#ifdef DBUS_CXX_DEBUG_ENABLED
  #include <iostream>

 #define DEBUG_OUT(x,y) if (DEBUG) std::cout << x << " " << pthread_self() << ": " << y << std::endl
 #define DBUS_CXX_DEBUG(x) std::cout << pthread_self() << ": " << x << std::endl
//   #define DEBUG_OUT(x,y) ;
//   #define DBUS_CXX_DEBUG(x) ;
#else
  #define DEBUG_OUT(x,y) ;
  #define DBUS_CXX_DEBUG(x) ;
#endif

#define DBUS_CXX_INTROSPECTABLE_INTERFACE "org.freedesktop.DBus.Introspectable"

namespace DBus
{

  typedef sigc::nil nil;

  class Connection;

  void init(bool threadsafe=true);
  
  bool initialized();

  std::string introspect( DBusCxxPointer<Connection> conn, const std::string& destination, const std::string& path );

  inline std::string signature( uint8_t )     { return DBUS_TYPE_BYTE_AS_STRING; }
  inline std::string signature( bool )        { return DBUS_TYPE_BOOLEAN_AS_STRING; }
  inline std::string signature( int16_t )     { return DBUS_TYPE_INT16_AS_STRING; }
  inline std::string signature( uint16_t )    { return DBUS_TYPE_UINT16_AS_STRING; }
  inline std::string signature( int32_t )     { return DBUS_TYPE_INT32_AS_STRING; }
  inline std::string signature( uint32_t )    { return DBUS_TYPE_UINT32_AS_STRING; }
  inline std::string signature( int64_t )     { return DBUS_TYPE_INT64_AS_STRING; }
  inline std::string signature( uint64_t )    { return DBUS_TYPE_UINT64_AS_STRING;      }
  inline std::string signature( double )      { return DBUS_TYPE_DOUBLE_AS_STRING;      }
  inline std::string signature( std::string ) { return DBUS_TYPE_STRING_AS_STRING;      }
  inline std::string signature( Signature )   { return DBUS_TYPE_SIGNATURE_AS_STRING;   }
  inline std::string signature( Path )        { return DBUS_TYPE_OBJECT_PATH_AS_STRING; }

  inline std::string signature( char )        { return DBUS_TYPE_BYTE_AS_STRING;        }
  inline std::string signature( int8_t )      { return DBUS_TYPE_BYTE_AS_STRING;        }
  
#if DBUS_CXX_SIZEOF_LONG_INT == 4
  inline std::string signature( long int )          { return DBUS_TYPE_INT32_AS_STRING;       }
  inline std::string signature( long unsigned int ) { return DBUS_TYPE_UINT32_AS_STRING;      }
#endif
  
  inline std::string signature( float )         { return DBUS_TYPE_DOUBLE_AS_STRING; }

  template <typename T> inline std::string signature()   { return 1; /* This is invalid; you must use one of the specializations only */}
  template<> inline std::string signature<uint8_t>()     { return DBUS_TYPE_BYTE_AS_STRING;        }
  template<> inline std::string signature<bool>()        { return DBUS_TYPE_BOOLEAN_AS_STRING;     }
  template<> inline std::string signature<int16_t>()     { return DBUS_TYPE_INT16_AS_STRING;       }
  template<> inline std::string signature<uint16_t>()    { return DBUS_TYPE_UINT16_AS_STRING;      }
  template<> inline std::string signature<int32_t>()     { return DBUS_TYPE_INT32_AS_STRING;       }
  template<> inline std::string signature<uint32_t>()    { return DBUS_TYPE_UINT32_AS_STRING;      }
  template<> inline std::string signature<int64_t>()     { return DBUS_TYPE_INT64_AS_STRING;       }
  template<> inline std::string signature<uint64_t>()    { return DBUS_TYPE_UINT64_AS_STRING;      }
  template<> inline std::string signature<double>()      { return DBUS_TYPE_DOUBLE_AS_STRING;      }
  template<> inline std::string signature<std::string>() { return DBUS_TYPE_STRING_AS_STRING;      }
  template<> inline std::string signature<Signature>()   { return DBUS_TYPE_SIGNATURE_AS_STRING;   }
  template<> inline std::string signature<Path>()        { return DBUS_TYPE_OBJECT_PATH_AS_STRING; }
  
  template<> inline std::string signature<char>()        { return DBUS_TYPE_BYTE_AS_STRING;        }
  template<> inline std::string signature<int8_t>()      { return DBUS_TYPE_BYTE_AS_STRING;        }

#if DBUS_CXX_SIZEOF_LONG_INT == 4
  template<> inline std::string signature<long int>()          { return DBUS_TYPE_INT32_AS_STRING;       }
  template<> inline std::string signature<long unsigned int>() { return DBUS_TYPE_UINT32_AS_STRING;       }
#endif
  
  template<> inline std::string signature<float>()         { return DBUS_TYPE_DOUBLE_AS_STRING; }


//   inline std::string signature( Variant )     { return DBUS_TYPE_VARIANT_AS_STRING; }
//   template <typename T> inline std::string signature( const std::vector<T>& ) { T t; return DBUS_TYPE_ARRAY_AS_STRING + signature( t ); }

//   template <typename Key,typename Data> inline std::string signature( const std::vector<std::pair<Key,Data> >& )
//   {
//     Key k; Data d;
//     std::string sig;
//     sig = DBUS_TYPE_ARRAY_AS_STRING;
//     sig += DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING + signature(k) + signature(d) + DBUS_DICT_ENTRY_END_CHAR_AS_STRING;
//     return sig;
//   }

//   template <typename T1>
//   inline std::string signature( const Struct<T1>& )
//   {
//     T1 t1;
//     return DBUS_STRUCT_BEGIN_CHAR_AS_STRING + signature( t1 ) + DBUS_STRUCT_END_CHAR_AS_STRING;
//   }
// 
//   template <typename T1, typename T2>
//   inline std::string signature( const Struct<T1,T2>& )
//   {
//     T1 t1; T2 t2;
//     return DBUS_STRUCT_BEGIN_CHAR_AS_STRING + signature( t1 ) + signature( t2 ) + DBUS_STRUCT_END_CHAR_AS_STRING;
//   }
// 
//   template <typename T1, typename T2, typename T3>
//   inline std::string signature( const Struct<T1,T2,T3>& )
//   {
//     T1 t1; T2 t2; T3 t3;
//     return DBUS_STRUCT_BEGIN_CHAR_AS_STRING + signature( t1 ) + signature( t2 ) + signature( t3 ) + DBUS_STRUCT_END_CHAR_AS_STRING;
//   }
// 
//   template <typename T1, typename T2, typename T3, typename T4>
//   inline std::string signature( const Struct<T1,T2,T3,T4>& )
//   {
//     T1 t1; T2 t2; T3 t3; T4 t4;
//     return DBUS_STRUCT_BEGIN_CHAR_AS_STRING + signature( t1 ) + signature( t2 ) + signature( t3 ) + signature( t4 ) + DBUS_STRUCT_END_CHAR_AS_STRING;
//   }
// 
//   template <typename T1, typename T2, typename T3, typename T4, typename T5>
//   inline std::string signature( const Struct<T1,T2,T3,T4,T5>& )
//   {
//     T1 t1; T2 t2; T3 t3; T4 t4; T5 t5;
//     return DBUS_STRUCT_BEGIN_CHAR_AS_STRING + signature( t1 ) + signature( t2 ) + signature( t3 ) + signature( t4 ) + signature( t5 ) + DBUS_STRUCT_END_CHAR_AS_STRING;
//   }
// 
//   template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
//   inline std::string signature( const Struct<T1,T2,T3,T4,T5,T6>& )
//   {
//     T1 t1; T2 t2; T3 t3; T4 t4; T5 t5; T6 t6;
//     return DBUS_STRUCT_BEGIN_CHAR_AS_STRING + signature( t1 ) + signature( t2 ) + signature( t3 ) + signature( t4 ) + signature( t5 ) + signature( t6 ) + DBUS_STRUCT_END_CHAR_AS_STRING;
//   }
// 
//   template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
//   inline std::string signature( const Struct<T1,T2,T3,T4,T5,T6,T7>& )
//   {
//     T1 t1; T2 t2; T3 t3; T4 t4; T5 t5; T6 t6; T7 t7;
//     return DBUS_STRUCT_BEGIN_CHAR_AS_STRING + signature( t1 ) + signature( t2 ) + signature( t3 ) + signature( t4 ) + signature( t5 ) + signature( t6 ) + signature( t7 ) + DBUS_STRUCT_END_CHAR_AS_STRING;
//   }

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
//   inline Type type( Variant )            { return TYPE_VARIANT; }
  
  inline Type type( char )               { return TYPE_BYTE; }
  inline Type type( int8_t )             { return TYPE_BYTE; }

#if DBUS_CXX_SIZEOF_LONG_INT == 4
  inline Type type( long int )            { return TYPE_INT32; }
  inline Type type( long unsigned int )   { return TYPE_UINT32; }
#endif
  
  inline Type type( float )               { return TYPE_DOUBLE; }


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

  inline std::string type_string( uint8_t )     { return "byte"; }
  inline std::string type_string( bool )        { return "boolean"; }
  inline std::string type_string( int16_t )     { return "int16_t"; }
  inline std::string type_string( uint16_t )    { return "uint16_t"; }
  inline std::string type_string( int32_t )     { return "int32_t"; }
  inline std::string type_string( uint32_t )    { return "uint32_t"; }
  inline std::string type_string( int64_t )     { return "int64_t"; }
  inline std::string type_string( uint64_t )    { return "uint64_t"; }
  inline std::string type_string( double )      { return "double"; }
  inline std::string type_string( std::string ) { return "std::string"; }
  inline std::string type_string( Path )        { return "Path"; }
  inline std::string type_string( Signature )   { return "Signature"; }
//   inline std::string type_string( Variant )     { return "Variant"; }

  inline std::string type_string_from_code( DBus::Type t )
  {
    switch ( t )
    {
      case TYPE_BYTE:
        return "byte";
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
        return "string";
      case TYPE_OBJECT_PATH:
        return "Path";
      case TYPE_SIGNATURE:
        return "Signature";
      case TYPE_ARRAY:
        return "Array";
      case TYPE_VARIANT:
        return "Variant";
      case TYPE_STRUCT:
        return "Struct";
      case TYPE_DICT_ENTRY:
        return "Dict Entry";
      default:
        break;
    }
    return std::string("Invalid");
  }

  Type checked_type_cast( int n );

}

#endif

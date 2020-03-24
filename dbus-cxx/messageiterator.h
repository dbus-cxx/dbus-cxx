/***************************************************************************
 *   Copyright (C) 2007,2009,2010 by Rick L. Vinyard, Jr.                  *
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
#include <dbus-cxx/demangle.h>
#include <dbus-cxx/types.h>
#include <dbus-cxx/variant.h>
#include <dbus/dbus.h>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>
#include "enums.h"
#include "error.h"

#ifndef DBUSCXX_MESSAGEITERATOR_H
#define DBUSCXX_MESSAGEITERATOR_H

#if defined(__GNUC__) && (__GNUC__ <= 8)
/*
 * There seems to be a bug with G++ on versions <= 8 where the operator Variant()
 * is trying to call the constructor instead of actually calling the operator on
 * the iterator.  This macro works around this bug
 */
#define DBUSCXX_MESSAGEITERATOR_OPERATOR_VARIANT(iter) iter.operator DBus::Variant()
#else
#define DBUSCXX_MESSAGEITERATOR_OPERATOR_VARIANT(x) (DBus::Variant)iter
#endif

namespace DBus
{

  class FileDescriptor;
  class Message;

  /**
   * Extraction iterator allowing values to be retrieved from a message
   * 
   * @ingroup message
   *
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class MessageIterator
  {
    public:

      MessageIterator();

      MessageIterator( const Message& message );
      
      MessageIterator( std::shared_ptr<Message> message );

      /**
       * Returns a pointer to the message associated with this iterator or NULL
       * if no message is associated.
       */
      const Message* message() const;

      /** Returns a pointer to the underlying DBusMessageIter object */
      DBusMessageIter* cobj();

      /** Initializes the iterator for the specified message */
      bool init( const Message& message );

      /** Invalidates the iterator */
      void invalidate();

      /** True if this is a valid iterator */
      bool is_valid() const;

      /** True if there are any more fields left to iterate over */
      bool has_next() const;

      /**
       * Moves the iterator to the next field and invalidates it if it moves beyond the last field
       * @return true if the iterator moves forward, false if there is no next field and the iterator
       *         has been invalidated
       */
      bool next();

      MessageIterator& operator ++();

      MessageIterator operator ++( int );

      bool operator==( const MessageIterator& other );

      /** Returns the argument type that the iterator points to */
      DataType arg_type() const;

      /**
       * Returns the element type of the array that the iterator points to
       *
       * If the iterator does not point to an array TYPE_INVALID is returned.
       */
      DataType element_type() const;

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
      MessageIterator recurse();

      /** Returns the current signature of the iterator */
      std::string signature() const;

      /**
       * TODO
       *
       * Add support for:
       * - dbus_message_iter_get_fixed_array
       * - dbus_message_iter_get_array_len
       */

      operator bool();
      operator uint8_t();
      operator uint16_t();
      operator int16_t();
      operator uint32_t();
      operator int32_t();
      operator uint64_t();
      operator int64_t();
      operator double();
      operator const char*();
      
      operator char();
      operator int8_t();
      operator float();
      #if DBUS_CXX_SIZEOF_LONG_INT == 4
        operator long int();
        operator unsigned long int();
      #endif
      operator std::shared_ptr<FileDescriptor>();
      operator Variant();
        
      template <typename T>
      operator std::vector<T>() {
        if ( not this->is_array() )
          throw ErrorInvalidTypecast( "MessageIterator: Extracting non array into std::vector" );

        return get_array<T>();
      }

      template <typename Key, typename Data>
      operator std::map<Key,Data>() {
        if ( not this->is_dict() )
          throw ErrorInvalidTypecast( "MessageIterator: Extracting non dict into std::map" );

        return get_dict<Key,Data>();
      }

      template <typename... T>
      operator std::tuple<T...>() {
        std::tuple<T...> tup;
        get_struct<T...>(tup);
        return tup;
      }


      bool        get_bool();
      uint8_t     get_uint8();
      uint16_t    get_uint16();
      int16_t     get_int16();
      uint32_t    get_uint32();
      int32_t     get_int32();
      uint64_t    get_uint64();
      int64_t     get_int64();
      double      get_double();
      const char* get_string();
      std::shared_ptr<FileDescriptor> get_filedescriptor();
      Variant get_variant();

      /**
       * get_array for simple types - arithmetic types where the array is fixed and thus
       * we can use dbus_message_iter_get_fixed_array.
       */
      template <typename T,
          typename std::enable_if<std::is_arithmetic<T>::value>::type>
      void get_array( std::vector<T>& array ) {
        if ( not this->is_fixed() ) /* This should never happen */
          throw ErrorInvalidTypecast( "MessageIterator: Extracting non fixed array into std::vector" );
        
        T type;
        if ( this->element_type() != DBus::type( type ) ) {
          TypeInfo t( DBus::type( type ) );
          std::string s = "MessageIterator: Extracting DBus array type ";
          s += t.cppType();
          s += " into C++ vector with type ";
          s += demangle<T>();
	  throw ErrorInvalidTypecast( s.c_str() );
        }
        
        int elements;
        T* values;
        
        MessageIterator subiter = this->recurse();

        array.clear();
        
        // Get the underlying array
        dbus_message_iter_get_fixed_array( subiter.cobj(), &values, &elements );
        
        // Iteratively add the elements to the array
        for ( int i=0; i < elements; i++ )
          array.push_back( values[i] );
      }
      
      /**
       * get_array for complex types - types that you can't use dbus_message_iter_get_fixed_array for.
       */
      template <typename T>
      void get_array(std::vector<T> &array) {
        if ( not this->is_array() ) /* Should never happen */
          throw ErrorInvalidTypecast( "MessageIterator: Extracting non array into std::vector" );

        array.clear();

        MessageIterator subiter = this->recurse();
        while( subiter.is_valid() )
        {
          //NOTE: we don't need to do subiter.next() here, because
          //operator>> does that for us
          T val;
          subiter >> val;
          array.push_back( val );
        }
     }

      template <typename... T>
      void get_struct(std::tuple<T...> &tup) {
        MessageIterator subiter = this->recurse();
        std::apply( [subiter](auto&& ...arg) mutable {
               (subiter >> ... >> arg);
              },
        tup );
     }

     template <typename Key, typename Data>
     void get_dict( std::map<Key,Data>& dict ){
	Key val_key;
        Data val_data;

        MessageIterator subiter = this->recurse();
        while( subiter.is_valid() ) {
          MessageIterator subSubiter = subiter.recurse();
          while( subSubiter.is_valid() )
          {
            subSubiter >> val_key;
            subSubiter >> val_data;
            dict[ val_key ] = val_data;
            subSubiter.next();
          }
         subiter.next();
       }
     }

     template <typename Key, typename Data>
     std::map<Key,Data> get_dict(){
       std::map<Key,Data> newMap;
       get_dict( newMap );
       return newMap;
     }

      template <typename Key, typename Data>
      MessageIterator& operator>>( std::map<Key,Data>& m )
      {
        if ( not this->is_dict() )
          throw ErrorInvalidTypecast( "MessageIterator: Extracting non dict into std::map" );
        try{
          get_dict<Key,Data>( m );
          this->next();
          return *this;
	}catch(std::shared_ptr<DBus::ErrorInvalidTypecast> e){
          //TODO make sure we don't need this? it doesn't make much sense
	  throw (ErrorInvalidTypecast)*e;
	}
      }
       
      template <typename... T>
      MessageIterator& operator>>( std::tuple<T...>& v )
      {
	try{
          this->get_struct<T...>(v);
          this->next();
          return *this;
	}catch(std::shared_ptr<DBus::ErrorInvalidTypecast> e){
	  throw (ErrorInvalidTypecast)*e;
	}
      }

      template <typename T>
      MessageIterator& operator>>( std::vector<T>& v )
      {
        if ( not this->is_array() )
          throw ErrorInvalidTypecast( "MessageIterator: Extracting non array into std::vector" );
	try{
          this->get_array<T>(v);
          this->next();
          return *this;
	}catch(std::shared_ptr<DBus::ErrorInvalidTypecast> e){
	  throw (ErrorInvalidTypecast)*e;
	}
      }

      MessageIterator& operator>>( Variant& v )
      {
	try{
          v = this->get_variant();
          this->next();
          return *this;
        }catch(std::shared_ptr<DBus::ErrorInvalidTypecast> e){
          throw (ErrorInvalidTypecast)*e;
        }
      }

      template <typename T>
      MessageIterator& operator>>( T& v )
      {
	try{
          v = (T)(*this);
          this->next();
          return *this;
        }catch(std::shared_ptr<DBus::ErrorInvalidTypecast> e){
          throw (ErrorInvalidTypecast)*e;
        }
      }

      template <typename T>
      void value( T& temp ) {
        if ( this->arg_type() != DBus::type( temp ) ) {
          TypeInfo t( DBus::type( temp ) );
          std::string s = "MessageIterator: Extracting DBus type ";
          s += t.cppType();
          s += " into C++ type ";
          s += demangle<T>();
          throw ErrorInvalidTypecast( s.c_str() );
        }
        dbus_message_iter_get_basic( &m_cobj, &temp );
      }

    protected:
      const Message* m_message;
      DBusMessageIter m_cobj;

  };
}

#endif

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
#include <dbus/dbus.h>

#include <dbus-cxx/types.h>

#ifndef DBUSCXX_MESSAGEITERATOR_H
#define DBUSCXX_MESSAGEITERATOR_H

namespace DBus
{

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
      
      MessageIterator( DBusCxxPointer<Message> message );

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
      Type arg_type() const;

      /**
       * Returns the element type of the array that the iterator points to
       *
       * If the iterator does not point to an array TYPE_INVALID is returned.
       */
      Type element_type() const;

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

//       void value( std::vector<std::string>& temp );

//       void value( std::string& temp );

//       void value( Variant& temp );

//       template <typename T0>
//       void value( Struct<T0>& temp ) {
//         if ( this->element_type() != TYPE_STRUCT ) throw ErrorInvalidTypecast();
//         MessageIterator subiter = this->recurse();
//         subiter.value( boost::get<0>( temp ) );
//       }

//       template <typename T0, typename T1>
//       void value( Struct<T0,T1>& temp ) {
//         if ( this->element_type() != TYPE_STRUCT ) throw ErrorInvalidTypecast();
//         MessageIterator subiter = this->recurse();
//         subiter.value( boost::get<0>( temp ) );
//         subiter.value( boost::get<1>( temp ) );
//       }

//       template <typename T0, typename T1, typename T2>
//       void value( Struct<T0,T1,T2>& temp ) {
//         if ( this->element_type() != TYPE_STRUCT ) throw ErrorInvalidTypecast();
//         MessageIterator subiter = this->recurse();
//         subiter.value( boost::get<0>( temp ) );
//         subiter.value( boost::get<1>( temp ) );
//         subiter.value( boost::get<2>( temp ) );
//       }
// 
//       template <typename T0, typename T1, typename T2, typename T3>
//       void value( Struct<T0,T1,T2,T3>& temp ) {
//         if ( this->element_type() != TYPE_STRUCT ) throw ErrorInvalidTypecast();
//         MessageIterator subiter = this->recurse();
//         subiter.value( boost::get<0>( temp ) );
//         subiter.value( boost::get<1>( temp ) );
//         subiter.value( boost::get<2>( temp ) );
//         subiter.value( boost::get<3>( temp ) );
//       }
// 
//       template <typename T0, typename T1, typename T2, typename T3, typename T4>
//       void value( Struct<T0,T1,T2,T3,T4>& temp ) {
//         if ( this->element_type() != TYPE_STRUCT ) throw ErrorInvalidTypecast();
//         MessageIterator subiter = this->recurse();
//         subiter.value( boost::get<0>( temp ) );
//         subiter.value( boost::get<1>( temp ) );
//         subiter.value( boost::get<2>( temp ) );
//         subiter.value( boost::get<3>( temp ) );
//         subiter.value( boost::get<4>( temp ) );
//       }
// 
//       template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
//       void value( Struct<T0,T1,T2,T3,T4,T5>& temp ) {
//         if ( this->element_type() != TYPE_STRUCT ) throw ErrorInvalidTypecast();
//         MessageIterator subiter = this->recurse();
//         subiter.value( boost::get<0>( temp ) );
//         subiter.value( boost::get<1>( temp ) );
//         subiter.value( boost::get<2>( temp ) );
//         subiter.value( boost::get<3>( temp ) );
//         subiter.value( boost::get<4>( temp ) );
//         subiter.value( boost::get<5>( temp ) );
//       }
// 
//       template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
//       void value( Struct<T0,T1,T2,T3,T4,T5,T6>& temp ) {
//         if ( this->element_type() != TYPE_STRUCT ) throw ErrorInvalidTypecast();
//         MessageIterator subiter = this->recurse();
//         subiter.value( boost::get<0>( temp ) );
//         subiter.value( boost::get<1>( temp ) );
//         subiter.value( boost::get<2>( temp ) );
//         subiter.value( boost::get<3>( temp ) );
//         subiter.value( boost::get<4>( temp ) );
//         subiter.value( boost::get<5>( temp ) );
//         subiter.value( boost::get<6>( temp ) );
//       }

//       template <typename Key, typename Data>
//       void value( std::vector<std::pair<Key,Data> >& temp ) {
//         if ( this->element_type() != TYPE_ARRAY )
//           throw ErrorInvalidTypecast( "Extracting non-array type into dictionary" );
// 
//         temp.clear();
//         MessageIterator subiter = this->recurse();
//         MessageIterator subsubiter;
//         Key k;
//         Data d;
// 
//         while ( subiter.has_next() ) {
//           if ( subiter.element_type() != TYPE_DICT_ENTRY )
//             throw ErrorInvalidTypecast( "Extracting non-dictionary-entry type into dictionary entry" );
//           subsubiter = subiter.recurse();
//           subsubiter.value( k );
//           ++subsubiter;
//           subsubiter.value( d );
//           temp.push_back( std::make_pair( k,d ) );
//           ++subiter;
//         }
//       }



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
        
      template <typename T>
      operator std::vector<T>() {
        return get_array<T>();
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

      template <typename T>
      std::vector<T> get_array() {
        
        if ( not this->is_array() )
          throw ErrorInvalidTypecast::create( "MessageIterator: Extracting non array into std::vector" );
        
        if ( not this->is_fixed() )
          throw ErrorInvalidTypecast::create( "MessageIterator: Extracting non fixed array into std::vector" );
        
        if ( this->element_type() != DBus::type<T>() ) {
          std::string s = "MessageIterator: Extracting DBus array type ";
          s += type_string<T>();
          s += " into C++ vector with RTTI type ";
          s += typeid( T ).name();
	  throw ErrorInvalidTypecast::create( s.c_str() );
        }
        
        std::vector<T> array;
        int elements;
        T* values;
        
        MessageIterator subiter = this->recurse();
        
        // Get the underlying array
        dbus_message_iter_get_fixed_array( subiter.cobj(), &values, &elements );
        
        // Iteratively add the elements to the array
        for ( int i=0; i < elements; i++ )
          array.push_back( values[i] );
        
        return array;
      }
      
      template <typename T>
      void get_array(std::vector<T>& array) {
        
        if ( not this->is_array() )
          throw ErrorInvalidTypecast::create( "MessageIterator: Extracting non array into std::vector" );
        
        if ( not this->is_fixed() )
          throw ErrorInvalidTypecast::create( "MessageIterator: Extracting non fixed array into std::vector" );
        
        if ( this->element_type() != DBus::type<T>() ) {
          std::string s = "MessageIterator: Extracting DBus array type ";
          s += type_string<T>();
          s += " into C++ vector with RTTI type ";
          s += typeid( T ).name();
	  throw ErrorInvalidTypecast::create( s.c_str() );
        }
        
        int elements;
        T* values;
        
        MessageIterator subiter = this->recurse();
        
        // Get the underlying array
        dbus_message_iter_get_fixed_array( subiter.cobj(), &values, &elements );
        
        // Iteratively add the elements to the array
        array.clear();
        for ( int i=0; i < elements; i++ )
          array.push_back( values[i] );
      }

      template <typename T>
      MessageIterator& operator>>( std::vector<T>& v )
      {
	try{
          this->get_array<T>(v);
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
          std::string s = "MessageIterator: Extracting DBus type ";
          s += type_string( temp );
          s += " into C++ RTTI type ";
          s += typeid( T ).name();
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

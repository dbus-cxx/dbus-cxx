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
      operator FileDescriptor::pointer();
        
      template <typename T>
      operator std::vector<T>() {
        if ( not this->is_array() )
          throw ErrorInvalidTypecast::create( "MessageIterator: Extracting non array into std::vector" );

        if ( this->is_fixed() )
          return get_array_simple<T>();
        else
          return get_array_complex<T>();
      }

      template <typename Key, typename Data>
      operator std::map<Key,Data>() {
        if ( not this->is_dict() )
          throw ErrorInvalidTypecast::create( "MessageIterator: Extracting non dict into std::map" );

        return get_dict<Key,Data>();
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
      FileDescriptor::pointer get_filedescriptor();

      template <typename T>
      void get_array_simple( std::vector<T>& array ) {
        if ( not this->is_fixed() ) /* This should never happen */
          throw ErrorInvalidTypecast::create( "MessageIterator: Extracting non fixed array into std::vector" );
        
        T type;
        if ( this->element_type() != DBus::type( type ) ) {
          std::string s = "MessageIterator: Extracting DBus array type ";
          s += type_string(type);
          s += " into C++ vector with RTTI type ";
          s += typeid( T ).name();
	  throw ErrorInvalidTypecast::create( s.c_str() );
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
      
      template <typename T>
      std::vector<T> get_array_simple() {
        if ( not this->is_fixed() ) /* This should never happen */
          throw ErrorInvalidTypecast::create( "MessageIterator: Extracting non fixed array into std::vector" );
        
        T type;
        if ( this->element_type() != DBus::type( type ) ) {
          std::string s = "MessageIterator: Extracting DBus array type ";
          s += type_string(type);
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
      void get_array_complex(std::vector<T> &array) {
        if ( not this->is_array() ) /* Should never happen */
          throw ErrorInvalidTypecast::create( "MessageIterator: Extracting non array into std::vector" );

        T type;
        if ( this->element_type() != DBus::type( type ) ) {
          std::string s = "MessageIterator: Extracting DBus array type ";
          s += type_string(type);
          s += " into C++ vector with RTTI type ";
          s += typeid( T ).name();
          throw ErrorInvalidTypecast::create( s.c_str() );
        }

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

      template <typename T>
      std::vector<T> get_array_complex() {
        if ( not this->is_array() ) /* Should never happen */
          throw ErrorInvalidTypecast::create( "MessageIterator: Extracting non array into std::vector" );

        T type;
        if ( this->element_type() != DBus::type( type ) ) {
          std::string s = "MessageIterator: Extracting DBus array type ";
          s += type_string(type);
          s += " into C++ vector with RTTI type ";
          s += typeid( T ).name();
          throw ErrorInvalidTypecast::create( s.c_str() );
        }

        std::vector<T> array;

        MessageIterator subiter = this->recurse();
        while( subiter.is_valid() )
        {
          T val;
          subiter >> val;
          array.push_back( val );
        }

        return array;
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

/*
      template <typename T>
      void get_array(std::vector<T>& array) {
        
        if ( not this->is_array() )
          throw ErrorInvalidTypecast::create( "MessageIterator: Extracting non array into std::vector" );
        
        if ( not this->is_fixed() ){
//OKAY, so this is not fixed.
//go and extract elements one at a time.
          T type;
          MessageIterator subiter = this->recurse();
        
          array.clear();

          subiter >> type;

          array.push_back( type );

          return;
DBUS_CXX_DEBUG( "ERR: " << __FILE__ << __LINE__ );
          throw ErrorInvalidTypecast::create( "MessageIterator: Extracting non fixed array into std::vector" );
}
        
        T type;
        if ( this->element_type() != DBus::type( type ) ) {
          std::string s = "MessageIterator: Extracting DBus array type ";
          s += type_string(type);
          s += " into C++ vector with RTTI type ";
          s += typeid( T ).name();
	  throw ErrorInvalidTypecast::create( s.c_str() );
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
*/

      /*template<>
      void get_array<std::string>(std::vector<std::string> &array) {
        if ( not this->is_array() )
          throw ErrorInvalidTypecast::create( "MessageIterator: Extracting non array into std::vector" );

        if ( this->element_type() != DBus::type<std::string>() ) {
          std::string s = "MessageIterator: Extracting DBus array type ";
          s += type_string<std::string>();
          s += " into C++ vector with RTTI type ";
          s += typeid(std::string).name();
          throw ErrorInvalidTypecast::create( s.c_str() );
        }

        for(MessageIterator subiter = this->recurse(); subiter.is_valid(); subiter.next())
        {
          std::string v = subiter.get_string();
          array.push_back(v);
        }
      }
*/

      template <typename Key, typename Data>
      MessageIterator& operator>>( std::map<Key,Data>& m )
      {
        if ( not this->is_dict() )
          throw ErrorInvalidTypecast::create( "MessageIterator: Extracting non dict into std::map" );
        try{
          get_dict<Key,Data>( m );
          this->next();
          return *this;
	}catch(DBusCxxPointer<DBus::ErrorInvalidTypecast> e){
          //TODO make sure we don't need this? it doesn't make much sense
	  throw (ErrorInvalidTypecast)*e;
	}
      }
       

      template <typename T>
      MessageIterator& operator>>( std::vector<T>& v )
      {
        if ( not this->is_array() )
          throw ErrorInvalidTypecast::create( "MessageIterator: Extracting non array into std::vector" );
	try{
         // this->get_array<T>(v);
          if ( this->is_fixed() )
            get_array_simple<T>( v );
          else
            get_array_complex<T>( v );
          this->next();
          return *this;
	}catch(DBusCxxPointer<DBus::ErrorInvalidTypecast> e){
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
        }catch(DBusCxxPointer<DBus::ErrorInvalidTypecast> e){
          throw (ErrorInvalidTypecast)*e;
        }
      }

      template <typename T>
      MessageIterator& operator>>( Variant<T>& v )
      {
	try{
          MessageIterator subiter = this->recurse();
          dbus_message_iter_get_basic(subiter.cobj(), &v.data);
          this->next();
          return *this;
        }catch(DBusCxxPointer<DBus::ErrorInvalidTypecast> e){
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
          throw ErrorInvalidTypecast::create( s.c_str() );
        }
        dbus_message_iter_get_basic( &m_cobj, &temp );
      }

    protected:
      const Message* m_message;
      DBusMessageIter m_cobj;

  };

/*
  template<>
  void inline MessageIterator::get_array(std::vector<std::string> &array) {
    std::string s_;
    if ( not this->is_array() )
      throw ErrorInvalidTypecast::create( "MessageIterator: Extracting non array into std::vector" );

    if ( this->element_type() != DBus::type(s_) ) {
      std::string s = "MessageIterator: Extracting DBus array type ";
      s += type_string(s_);
      s += " into C++ vector with RTTI type ";
      s += typeid(std::string).name();
      throw ErrorInvalidTypecast::create( s.c_str() );
    }

    for(MessageIterator subiter = this->recurse(); subiter.is_valid(); subiter.next())
    {
      std::string v = subiter.get_string();
      array.push_back(v);
    }
  }
*/

}

#endif

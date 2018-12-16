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
#include <map>

#include <dbus/dbus.h>

#include <dbus-cxx/types.h>
#include <dbus-cxx/filedescriptor.h>

#ifndef DBUSCXX_MESSAGEAPPENDITERATOR_H
#define DBUSCXX_MESSAGEAPPENDITERATOR_H

namespace DBus
{

  class Message;

  /**
   * Insertion iterator allow values to be appended to a message
   *
   * @ingroup message
   *
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class MessageAppendIterator
  {
    public:

      MessageAppendIterator();

      MessageAppendIterator( Message& message );

      MessageAppendIterator( DBusCxxPointer<Message> message );

      ~MessageAppendIterator();

      /**
       * Returns a pointer to the message associated with this iterator or NULL
       * if no message is associated.
       */
      const Message* message() const;

      /** Returns a pointer to the underlying DBusMessageIter object */
      DBusMessageIter* cobj();

      /** Initializes the iterator for the specified message */
      bool init( Message& message );

      /** Invalidates the iterator */
      void invalidate();

      /** True if this is a valid iterator */
      bool is_valid() const;

      /** True if the iterator is valid and initialized, false otherwise */
      operator bool() const;

      bool append( bool v );
      bool append( uint8_t v );
      bool append( int16_t v );
      bool append( uint16_t v );
      bool append( int32_t v );
      bool append( uint32_t v );
      bool append( int64_t v );
      bool append( uint64_t v );
      bool append( double v );
      bool append( const char* v );
      bool append( const std::string& v );
      bool append( const Signature& v );
      bool append( const Path& v );
      bool append( const FileDescriptor::pointer& fd);
      
      bool append( char v );
      bool append( int8_t v );
      bool append( float v );
      #if DBUS_CXX_SIZEOF_LONG_INT == 4
        bool append( long int v );
        bool append( long unsigned int v );
      #endif

      template <typename T>
      bool append( const std::vector<T>& v){
        bool success;
        T type;
        success = this->open_container( CONTAINER_ARRAY, DBus::signature(type).c_str() );

        if( not success ){
          throw ErrorNoMemory::create();
        }

        for ( size_t i=0; i < v.size(); i++ )
          *m_subiter << v[i];

        return this->close_container();
      }

      template <typename Key, typename Data>
      bool append( const std::map<Key,Data>& dictionary ){
        std::string sig = signature_dict_data( dictionary );
        typename std::map<Key,Data>::const_iterator it;
        this->open_container( CONTAINER_ARRAY, sig );
        for ( it = dictionary.begin(); it != dictionary.end(); it++ ) {
          m_subiter->open_container( CONTAINER_DICT_ENTRY, std::string() );
          m_subiter->m_subiter->append( (*it).first );
          m_subiter->m_subiter->append( (*it).second );
          m_subiter->close_container();
        }
        return this->close_container();

      }

      template <typename T>
      bool append( const Variant<T> & var){
        this->open_container( CONTAINER_VARIANT, signature(var.data)  );
        m_subiter->append(var.data);
        return this->close_container();
      }

      template <typename T>
      MessageAppendIterator& operator<<( const T& v )
      {
        this->append( v );
        return *this;
      }


//           template <typename T0, typename T1>
//           void append(const Struct<T0,T1>& s) {
//             this->open_container( CONTAINER_STRUCT, std::string() );
//             m_subiter->append( boost::get<0>(s) );
//             m_subiter->append( boost::get<1>(s) );
//             this->close_container();
//           }
      //
//           template <typename T0, typename T1, typename T2>
//           void append(const Struct<T0,T1,T2>& s) {
//             this->open_container( CONTAINER_STRUCT, std::string() );
//             m_subiter->append( boost::get<0>(s) );
//             m_subiter->append( boost::get<1>(s) );
//             m_subiter->append( boost::get<2>(s) );
//             this->close_container();
//           }
      //
//           template <typename T0, typename T1, typename T2, typename T3>
//           void append(const Struct<T0,T1,T2,T3>& s) {
//             this->open_container( CONTAINER_STRUCT, std::string() );
//             m_subiter->append( boost::get<0>(s) );
//             m_subiter->append( boost::get<1>(s) );
//             m_subiter->append( boost::get<2>(s) );
//             m_subiter->append( boost::get<3>(s) );
//             this->close_container();
//           }
      //
//           template <typename T0, typename T1, typename T2, typename T3, typename T4>
//           void append(const Struct<T0,T1,T2,T3,T4>& s) {
//             this->open_container( CONTAINER_STRUCT, std::string() );
//             m_subiter->append( boost::get<0>(s) );
//             m_subiter->append( boost::get<1>(s) );
//             m_subiter->append( boost::get<2>(s) );
//             m_subiter->append( boost::get<3>(s) );
//             m_subiter->append( boost::get<4>(s) );
//             this->close_container();
//           }
      //
//           template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
//           void append(const Struct<T0,T1,T2,T3,T4,T5>& s) {
//             this->open_container( CONTAINER_STRUCT, std::string() );
//             m_subiter->append( boost::get<0>(s) );
//             m_subiter->append( boost::get<1>(s) );
//             m_subiter->append( boost::get<2>(s) );
//             m_subiter->append( boost::get<3>(s) );
//             m_subiter->append( boost::get<4>(s) );
//             m_subiter->append( boost::get<5>(s) );
//             this->close_container();
//           }
      //
//           template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
//           void append(const Struct<T0,T1,T2,T3,T4,T5,T6>& s) {
//             this->open_container( CONTAINER_STRUCT, std::string() );
//             m_subiter->append( boost::get<0>(s) );
//             m_subiter->append( boost::get<1>(s) );
//             m_subiter->append( boost::get<2>(s) );
//             m_subiter->append( boost::get<3>(s) );
//             m_subiter->append( boost::get<4>(s) );
//             m_subiter->append( boost::get<5>(s) );
//             m_subiter->append( boost::get<6>(s) );
//             this->close_container();
//           }

      bool open_container( ContainerType t, const std::string& contained_signature );

      bool close_container( );

      MessageAppendIterator* sub_iterator();

//           bool append_array( char type, const void* ptr, size_t length );

//           iterator new_array( const char* sig );

//           iterator new_variant( const char* sig );

//           iterator new_struct();

//           iterator new_dict_entry();

//           void close_container( iterator& container );

    protected:
      Message* m_message;
      DBusMessageIter m_cobj;
      MessageAppendIterator* m_subiter;

      template <typename T> bool protected_append( const T& v );
      bool protected_append( const bool& v );
      bool protected_append( const std::string& v );
      bool protected_append( const Signature& v );
      bool protected_append( const Path& v );
      bool protected_append( const FileDescriptor::pointer& fd );
  };

}

#endif


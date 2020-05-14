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
#include <stdint.h>
#include <dbus-cxx/enums.h>
#include <dbus-cxx/signature.h>
#include <dbus-cxx/marshaling.h>
#include <dbus-cxx/dbus-cxx-config.h>
#include <dbus/dbus.h>
#include <stddef.h>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>
#include "error.h"
#include "path.h"
#include "variant.h"

#ifndef DBUSCXX_MESSAGEAPPENDITERATOR_H
#define DBUSCXX_MESSAGEAPPENDITERATOR_H

namespace DBus
{

  class FileDescriptor;
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

      MessageAppendIterator( ContainerType container = ContainerType::None );

      MessageAppendIterator( Message& message, ContainerType container = ContainerType::None );

      MessageAppendIterator( std::shared_ptr<Message> message, ContainerType container = ContainerType::None );

      ~MessageAppendIterator();

      /**
       * Returns a pointer to the message associated with this iterator or NULL
       * if no message is associated.
       */
      const Message* message() const;

      /** Invalidates the iterator */
      void invalidate();

      /** True if this is a valid iterator */
      bool is_valid() const;

      /** True if the iterator is valid and initialized, false otherwise */
      operator bool() const;

      MessageAppendIterator& operator<<( const bool& v );
      MessageAppendIterator& operator<<( const uint8_t& v );
      MessageAppendIterator& operator<<( const int16_t& v );
      MessageAppendIterator& operator<<( const uint16_t& v );
      MessageAppendIterator& operator<<( const int32_t& v );
      MessageAppendIterator& operator<<( const uint32_t& v );
      MessageAppendIterator& operator<<( const int64_t& v );
      MessageAppendIterator& operator<<( const uint64_t& v );
      MessageAppendIterator& operator<<( const double& v );
      MessageAppendIterator& operator<<( const char* v );
      MessageAppendIterator& operator<<( const std::string& v );
      MessageAppendIterator& operator<<( const Signature& v );
      MessageAppendIterator& operator<<( const Path& v );
      MessageAppendIterator& operator<<( const std::shared_ptr<FileDescriptor> v );
      MessageAppendIterator& operator<<( const Variant& v );
/*
      #if DBUS_CXX_SIZEOF_LONG_INT == 4
        bool append( long int v );
        bool append( long unsigned int v );
      #endif
*/

      template <typename T>
      MessageAppendIterator& operator<<( const std::vector<T>& v){
        bool success;
        T type;

        success = this->open_container( ContainerType::ARRAY, DBus::signature(type) );

        if( not success ){
          throw ErrorNoMemory();
        }

        for ( size_t i=0; i < v.size(); i++ )
          *sub_iterator() << v[i];

        success = this->close_container();
        if( not success ){
          throw ErrorNoMemory();
        }

        return *this;
      }

      template <typename Key, typename Data>
      MessageAppendIterator& operator<<( const std::map<Key,Data>& dictionary ){
        std::string sig = signature_dict_data( dictionary );
        typename std::map<Key,Data>::const_iterator it;
        this->open_container( ContainerType::ARRAY, sig );
        for ( it = dictionary.begin(); it != dictionary.end(); it++ ) {
          sub_iterator()->open_container( ContainerType::DICT_ENTRY, std::string() );
          *(sub_iterator()->sub_iterator()) << it->first;
          *(sub_iterator()->sub_iterator()) << it->second;
          sub_iterator()->close_container();
        }
        this->close_container();

        return *this;
      }

      template <typename... T>
      MessageAppendIterator& operator<<( const std::tuple<T...>& tup){
        bool success;
        std::string signature = DBus::priv::dbus_signature<T...>().dbus_sig();
        success = this->open_container( ContainerType::STRUCT, signature.c_str() );
        MessageAppendIterator* subiter = sub_iterator();
        std::apply( [subiter](auto&& ...arg) mutable {
               (*subiter << ... << arg);
              },
        tup );
        this->close_container();

        return *this;
      }

      bool open_container( ContainerType t, const std::string& contained_signature );

      bool close_container( );

      MessageAppendIterator* sub_iterator();

    private:
      class priv_data;

      std::shared_ptr<priv_data> m_priv;
  };

}

#endif


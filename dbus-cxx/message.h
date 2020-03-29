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
#include <dbus-cxx/error.h>
#include <dbus-cxx/messageappenditerator.h>
#include <dbus-cxx/messageiterator.h>
#include <dbus/dbus.h>
#include <memory>
#include <string>
#include "enums.h"

#include <dbus-cxx/variant.h>

#ifndef DBUSCXX_MESSAGE_H
#define DBUSCXX_MESSAGE_H

namespace DBus
{
  class ReturnMessage;

  /**
   * @defgroup message DBus Messages
   * Messages may be either sent across the DBus or received from the DBus
   */

  /**
   * This class represents a basic DBus message and also serves as a base
   * class for the specialized message types (call, return, signal, error).
   * 
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   *
   * @ingroup message
   */

  // TODO support
  //  - dbus_message_type_from_string
  //  - dbus_message_type_to_string
  //  - dbus_message_marshal
  //  - dbus_message_demarshal
  
  class Message
  {
    protected:
      
      Message( MessageType type );

      Message( DBusMessage* cobj=nullptr, CreateMethod m = CreateMethod::ALIAS );

      Message( std::shared_ptr<Message> other, CreateMethod m = CreateMethod::ALIAS );

      Message( std::shared_ptr<const Message> other, CreateMethod m = CreateMethod::ALIAS );

    public:
      static std::shared_ptr<Message> create( MessageType type );

      static std::shared_ptr<Message> create( DBusMessage* cobj=nullptr, CreateMethod m = CreateMethod::ALIAS );

      static std::shared_ptr<Message> create( std::shared_ptr<Message> other, CreateMethod m = CreateMethod::ALIAS );

      static std::shared_ptr<Message> create( std::shared_ptr<const Message> other, CreateMethod m = CreateMethod::ALIAS );

      std::shared_ptr<ReturnMessage> create_reply() const;

      virtual ~Message();

      Message& operator = ( const Message& m );

      bool operator == ( const Message& other );

      bool is_valid() const;

      void invalidate();

      operator bool() const;

      uint32_t serial() const;

//       Message copy();

      virtual MessageType type() const;

      void set_auto_start( bool auto_start);

      bool auto_start();

      bool set_destination( const std::string& s );

      const char* destination() const;

      bool set_sender( const std::string& s );

      const char* sender() const;

      bool is_call( const std::string& interface, const std::string& method ) const;

      bool is_signal( const std::string& interface, const std::string& signal_name ) const;

      bool is_error( const std::string& error_name ) const;

      bool has_destination( const std::string& name ) const;

      bool has_sender( const std::string& name ) const;

      Signature signature() const;

      bool has_signature( const std::string& signature ) const;

      template <typename T>
      MessageIterator operator>>( T& value ) const
      {
        MessageIterator iter = this->begin();
        iter >> value;
        return iter;
      }
      
      template <typename T>
      MessageAppendIterator operator<<( const T& value )
      {
        MessageAppendIterator aiter( *this );
        aiter << value;
        return aiter;
      }

      MessageIterator begin() const;

      MessageIterator end() const;

      MessageAppendIterator append();

      DBusMessage* cobj() const;

void serialize_to_vector( std::vector<uint8_t>* vec, uint32_t serial ) const;

/**
 * Returns the given header field(if it exists), otherwise returns a default
 * constructed variant.
 *
 * @param field The field number to get
 * @return The data, otherwise a default-constructed variant.
 */
Variant header_field( uint8_t field ) const;

static std::shared_ptr<Message> create_from_data( uint8_t* data, uint32_t data_len );

    protected:

void append_signature( std::string toappend );

      friend void init(bool);

      DBusMessage* m_cobj;

      bool m_valid;

std::map<uint8_t,Variant> m_headerMap;
std::vector<uint8_t> m_body;
Endianess m_endianess;

friend class MessageAppendIterator;
friend class MessageIterator;
friend std::ostream& operator<<( std::ostream& os, const DBus::Message* msg );

  };


template <typename T>
inline
DBus::MessageIterator operator>>( std::shared_ptr<const DBus::Message> ptr, T& value )
{
  if ( not ptr ) throw DBus::ErrorInvalidSharedPtr();
  return (*ptr) >> value;
}

template <typename T>
inline
DBus::MessageAppendIterator operator<<( std::shared_ptr<DBus::Message> ptr, const T& value )
{
  if ( not ptr ) throw DBus::ErrorInvalidSharedPtr();
  return (*ptr) << value;
}

}

#endif

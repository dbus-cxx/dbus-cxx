/***************************************************************************
 *   Copyright (C) 2007,2009 by Rick L. Vinyard, Jr.                       *
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
#include "message.h"
#include <dbus/dbus.h>
#include "messageappenditerator.h"
#include "messageiterator.h"
#include "returnmessage.h"
#include "variant.h"
#include "marshaling.h"
#include <dbus-cxx/dbus-cxx-private.h>
#include <dbus-cxx/simplelogger.h>

#include <ctype.h>
#include <stdio.h>

void hexdump(void *ptr, int buflen) {
fflush(stdout);
fflush(stderr);
  unsigned char *buf = (unsigned char*)ptr;
  int i, j;
  for (i=0; i<buflen; i+=16) {
    printf("%06x: ", i);
    for (j=0; j<16; j++) 
      if (i+j < buflen)
        printf("%02x ", buf[i+j]);
      else
        printf("   ");
    printf(" ");
    for (j=0; j<16; j++) 
      if (i+j < buflen)
        printf("%c", isprint(buf[i+j]) ? buf[i+j] : '.');
    printf("\n");
  }
fflush(stdout);
}

namespace DBus
{

  Message::Message( MessageType type ): m_valid(false)
  {
    m_cobj = dbus_message_new( static_cast<int>( type ) );
  }

  Message::Message( DBusMessage* cobj, CreateMethod m ): m_valid(false)
  {
    if ( cobj == nullptr )
    {
      m_cobj = cobj;
    }
    else if ( m == CreateMethod::ALIAS )
    {
      m_cobj = cobj;
      m_valid = true;
      dbus_message_ref(m_cobj);
    }
    else
    {
      m_cobj = dbus_message_copy(cobj);
      m_valid = true;
    }
  }

  Message::Message( std::shared_ptr<Message> other, CreateMethod m ): m_cobj(nullptr), m_valid(false)
  {
    if ( other and other->m_cobj != nullptr )
    {
      if ( m == CreateMethod::ALIAS )
      {
        m_cobj = other->m_cobj;
        dbus_message_ref(m_cobj);
      }
      else
      {
        m_cobj = dbus_message_copy(other->m_cobj);
      }
    }
  }

  Message::Message( std::shared_ptr<const Message> other, CreateMethod m ): m_cobj(nullptr), m_valid(false)
  {
    if ( other and other->m_cobj != nullptr )
    {
      if ( m == CreateMethod::ALIAS )
      {
        m_cobj = other->m_cobj;
        dbus_message_ref(m_cobj);
      }
      else
      {
        m_cobj = dbus_message_copy(other->m_cobj);
      }
    }
  }

  std::shared_ptr<Message> Message::create(MessageType type)
  {
    return std::shared_ptr<Message>(new Message(type) );
  }

  std::shared_ptr<Message> Message::create(DBusMessage * cobj, CreateMethod m)
  {
    return std::shared_ptr<Message>(new Message(cobj, m) );
  }

  std::shared_ptr<Message> Message::create(std::shared_ptr<Message> other, CreateMethod m)
  {
    return std::shared_ptr<Message>(new Message(other, m) );
  }

  std::shared_ptr<Message> Message::create(std::shared_ptr<const Message> other, CreateMethod m)
  {
    return std::shared_ptr<Message>(new Message(other, m) );
  }

  std::shared_ptr<ReturnMessage> Message::create_reply() const
  {
    if ( not this->is_valid() ) return std::shared_ptr<ReturnMessage>();
    if ( this->type() != MessageType::CALL ) return std::shared_ptr<ReturnMessage>();
    DBusMessage* rmcobj = dbus_message_new_method_return( this->cobj() );
    if ( not rmcobj ) return std::shared_ptr<ReturnMessage>();
    //when we create a new return message, this will increment the ref count.
    //make sure to decrement our ref count, as we don't need it anymore in this object
    std::shared_ptr<ReturnMessage> rmPtr = ReturnMessage::create(rmcobj);
    dbus_message_unref( rmcobj );
    return rmPtr;
  }

  Message::~Message()
  {
    if ( m_cobj ) dbus_message_unref( m_cobj );
  }

  Message& Message::operator = ( const Message& m )
  {
    if ( m_cobj != nullptr )
      dbus_message_unref( m_cobj );
    m_cobj = m.m_cobj;
    if ( m_cobj != nullptr ) {
      dbus_message_ref( m_cobj );
    }
    return *this;
  }

  bool Message::operator == ( const Message& other )
  {
    return m_cobj == other.m_cobj;
  }

  bool Message::is_valid() const
  {
    // TODO fix this
//     return ( m_cobj != nullptr and m_valid );
    return m_cobj != nullptr;
  }

  void Message::invalidate()
  {
    m_valid = false;
  }

  Message::operator bool() const
  {
    return this->is_valid();
  }

  uint32_t Message::serial() const
  {
    if ( m_cobj ) return dbus_message_get_serial( m_cobj );
    return 0;
  }

//   Message Message::copy()
//   {
    // TODO fix
//     return Message( *this, CreateMethod::COPY );
//   }

  MessageType Message::type() const
  {
    if ( m_cobj ) return static_cast<MessageType>( dbus_message_get_type( m_cobj ) );
    return MessageType::INVALID;
  }

  void Message::set_auto_start( bool auto_start)
  {
    if ( m_cobj == nullptr ) return;
    dbus_message_set_auto_start( m_cobj, auto_start );
  }

  bool Message::auto_start()
  {
    if ( m_cobj == nullptr ) return false;
    return dbus_message_get_auto_start( m_cobj );
  }

  bool Message::set_destination( const std::string& s )
  {
    if ( m_cobj == nullptr ) return false;
m_headerMap[ 6 ] = DBus::Variant( s );
    return dbus_message_set_destination( m_cobj, s.c_str() );
  }

  const char* Message::destination() const
  {
    if ( m_cobj == nullptr ) return NULL;
    return dbus_message_get_destination( m_cobj );
  }

  bool Message::set_sender( const std::string& s )
  {
    if ( m_cobj == nullptr ) return false;
    return dbus_message_set_sender( m_cobj, s.c_str() );
  }

  const char* Message::sender() const
  {
    if ( m_cobj == nullptr ) return NULL;
    return dbus_message_get_sender( m_cobj );
  }

  bool Message::is_call( const std::string& interface, const std::string& method ) const
  {
    if ( m_cobj == nullptr ) return false;
    return dbus_message_is_method_call( m_cobj, interface.c_str(), method.c_str() );
  }

  bool Message::is_signal( const std::string& interface, const std::string& signal_name ) const
  {
    if ( m_cobj == nullptr ) return false;
    return dbus_message_is_signal( m_cobj, interface.c_str(), signal_name.c_str() );
  }

  bool Message::is_error( const std::string& error_name ) const
  {
    if ( m_cobj == nullptr ) return false;
    return dbus_message_is_error( m_cobj, error_name.c_str() );
  }

  bool Message::has_destination( const std::string& name ) const
  {
    if ( m_cobj == nullptr ) return false;
    return dbus_message_has_destination( m_cobj, name.c_str() );
  }

  bool Message::has_sender( const std::string& name ) const
  {
    if ( m_cobj == nullptr ) return false;
    return dbus_message_has_sender( m_cobj, name.c_str() );
  }

  MessageIterator Message::begin() const
  {
    return MessageIterator( *this );
  }

  MessageIterator Message::end() const
  {
    return MessageIterator();
  }

  MessageAppendIterator Message::append()
  {
    return MessageAppendIterator( *this );
  }

  DBusMessage* Message::cobj( ) const
  {
    return m_cobj;
  }

void Message::serialize_to_vector( std::vector<uint8_t>* vec, uint32_t serial ) const {
    Marshaling marshal( vec, Endianess::Big );

	vec->reserve( m_body.size() + 256 );
    marshal.marshal( static_cast<uint8_t>( 'B' ) );
    switch( type() ){
    case MessageType::INVALID:
        return;
    case MessageType::CALL:
        marshal.marshal( static_cast<uint8_t>( 1 ) );
		break;
    case MessageType::RETURN:
        marshal.marshal( static_cast<uint8_t>( 2 ) );
		break;
    case MessageType::ERROR:
        marshal.marshal( static_cast<uint8_t>( 3 ) );
		break;
    case MessageType::SIGNAL:
        marshal.marshal( static_cast<uint8_t>( 4 ) );
		break;
	}

	// Now marshal the flags
    marshal.marshal( static_cast<uint8_t>( 0 ) );

	// Marshal the protocol version
    marshal.marshal( static_cast<uint8_t>( 1 ) );

	// Marshal the length
    marshal.marshal( static_cast<uint32_t>( m_body.size() ) );

	// Marshal the serial
    marshal.marshal( serial );

    std::ostringstream logmsg;
    // Marshal our header array
    marshal.marshal( static_cast<uint32_t>( 0 ) ); // The size of the header array; we update this later

	for( const std::pair<uint8_t,Variant>& entry : m_headerMap ){
        marshal.align( 8 );
        marshal.marshal( entry.first );
        marshal.marshal( entry.second );
	}
    // The size of the header array is always at offset 12
    marshal.marshalAtOffset( 12, static_cast<uint32_t>( vec->size() ) - 16 );

    // Align the message data to an 8-byte boundary and add the data!
    marshal.align( 8 );

    for( const uint8_t& byte : m_body ){
        vec->push_back( byte );
    }

    logmsg.str( "" );
    logmsg.clear();
    logmsg << "marshaled size: " << vec->size() << " array size: " << vec->size() - 12;
        SIMPLELOGGER_DEBUG("dbus.Message", logmsg.str() );

	hexdump( vec->data(), vec->size() );
}

}


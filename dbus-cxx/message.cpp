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

}


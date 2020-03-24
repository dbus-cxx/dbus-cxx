/***************************************************************************
 *   Copyright (C) 2009 by Rick L. Vinyard, Jr.                            *
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
#include "returnmessage.h"
#include "message.h"

namespace DBus
{

  ReturnMessage::ReturnMessage( )
  {
  }

  ReturnMessage::ReturnMessage( DBusMessage* callee ):
      Message(callee)
  {
  }

  ReturnMessage::ReturnMessage( std::shared_ptr<Message> callee ):
      Message(callee)
  {
  }

  ReturnMessage::ReturnMessage( std::shared_ptr<const Message> callee ):
      Message(callee)
  {
  }

  std::shared_ptr<ReturnMessage> ReturnMessage::create()
  {
    return std::shared_ptr<ReturnMessage>(new ReturnMessage() );
  }

  std::shared_ptr<ReturnMessage> ReturnMessage::create(DBusMessage * callee)
  {
    return std::shared_ptr<ReturnMessage>(new ReturnMessage(callee) );
  }

  std::shared_ptr<ReturnMessage> ReturnMessage::create(std::shared_ptr<Message> callee)
  {
    return std::shared_ptr<ReturnMessage>(new ReturnMessage(callee) );
  }

  std::shared_ptr<ReturnMessage> ReturnMessage::create(std::shared_ptr<const Message> callee)
  {
    return std::shared_ptr<ReturnMessage>(new ReturnMessage(callee) );
  }

  const char* ReturnMessage::signature() const
  {
    return dbus_message_get_signature( m_cobj );
  }

  bool ReturnMessage::has_signature( const std::string& signature ) const
  {
    return dbus_message_has_signature( m_cobj, signature.c_str() );
  }

  ReturnMessage& ReturnMessage::operator=( const Message& other )
  {
    m_cobj = other.cobj();
    if ( other.cobj() != nullptr ) dbus_message_ref( m_cobj );
    return *this;
  }

  bool ReturnMessage::set_reply_serial( uint32_t s )
  {
    if ( m_cobj ) return dbus_message_set_reply_serial( m_cobj, s );
    return false;
  }

  uint32_t ReturnMessage::reply_serial() const
  {
    if ( m_cobj ) return dbus_message_get_reply_serial( m_cobj );
    return 0;
  }

}


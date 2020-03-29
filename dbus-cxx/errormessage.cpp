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
#include "errormessage.h"
#include <dbus/dbus.h>
#include "enums.h"
#include "error.h"
#include "message.h"

namespace DBus
{

  ErrorMessage::ErrorMessage()
  {
    m_cobj = dbus_message_new( DBUS_MESSAGE_TYPE_ERROR );
  }

  ErrorMessage::ErrorMessage( DBusMessage* msg )
  {
    if ( msg == nullptr )
      throw ErrorInvalidCObject();

    if ( dbus_message_get_type( msg ) != DBUS_MESSAGE_TYPE_ERROR )
      throw ErrorInvalidMessageType();

    m_cobj = msg;
    dbus_message_ref( m_cobj );
  }

  ErrorMessage::ErrorMessage(std::shared_ptr<Message> msg )
  {
    if ( msg->type() != MessageType::ERROR )
      throw ErrorInvalidMessageType();

    if ( msg and *msg )
    {
      m_cobj = msg->cobj();
      dbus_message_ref( m_cobj );
    }
  }

  ErrorMessage::ErrorMessage( std::shared_ptr<const Message> to_reply, const std::string& name, const std::string& message )
  {
    if ( to_reply and *to_reply )
      m_cobj = dbus_message_new_error( to_reply->cobj(), name.c_str(), message.c_str() );
  }

  std::shared_ptr<ErrorMessage> ErrorMessage::create()
  {
    return std::shared_ptr<ErrorMessage>(new ErrorMessage() );
  }

  std::shared_ptr<ErrorMessage> ErrorMessage::create(DBusMessage * cobj)
  {
    return std::shared_ptr<ErrorMessage>(new ErrorMessage(cobj) );
  }

  std::shared_ptr<ErrorMessage> ErrorMessage::create(std::shared_ptr<Message> msg)
  {
    return std::shared_ptr<ErrorMessage>(new ErrorMessage(msg) );
  }

  std::shared_ptr<ErrorMessage> ErrorMessage::create(std::shared_ptr<const Message> msg, const std::string & name, const std::string & message)
  {
    return std::shared_ptr<ErrorMessage>(new ErrorMessage(msg, name, message) );
  }

  bool ErrorMessage::operator == ( const ErrorMessage& m ) const
  {
    return dbus_message_is_error( m_cobj, m.name() );
  }

  const char* ErrorMessage::name() const
  {
    return dbus_message_get_error_name( m_cobj );
  }

  bool ErrorMessage::set_name( const std::string& n )
  {
    return dbus_message_set_error_name( m_cobj, n.c_str() );
  }

  MessageType ErrorMessage::type() const {
      return MessageType::ERROR;
  }

}


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
#include <dbus-cxx/dbus-cxx-private.h>
#include "callmessage.h"
#include "enums.h"
#include "error.h"
#include "message.h"
#include "path.h"
#include "variant.h"

namespace DBus
{

  CallMessage::CallMessage()
  {
    m_cobj = dbus_message_new( static_cast<int>( MessageType::CALL ) );
  }

  CallMessage::CallMessage( DBusMessage* cobj )
  {
    if ( cobj == nullptr )
      throw ErrorInvalidCObject();

    if ( dbus_message_get_type( cobj ) != DBUS_MESSAGE_TYPE_METHOD_CALL )
      throw ErrorInvalidMessageType();

    m_cobj = cobj;
    dbus_message_ref( m_cobj );
  }

  CallMessage::CallMessage( std::shared_ptr<Message> msg )
  {
    if ( msg->type() != MessageType::CALL )
      throw ErrorInvalidMessageType();

    if ( msg and *msg )
    {
      m_cobj = msg->cobj();
      dbus_message_ref( m_cobj );
    }
  }

  CallMessage::CallMessage( std::shared_ptr<const Message> msg )
  {
    if ( msg->type() != MessageType::CALL )
      throw ErrorInvalidMessageType();

    if ( msg and *msg )
    {
      m_cobj = msg->cobj();
      dbus_message_ref( m_cobj );
    }
  }

  CallMessage::CallMessage( const std::string& dest, const std::string& path, const std::string& iface, const std::string& method )
  {
    m_cobj = dbus_message_new_method_call( dest.c_str(), path.c_str(), iface.c_str(), method.c_str() );
    m_valid = true;
    std::ostringstream debug_msg;
    debug_msg << "Creating call message to " << dest << " path: " << path << " " << iface << "." << method;
    SIMPLELOGGER_DEBUG( "DBus.CallMessage", debug_msg.str() );
m_headerMap[ 1 ] = DBus::Variant( Path( path ) );
m_headerMap[ 2 ] = DBus::Variant( iface );
m_headerMap[ 3 ] = DBus::Variant( method );
m_headerMap[ 6 ] = DBus::Variant( dest );
  }

  CallMessage::CallMessage( const std::string& path, const std::string& iface, const std::string& method )
  {
    m_cobj = dbus_message_new_method_call( nullptr, path.c_str(), iface.c_str(), method.c_str() );
    m_valid = true;
  }

  CallMessage::CallMessage( const std::string& path, const std::string& method )
  {
    m_cobj = dbus_message_new_method_call( nullptr, path.c_str(), nullptr, method.c_str() );
    m_valid = true;
  }

  std::shared_ptr<CallMessage> CallMessage::create()
  {
    return std::shared_ptr<CallMessage>( new CallMessage() );
  }

  std::shared_ptr<CallMessage> CallMessage::create(DBusMessage * cobj)
  {
    return std::shared_ptr<CallMessage>( new CallMessage(cobj) );
  }

  std::shared_ptr<CallMessage> CallMessage::create(std::shared_ptr<Message> msg)
  {
    return std::shared_ptr<CallMessage>( new CallMessage(msg) );
  }

  std::shared_ptr<const CallMessage> CallMessage::create(std::shared_ptr<const Message> msg)
  {
    return std::shared_ptr<const CallMessage>( new CallMessage(msg) );
  }

  std::shared_ptr<CallMessage> CallMessage::create(const std::string & dest, const std::string & path, const std::string & iface, const std::string & method)
  {
    return std::shared_ptr<CallMessage>( new CallMessage(dest, path, iface, method) );
  }

  std::shared_ptr<CallMessage> CallMessage::create(const std::string & path, const std::string & iface, const std::string & method)
  {
    return std::shared_ptr<CallMessage>( new CallMessage(path, iface, method) );
  }

  std::shared_ptr<CallMessage> CallMessage::create(const std::string & path, const std::string & method)
  {
    return std::shared_ptr<CallMessage>( new CallMessage(path, method) );
  }

  bool CallMessage::set_path( const std::string& p )
  {
    return dbus_message_set_path( m_cobj, p.c_str() );
  }

  Path CallMessage::path() const
  {
    return dbus_message_get_path( m_cobj );
  }

  bool CallMessage::has_path( const std::string& p ) const
  {
    return dbus_message_has_path( m_cobj, p.c_str() );
  }

  std::vector<std::string> CallMessage::path_decomposed() const
  {
    std::vector<std::string> decomposed;
    char** p;
    dbus_message_get_path_decomposed( m_cobj, &p );
    for ( char** q=p; q != nullptr; q++ )
      decomposed.push_back( *q );
    dbus_free_string_array( p );
    return decomposed;
  }

  bool CallMessage::set_interface( const std::string& i )
  {
    return dbus_message_set_interface( m_cobj, i.c_str() );
  }

  const char* CallMessage::interface() const {
      return dbus_message_get_interface( m_cobj );
    }

  bool CallMessage::has_interface( const std::string& i ) const
  {
    return dbus_message_has_interface( m_cobj, i.c_str() );
  }

  bool CallMessage::set_member( const std::string& m )
  {
    return dbus_message_set_member( m_cobj, m.c_str() );
  }

  const char* CallMessage::member() const
  {
    return dbus_message_get_member( m_cobj );
  }

  bool CallMessage::has_member( const std::string& m ) const
  {
    return dbus_message_has_member( m_cobj, m.c_str() );
  }

  bool CallMessage::operator == ( const CallMessage& m ) const
  {
    return dbus_message_is_method_call( m_cobj, m.interface(), m.member() );
  }

  void CallMessage::set_no_reply( bool no_reply )
  {
    if ( m_cobj == nullptr ) return;
    dbus_message_set_no_reply( m_cobj, no_reply );
  }

  bool CallMessage::expects_reply() const
  {
    if ( m_cobj == nullptr ) return false;
    return !dbus_message_get_no_reply( m_cobj );
  }

  MessageType CallMessage::type() const {
      return MessageType::CALL;
  }

}


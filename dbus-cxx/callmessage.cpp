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
#include "returnmessage.h"
#include "errormessage.h"

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
    m_valid = true;
    std::ostringstream debug_msg;
    debug_msg << "Creating call message to " << dest << " path: " << path << " " << iface << "." << method;
    SIMPLELOGGER_DEBUG( "DBus.CallMessage", debug_msg.str() );
    set_path( path );
    set_interface( iface );
    set_member( method );
    set_destination( dest );
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

  std::shared_ptr<ReturnMessage> CallMessage::create_reply() const
  {
    if ( not this->is_valid() ) return std::shared_ptr<ReturnMessage>();
    std::shared_ptr<ReturnMessage> retmsg = ReturnMessage::create();
    retmsg->set_reply_serial( serial() );
    if( m_flags & DBUSCXX_MESSAGE_NO_REPLY_EXPECTED ){
        retmsg->invalidate();
    }
    return retmsg;
  }

  std::shared_ptr<ErrorMessage> CallMessage::create_error_reply() const
  {
    if ( not this->is_valid() ) return std::shared_ptr<ErrorMessage>();
    std::shared_ptr<ErrorMessage> retmsg = ErrorMessage::create();
    retmsg->set_reply_serial( serial() );
    if( m_flags & DBUSCXX_MESSAGE_NO_REPLY_EXPECTED ){
        retmsg->invalidate();
    }
    return retmsg;
  }

  void CallMessage::set_path( const std::string& p )
  {
    m_headerMap[ MessageHeaderFields::Interface ] = Variant( Path( p ) );
  }

  Path CallMessage::path() const
  {
    Variant field = header_field( MessageHeaderFields::Path );
    if( field.currentType() == DataType::OBJECT_PATH ){
        return std::any_cast<Path>( field.value() );
    }

    return Path();
  }

  void CallMessage::set_interface( const std::string& i )
  {
    m_headerMap[ MessageHeaderFields::Interface ] = Variant( i );
  }

  std::string CallMessage::interface() const {
      Variant iface = header_field( MessageHeaderFields::Interface );
      if( iface.currentType() == DataType::STRING ){
          return std::any_cast<std::string>( iface.value() );
      }
    return "";
    }

  void CallMessage::set_member( const std::string& m )
  {
    m_headerMap[ MessageHeaderFields::Member ] = Variant( m );
  }

  std::string CallMessage::member() const
  {
      Variant member = header_field( MessageHeaderFields::Member );
      if( member.currentType() == DataType::STRING ){
          return std::any_cast<std::string>( member.value() );
      }
    return "";
  }

  bool CallMessage::operator == ( const CallMessage& m ) const
  {
    //return
      return false;
  }

  void CallMessage::set_no_reply( bool no_reply )
  {
      if( no_reply ){
        m_flags |= DBUSCXX_MESSAGE_NO_REPLY_EXPECTED;
      }else{
          m_flags &= (~DBUSCXX_MESSAGE_NO_REPLY_EXPECTED);
      }
  }

  bool CallMessage::expects_reply() const
  {
    return m_flags & DBUSCXX_MESSAGE_NO_REPLY_EXPECTED;
  }

  MessageType CallMessage::type() const {
      return MessageType::CALL;
  }

}


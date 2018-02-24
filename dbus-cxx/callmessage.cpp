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
#include "callmessage.h"

#include "returnmessage.h"

namespace DBus
{

  CallMessage::CallMessage()
  {
    m_cobj = dbus_message_new( CALL_MESSAGE );
  }

  CallMessage::CallMessage( DBusMessage* cobj )
  {
    if ( cobj == NULL )
      throw ErrorInvalidCObject::create();

    if ( dbus_message_get_type( cobj ) != DBUS_MESSAGE_TYPE_METHOD_CALL )
      throw ErrorInvalidMessageType::create();

    m_cobj = cobj;
    dbus_message_ref( m_cobj );
  }

  CallMessage::CallMessage( Message::pointer msg )
  {
    if ( msg->type() != CALL_MESSAGE )
      throw ErrorInvalidMessageType::create();

    if ( msg and *msg )
    {
      m_cobj = msg->cobj();
      dbus_message_ref( m_cobj );
    }
  }

  CallMessage::CallMessage( Message::const_pointer msg )
  {
    if ( msg->type() != CALL_MESSAGE )
      throw ErrorInvalidMessageType::create();

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
  }

  CallMessage::CallMessage( const std::string& path, const std::string& iface, const std::string& method )
  {
    m_cobj = dbus_message_new_method_call( NULL, path.c_str(), iface.c_str(), method.c_str() );
    m_valid = true;
  }

  CallMessage::CallMessage( const std::string& path, const std::string& method )
  {
    m_cobj = dbus_message_new_method_call( NULL, path.c_str(), NULL, method.c_str() );
    m_valid = true;
  }

  void callmsg_wp_deleter( void* v )
  {
    CallMessage::weak_pointer* wp = static_cast<CallMessage::weak_pointer*>(v);
    delete wp;
  }

  CallMessage::pointer CallMessage::create()
  {
    return pointer( new CallMessage() );
  }

  CallMessage::pointer CallMessage::create(DBusMessage * cobj)
  {
    try{
      return pointer( new CallMessage(cobj) );
    }catch(DBusCxxPointer<DBus::ErrorInvalidMessageType> err){
      throw err;
    }
  }

  CallMessage::pointer CallMessage::create(Message::pointer msg)
  {
    try{
      return pointer( new CallMessage(msg) );
    }catch(DBusCxxPointer<DBus::ErrorInvalidMessageType> err){
      throw err;
    }
  }

  CallMessage::const_pointer CallMessage::create(Message::const_pointer msg)
  {
    try{
      return const_pointer( new CallMessage(msg) );
    }catch(DBusCxxPointer<DBus::ErrorInvalidMessageType> err){
      throw err;
    }
  }

  CallMessage::pointer CallMessage::create(const std::string & dest, const std::string & path, const std::string & iface, const std::string & method)
  {
    return pointer( new CallMessage(dest, path, iface, method) );
  }

  CallMessage::pointer CallMessage::create(const std::string & path, const std::string & iface, const std::string & method)
  {
    return pointer( new CallMessage(path, iface, method) );
  }

  CallMessage::pointer CallMessage::create(const std::string & path, const std::string & method)
  {
    return pointer( new CallMessage(path, method) );
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
    for ( char** q=p; q != NULL; q++ )
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

  const char* CallMessage::signature() const
  {
    return dbus_message_get_signature( m_cobj );
  }

  bool CallMessage::has_signature( const std::string& signature ) const
  {
    return dbus_message_has_signature( m_cobj, signature.c_str() );
  }

  bool CallMessage::operator == ( const CallMessage& m ) const
  {
    return dbus_message_is_method_call( m_cobj, m.interface(), m.member() );
  }

  void CallMessage::set_no_reply( bool no_reply )
  {
    if ( m_cobj == NULL ) return;
    dbus_message_set_no_reply( m_cobj, no_reply );
  }

  bool CallMessage::expects_reply() const
  {
    if ( m_cobj == NULL ) return false;
    return !dbus_message_get_no_reply( m_cobj );
  }

}


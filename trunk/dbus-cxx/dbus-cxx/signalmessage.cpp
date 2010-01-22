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
#include "signalmessage.h"

namespace DBus
{

  SignalMessage::SignalMessage( DBusMessage* cobj, CreateMethod m ):
      Message(cobj, m)
  {
    if ( not cobj or dbus_message_get_type(cobj) != DBUS_MESSAGE_TYPE_SIGNAL )
      throw ErrorInvalidMessageType::create();
  }
  
  SignalMessage::SignalMessage( Message::pointer msg ):
      Message( msg )
  {
    if ( not msg or not *msg or msg->type() != SIGNAL_MESSAGE )
      throw ErrorInvalidMessageType::create();
  }

  SignalMessage::SignalMessage( Message::const_pointer msg ):
      Message( msg )
  {
    if ( not msg or not *msg or msg->type() != SIGNAL_MESSAGE )
      throw ErrorInvalidMessageType::create();
  }

  SignalMessage::SignalMessage( const std::string& name ):
      Message( SIGNAL_MESSAGE )
  {
    this->set_member( name );
  }

  SignalMessage::SignalMessage( const std::string& path, const std::string& interface, const std::string& name )
  {
    m_cobj = dbus_message_new_signal( path.c_str(), interface.c_str(), name.c_str() );
    if ( m_cobj == NULL )
      throw( ErrorNoMemory::create( "SignalMessage::SignalMessage: constructor failed because dbus couldn't allocate memory for signal" ) );
  }

  void sigmsg_wp_deleter( void* v )
  {
    SignalMessage::weak_pointer* wp = static_cast<SignalMessage::weak_pointer*>(v);
    delete wp;
  }

  SignalMessage::pointer SignalMessage::create( DBusMessage* cobj, CreateMethod m )
  {
    return pointer( new SignalMessage(cobj, m) );
  }

  SignalMessage::pointer SignalMessage::create(Message::pointer msg)
  {
    return pointer( new SignalMessage(msg) );
  }

  SignalMessage::const_pointer SignalMessage::create(Message::const_pointer msg)
  {
    return const_pointer( new SignalMessage(msg) );
  }

  SignalMessage::pointer SignalMessage::create(const std::string & name)
  {
    return pointer( new SignalMessage(name) );
  }

  SignalMessage::pointer SignalMessage::create(const std::string & path, const std::string & interface, const std::string & name)
  {
    return pointer( new SignalMessage(path, interface, name) );
  }

  bool SignalMessage::operator == ( const SignalMessage& m ) const
  {
    return dbus_message_is_signal( m_cobj, m.interface(), m.member() );
  }

  bool SignalMessage::set_path( const std::string& p )
  {
    return dbus_message_set_path( m_cobj, p.c_str() );
  }

  Path SignalMessage::path() const
  {
    return dbus_message_get_path( m_cobj );
  }

  bool SignalMessage::has_path( const std::string& p ) const
  {
    return dbus_message_has_path( m_cobj, p.c_str() );
  }

  std::vector<std::string> SignalMessage::path_decomposed() const
  {
    std::vector<std::string> decomposed;
    char** p;
    dbus_message_get_path_decomposed( m_cobj, &p );
    for ( char** q=p; q != NULL; q++ )
      decomposed.push_back( *q );
    dbus_free_string_array( p );
    return decomposed;
  }

  bool SignalMessage::set_interface( const std::string& i )
  {
    return dbus_message_set_interface( m_cobj, i.c_str() );
  }

  const char* SignalMessage::interface() const {
      return dbus_message_get_interface( m_cobj );
    }

  bool SignalMessage::has_interface( const std::string& i ) const
  {
    return dbus_message_has_interface( m_cobj, i.c_str() );
  }

  bool SignalMessage::set_member( const std::string& m )
  {
    return dbus_message_set_member( m_cobj, m.c_str() );
  }

  const char* SignalMessage::member() const
  {
    return dbus_message_get_member( m_cobj );
  }

  bool SignalMessage::has_member( const std::string& m ) const
  {
    return dbus_message_has_member( m_cobj, m.c_str() );
  }

}


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
#include <dbus/dbus.h>
#include "error.h"
#include "message.h"

namespace DBus
{

  SignalMessage::SignalMessage( ):
      Message()
  {
  }
  
  SignalMessage::SignalMessage( std::shared_ptr<Message> msg ):
      Message( msg )
  {
    if ( not msg or not *msg or msg->type() != MessageType::SIGNAL )
      throw ErrorInvalidMessageType();
  }

  SignalMessage::SignalMessage( std::shared_ptr<const Message> msg ):
      Message( msg )
  {
    if ( not msg or not *msg or msg->type() != MessageType::SIGNAL )
      throw ErrorInvalidMessageType();
  }

  SignalMessage::SignalMessage( const std::string& name ):
      Message( MessageType::SIGNAL )
  {
    this->set_member( name );
  }

  SignalMessage::SignalMessage( const std::string& path, const std::string& interface, const std::string& name )
  {
//    m_cobj = dbus_message_new_signal( path.c_str(), interface.c_str(), name.c_str() );
//    if ( m_cobj == NULL )
//      throw( ErrorNoMemory( "SignalMessage::SignalMessage: constructor failed because dbus couldn't allocate memory for signal" ) );
  }

  std::shared_ptr<SignalMessage> SignalMessage::create( )
  {
    return std::shared_ptr<SignalMessage>( new SignalMessage() );
  }

  std::shared_ptr<SignalMessage> SignalMessage::create(std::shared_ptr<Message> msg)
  {
    return std::shared_ptr<SignalMessage>( new SignalMessage(msg) );
  }

  std::shared_ptr<const SignalMessage> SignalMessage::create(std::shared_ptr<const Message> msg)
  {
    return std::shared_ptr<const SignalMessage>( new SignalMessage(msg) );
  }

  std::shared_ptr<SignalMessage> SignalMessage::create(const std::string & name)
  {
    return std::shared_ptr<SignalMessage>( new SignalMessage(name) );
  }

  std::shared_ptr<SignalMessage> SignalMessage::create(const std::string & path, const std::string & interface, const std::string & name)
  {
    return std::shared_ptr<SignalMessage>( new SignalMessage(path, interface, name) );
  }

  bool SignalMessage::operator == ( const SignalMessage& m ) const
  {
    //return dbus_message_is_signal( m_cobj, m.interface(), m.member() );
  }

  bool SignalMessage::set_path( const std::string& p )
  {
      m_headerMap[ MessageHeaderFields::Path ] = Variant( Path( p ) );
      return true;
  }

  Path SignalMessage::path() const
  {
      DBus::Variant memberName = header_field( MessageHeaderFields::Path );
      if( memberName.currentType() == DataType::OBJECT_PATH ){
          return std::any_cast<Path>( memberName.value() );
      }
    return std::string( "" );
  }

//  bool SignalMessage::has_path( const std::string& p ) const
//  {
//    //return dbus_message_has_path( m_cobj, p.c_str() );
//  }

  std::vector<std::string> SignalMessage::path_decomposed() const
  {
    std::vector<std::string> decomposed;
//    char** p;
//    dbus_message_get_path_decomposed( m_cobj, &p );
//    for ( char** q=p; q != NULL; q++ )
//      decomposed.push_back( *q );
//    dbus_free_string_array( p );
    return decomposed;
  }

  bool SignalMessage::set_interface( const std::string& i )
  {
//    return dbus_message_set_interface( m_cobj, i.c_str() );
  }

  std::string SignalMessage::interface() const {
      DBus::Variant memberName = header_field( MessageHeaderFields::Interface );
      if( memberName.currentType() == DataType::STRING ){
          return std::any_cast<std::string>( memberName.value() );
      }
    return std::string( "" );
    }

//  bool SignalMessage::has_interface( const std::string& i ) const
//  {
////    return dbus_message_has_interface( m_cobj, i.c_str() );
//  }

  bool SignalMessage::set_member( const std::string& m )
  {
    m_headerMap[ MessageHeaderFields::Member ] = Variant( m );
    return true;
  }

  std::string SignalMessage::member() const
  {
      DBus::Variant memberName = header_field( MessageHeaderFields::Member );
      if( memberName.currentType() == DataType::STRING ){
          return std::any_cast<std::string>( memberName.value() );
      }
    return std::string( "" );
  }

//  bool SignalMessage::has_member( const std::string& m ) const
//  {
////    return dbus_message_has_member( m_cobj, m.c_str() );
//  }

  MessageType SignalMessage::type() const {
      return MessageType::SIGNAL;
  }

}


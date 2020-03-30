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
#include "enums.h"
#include "error.h"
#include "message.h"
#include "types.h"

namespace DBus
{

  ErrorMessage::ErrorMessage()
  {
  }

  ErrorMessage::ErrorMessage( std::shared_ptr<const CallMessage> to_reply, const std::string& name, const std::string& message )
  {
    if ( to_reply ){
        m_headerMap[ DBUSCXX_HEADER_FIELD_REPLY_SERIAL ] = Variant( to_reply->serial() );
    }
    m_headerMap[ DBUSCXX_HEADER_FIELD_ERROR_NAME ] = Variant( name );
    append() << message;
  }

  std::shared_ptr<ErrorMessage> ErrorMessage::create()
  {
    return std::shared_ptr<ErrorMessage>(new ErrorMessage() );
  }

  std::shared_ptr<ErrorMessage> ErrorMessage::create(std::shared_ptr<const CallMessage> msg, const std::string & name, const std::string & message)
  {
    return std::shared_ptr<ErrorMessage>(new ErrorMessage(msg, name, message) );
  }

  bool ErrorMessage::operator == ( const ErrorMessage& m ) const
  {
    return name() == m.name() && message() == m.message();
  }

  std::string ErrorMessage::name() const
  {
      Variant msgName = header_field( DBUSCXX_HEADER_FIELD_ERROR_NAME );
      if( msgName.currentType() == DataType::STRING ){
          return std::any_cast<std::string>( msgName.value() );
      }
    return "";
  }

  void ErrorMessage::set_name( const std::string& n )
  {
    m_headerMap[ DBUSCXX_HEADER_FIELD_ERROR_NAME ] = Variant( n );
  }

  MessageType ErrorMessage::type() const {
      return MessageType::ERROR;
  }

  std::string ErrorMessage::message() const {
      Variant signature = header_field( DBUSCXX_HEADER_FIELD_SIGNATURE );
      std::string retval;

      if( signature.currentType() == DataType::SIGNATURE ){
        Signature value = std::any_cast<Signature>( signature.value() );
        if( value.begin().type() == DataType::STRING ){
            begin() >> retval;
        }
      }
    return retval;
  }

  void ErrorMessage::set_message( const std::string& message ) {
      clear_sig_and_data();
      append() << message;
  }

}


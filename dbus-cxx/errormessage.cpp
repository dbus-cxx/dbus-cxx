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
#include "dbus-error.h"

#define DBUS_ERROR_CHECK(err_name,error_throw) do{ if( name() == err_name ) throw error_throw( message() ); }while(0)

namespace DBus
{

  ErrorMessage::ErrorMessage()
  {
  }

  ErrorMessage::ErrorMessage( std::shared_ptr<const CallMessage> to_reply, const std::string& name, const std::string& message )
  {
    if ( to_reply ){
        set_header_field( MessageHeaderFields::Reply_Serial, Variant( to_reply->serial() ) );
    }
    set_header_field( MessageHeaderFields::Error_Name, Variant( name ) );
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
      Variant msgName = header_field( MessageHeaderFields::Error_Name );
      if( msgName.currentType() == DataType::STRING ){
          return std::any_cast<std::string>( msgName.value() );
      }
    return "";
  }

  void ErrorMessage::set_name( const std::string& n )
  {
    set_header_field( MessageHeaderFields::Error_Name, Variant( n ) );
  }

  MessageType ErrorMessage::type() const {
      return MessageType::ERROR;
  }

  std::string ErrorMessage::message() const {
      Variant signature = header_field( MessageHeaderFields::Signature );
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

  bool ErrorMessage::set_reply_serial( uint32_t s )
  {
    Variant serialField = Variant( s );
    set_header_field( MessageHeaderFields::Reply_Serial, serialField );
    return false;
  }

  uint32_t ErrorMessage::reply_serial() const
  {
    Variant field = header_field( MessageHeaderFields::Reply_Serial );
    if( field.currentType() == DataType::UINT32 ){
        return std::any_cast<uint32_t>( field.value() );
    }
    return 0;
  }

  void ErrorMessage::throw_error() {
    DBUS_ERROR_CHECK( DBUSCXX_ERROR_FAILED, ErrorFailed );
    DBUS_ERROR_CHECK( DBUSCXX_ERROR_SERVICE_UNKNOWN, ErrorServiceUnknown );
    DBUS_ERROR_CHECK( DBUSCXX_ERROR_NAME_HAS_NO_OWNER, ErrorNameHasNoOwner );
    DBUS_ERROR_CHECK( DBUSCXX_ERROR_NO_REPLY, ErrorNoReply );
    DBUS_ERROR_CHECK( DBUSCXX_ERROR_IO_ERROR, ErrorIOError );
    DBUS_ERROR_CHECK( DBUSCXX_ERROR_BAD_ADDRESS, ErrorBadAddress );
    DBUS_ERROR_CHECK( DBUSCXX_ERROR_NOT_SUPPORTED, ErrorNotSupported );
    DBUS_ERROR_CHECK( DBUSCXX_ERROR_LIMITS_EXCEEDED, ErrorLimitsExceeded );
    DBUS_ERROR_CHECK( DBUSCXX_ERROR_ACCESS_DENIED, ErrorAccessDenied );
    DBUS_ERROR_CHECK( DBUSCXX_ERROR_AUTH_FAILED, ErrorAuthFailed );
    DBUS_ERROR_CHECK( DBUSCXX_ERROR_NO_SERVER, ErrorNoServer );
    DBUS_ERROR_CHECK( DBUSCXX_ERROR_TIMEOUT, ErrorTimeout );
    DBUS_ERROR_CHECK( DBUSCXX_ERROR_NO_NETWORK, ErrorNoNetwork );
    DBUS_ERROR_CHECK( DBUSCXX_ERROR_ADDRESS_IN_USE, ErrorAddressInUse );
    DBUS_ERROR_CHECK( DBUSCXX_ERROR_DISCONNECTED, ErrorDisconnected );
    DBUS_ERROR_CHECK( DBUSCXX_ERROR_INVALID_ARGS, ErrorInvalidArgs );
    DBUS_ERROR_CHECK( DBUSCXX_ERROR_FILE_NOT_FOUND, ErrorFileNotFound );
    DBUS_ERROR_CHECK( DBUSCXX_ERROR_FILE_EXISTS, ErrorFileExists );
    DBUS_ERROR_CHECK( DBUSCXX_ERROR_UNKNOWN_METHOD, ErrorUnknownMethod );
    DBUS_ERROR_CHECK( DBUSCXX_ERROR_UNKNOWN_OBJECT, ErrorUnknownObject );
    DBUS_ERROR_CHECK( DBUSCXX_ERROR_UNKNOWN_INTERFACE, ErrorUnknownInterface );
    DBUS_ERROR_CHECK( DBUSCXX_ERROR_UNKNOWN_PROPERTY, ErrorUnknownProperty );
    DBUS_ERROR_CHECK( DBUSCXX_ERROR_PROPERTY_READ_ONLY, ErrorPropertyReadOnly );
    DBUS_ERROR_CHECK( DBUSCXX_ERROR_TIMED_OUT, ErrorTimedOut );
    DBUS_ERROR_CHECK( DBUSCXX_ERROR_MATCH_RULE_NOT_FOUND, ErrorMatchRuleNotFound );
    DBUS_ERROR_CHECK( DBUSCXX_ERROR_MATCH_RULE_INVALID, ErrorMatchRuleInvalid );

    throw Error( name(), message() );
  }

}


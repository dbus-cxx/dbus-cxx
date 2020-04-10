/***************************************************************************
 *   Copyright (C) 2007,2010 by Rick L. Vinyard, Jr.                       *
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
#include <dbus/dbus.h>
#include <ostream>

#ifndef DBUSCXX_ENUMS_H
#define DBUSCXX_ENUMS_H

namespace DBus {

  enum class BusType
  {
    NONE=-1,
    SESSION=DBUS_BUS_SESSION,     /**< The login session bus */
    SYSTEM=DBUS_BUS_SYSTEM,      /**< The systemwide bus */
    STARTER=DBUS_BUS_STARTER     /**< The bus that started us, if any */
  };


  enum class CreateMethod
  {
    ALIAS,
    COPY,
  };

  enum class DataType
  {
    INVALID = DBUS_TYPE_INVALID,
    BYTE = DBUS_TYPE_BYTE,
    BOOLEAN = DBUS_TYPE_BOOLEAN,
    INT16 = DBUS_TYPE_INT16,
    UINT16 = DBUS_TYPE_UINT16,
    INT32 = DBUS_TYPE_INT32,
    UINT32 = DBUS_TYPE_UINT32,
    INT64 = DBUS_TYPE_INT64,
    UINT64 = DBUS_TYPE_UINT64,
    DOUBLE = DBUS_TYPE_DOUBLE,
    STRING = DBUS_TYPE_STRING,
    OBJECT_PATH = DBUS_TYPE_OBJECT_PATH,
    SIGNATURE = DBUS_TYPE_SIGNATURE,
    ARRAY = DBUS_TYPE_ARRAY,
    VARIANT = DBUS_TYPE_VARIANT,
    STRUCT = DBUS_TYPE_STRUCT,
    DICT_ENTRY = DBUS_TYPE_DICT_ENTRY,
    UNIX_FD = DBUS_TYPE_UNIX_FD,
  };

  enum class ContainerType
  {
      None,
    ARRAY = DBUS_TYPE_ARRAY,
    VARIANT = DBUS_TYPE_VARIANT,
    STRUCT = DBUS_TYPE_STRUCT,
    DICT_ENTRY = DBUS_TYPE_DICT_ENTRY,
  };

  enum class MessageType : int32_t
  {
    INVALID = 0,
    CALL    = 1,
    RETURN  = 2,
    ERROR   = 3,
    SIGNAL  = 4,
  };

  enum class DispatchStatus
  {
    DATA_REMAINS = DBUS_DISPATCH_DATA_REMAINS,
    COMPLETE     = DBUS_DISPATCH_COMPLETE,
    NEED_MEMORY  = DBUS_DISPATCH_NEED_MEMORY,
  };

  enum class HandlerResult
  {
    HANDLED = DBUS_HANDLER_RESULT_HANDLED,
    NOT_HANDLED = DBUS_HANDLER_RESULT_NOT_YET_HANDLED,
    NEEDS_MEMORY = DBUS_HANDLER_RESULT_NEED_MEMORY,
  };

  enum class FilterResult
  {
    FILTER = DBUS_HANDLER_RESULT_HANDLED,
    DONT_FILTER = DBUS_HANDLER_RESULT_NOT_YET_HANDLED,
    NEEDS_MEMORY = DBUS_HANDLER_RESULT_NEED_MEMORY
  };

  enum class StartReply
  {
    FAILED = 0,
    SUCCESS = DBUS_START_REPLY_SUCCESS,
    ALREADY_RUNNING = DBUS_START_REPLY_ALREADY_RUNNING,
  };

  enum class WatchStatus
  {
    READABLE = DBUS_WATCH_READABLE,
    WRITABLE = DBUS_WATCH_WRITABLE,
    ERROR = DBUS_WATCH_ERROR,
    HANGUP = DBUS_WATCH_HANGUP
  };

  enum class PrimaryFallback
  {
    PRIMARY,
    FALLBACK
  };

  enum class Endianess {
      Little,
      Big,
  };

  enum class RegistrationStatus {
      Success,
      /** Unable to register object: There is already an object exported on this path */
      Failed_Path_in_Use,
      /** Unable to register object: invalid object */
      Failed_Invalid_Object,
      /** Unable to register object: No thread dispatcher for the given thread found */
      Failed_No_Thread_Dispatcher,
  };

  /**
   * Gives hints to the connection as to which thread should be
   * the one to call the methods on the given object.
   */
  enum class ThreadForCalling {
      /** Always call methods for this object from the dispatcher thread */
      DispatcherThread,
      /** Always call methods for this object from the current thread */
      CurrentThread,
  };

  enum class MessageHeaderFields {
      Invalid       = 0,
      Path          = 1,
      Interface     = 2,
      Member        = 3,
      Error_Name    = 4,
      Reply_Serial  = 5,
      Destination   = 6,
      Sender        = 7,
      Signature     = 8,
      Unix_FDs      = 9,
  };

  /**
   * Response status to attempting to register a name on the bus.
   */
  enum class RequestNameResponse {
      /** The caller is now the primary owner */
      PrimaryOwner,
      /** The caller is in the queue */
      NameInQueue,
      /** The name exists and we cannot get it */
      NameExists,
      /** We are already the owner of the specified name */
      AlreadyOwner
  };

  enum class ReleaseNameResponse {
      /** The name has been released */
      NameReleased,
      /** The given name does not exist on the bus */
      NameNonExistant,
      /** We are not the owner, and we also were not in the queue waiting to own the name */
      NotOwner
  };

  inline uint8_t header_field_to_int( MessageHeaderFields header ){
      switch ( header ){
      case MessageHeaderFields::Path:
          return 1;
      case MessageHeaderFields::Interface:
          return 2;
      case MessageHeaderFields::Member:
          return 3;
      case MessageHeaderFields::Error_Name:
          return 4;
      case MessageHeaderFields::Reply_Serial:
          return 5;
      case MessageHeaderFields::Destination:
          return 6;
      case MessageHeaderFields::Sender:
          return 7;
      case MessageHeaderFields::Signature:
          return 8;
      case MessageHeaderFields::Unix_FDs:
          return 9;
      default:
          return 0;
      }
  }

  inline MessageHeaderFields int_to_header_field( uint8_t val ){
      switch( val ){
      case 1:
          return MessageHeaderFields::Path;
      case 2:
          return MessageHeaderFields::Interface;
      case 3:
          return MessageHeaderFields::Member;
      case 4:
          return MessageHeaderFields::Error_Name;
      case 5:
          return MessageHeaderFields::Reply_Serial;
      case 6:
          return MessageHeaderFields::Destination;
      case 7:
          return MessageHeaderFields::Sender;
      case 8:
          return MessageHeaderFields::Signature;
      case 9:
          return MessageHeaderFields::Unix_FDs;
      }

      return MessageHeaderFields::Invalid;
  }

  inline std::ostream& operator<<( std::ostream& os, Endianess endian ){
      if( endian == Endianess::Little ){
          os << "Little";
      }else{
          os << "Big";
      }
      return os;
  }

  inline std::ostream& operator<<( std::ostream& os, MessageType type ){
      os << "MessageType::";
      switch ( type ) {
      case MessageType::INVALID:
          os << "INVALID";
          break;
      case MessageType::CALL:
          os << "CALL";
          break;
      case MessageType::RETURN:
          os << "RETURN";
          break;
      case MessageType::ERROR:
          os << "ERROR";
          break;
      case MessageType::SIGNAL:
          os << "SIGNAL";
          break;
      }

      return os;
  }

  inline std::ostream& operator<<( std::ostream& os, DispatchStatus status ){
      os << "DispatchStatus::";
      switch( status ){
      case DispatchStatus::DATA_REMAINS:
          os << "DATA_REMAINS";
          break;
      case DispatchStatus::COMPLETE:
          os << "COMPLETE";
          break;
      case DispatchStatus::NEED_MEMORY:
          os << "NEED_MEMORY";
          break;
      }

      return os;
  }

}

#endif


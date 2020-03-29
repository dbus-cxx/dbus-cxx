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

#define DBUSCXX_HEADER_FIELD_PATH           1
#define DBUSCXX_HEADER_FIELD_INTERFACE      2
#define DBUSCXX_HEADER_FIELD_MEMBER         3
#define DBUSCXX_HEADER_FIELD_ERROR_NAME     4
#define DBUSCXX_HEADER_FIELD_REPLY_SERIAL   5
#define DBUSCXX_HEADER_FIELD_DESTINATION    6
#define DBUSCXX_HEADER_FIELD_SENDER         7
#define DBUSCXX_HEADER_FIELD_SIGNATURE      8
#define DBUSCXX_HEADER_FIELD_UNIX_FDS       9

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

  enum class MessageType
  {
    INVALID = DBUS_MESSAGE_TYPE_INVALID,
    CALL    = DBUS_MESSAGE_TYPE_METHOD_CALL,
    RETURN  = DBUS_MESSAGE_TYPE_METHOD_RETURN,
    ERROR   = DBUS_MESSAGE_TYPE_ERROR,
    SIGNAL  = DBUS_MESSAGE_TYPE_SIGNAL,
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

  inline std::ostream& operator<<( std::ostream& os, Endianess endian ){
      if( endian == Endianess::Little ){
          os << "Little";
      }else{
          os << "Big";
      }
      return os;
  }

}

#endif


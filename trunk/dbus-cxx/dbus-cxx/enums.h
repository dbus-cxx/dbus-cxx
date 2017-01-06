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

#ifndef DBUSCXX_ENUMS_H
#define DBUSCXX_ENUMS_H

namespace DBus {

  typedef enum BusType
  {
    BUS_NONE=-1,
    BUS_SESSION=DBUS_BUS_SESSION,     /**< The login session bus */
    BUS_SYSTEM=DBUS_BUS_SYSTEM,      /**< The systemwide bus */
    BUS_STARTER=DBUS_BUS_STARTER     /**< The bus that started us, if any */
  } BusType;


  typedef enum CreateMethod
  {
    CREATE_ALIAS,
    CREATE_COPY,
  } CreateMethod;

  typedef enum Type
  {
    TYPE_INVALID = DBUS_TYPE_INVALID,
    TYPE_BYTE = DBUS_TYPE_BYTE,
    TYPE_BOOLEAN = DBUS_TYPE_BOOLEAN,
    TYPE_INT16 = DBUS_TYPE_INT16,
    TYPE_UINT16 = DBUS_TYPE_UINT16,
    TYPE_INT32 = DBUS_TYPE_INT32,
    TYPE_UINT32 = DBUS_TYPE_UINT32,
    TYPE_INT64 = DBUS_TYPE_INT64,
    TYPE_UINT64 = DBUS_TYPE_UINT64,
    TYPE_DOUBLE = DBUS_TYPE_DOUBLE,
    TYPE_STRING = DBUS_TYPE_STRING,
    TYPE_OBJECT_PATH = DBUS_TYPE_OBJECT_PATH,
    TYPE_SIGNATURE = DBUS_TYPE_SIGNATURE,
    TYPE_ARRAY = DBUS_TYPE_ARRAY,
    TYPE_VARIANT = DBUS_TYPE_VARIANT,
    TYPE_STRUCT = DBUS_TYPE_STRUCT,
    TYPE_DICT_ENTRY = DBUS_TYPE_DICT_ENTRY,
    TYPE_UNIX_FD = DBUS_TYPE_UNIX_FD,
  } Type;

  typedef enum ContainerType
  {
    CONTAINER_ARRAY = DBUS_TYPE_ARRAY,
    CONTAINER_VARIANT = DBUS_TYPE_VARIANT,
    CONTAINER_STRUCT = DBUS_TYPE_STRUCT,
    CONTAINER_DICT_ENTRY = DBUS_TYPE_DICT_ENTRY,
  } ContainerType;

  typedef enum MessageType
  {
    INVALID_MESSAGE = DBUS_MESSAGE_TYPE_INVALID,
    CALL_MESSAGE    = DBUS_MESSAGE_TYPE_METHOD_CALL,
    RETURN_MESSAGE  = DBUS_MESSAGE_TYPE_METHOD_RETURN,
    ERROR_MESSAGE   = DBUS_MESSAGE_TYPE_ERROR,
    SIGNAL_MESSAGE  = DBUS_MESSAGE_TYPE_SIGNAL,
  } MessageType;

  typedef enum DispatchStatus
  {
    DISPATCH_DATA_REMAINS = DBUS_DISPATCH_DATA_REMAINS,
    DISPATCH_COMPLETE     = DBUS_DISPATCH_COMPLETE,
    DISPATCH_NEED_MEMORY  = DBUS_DISPATCH_NEED_MEMORY,
  } DispatchStatus;

  typedef enum HandlerResult
  {
    HANDLED = DBUS_HANDLER_RESULT_HANDLED,
    NOT_HANDLED = DBUS_HANDLER_RESULT_NOT_YET_HANDLED,
    HANDLER_NEEDS_MEMORY = DBUS_HANDLER_RESULT_NEED_MEMORY,
  } HandlerResult;

  typedef enum FilterResult
  {
    FILTER = HANDLED,
    DONT_FILTER = NOT_HANDLED,
    FILTER_NEEDS_MEMORY = HANDLER_NEEDS_MEMORY
  } FilterResult;

  typedef enum StartReply
  {
    START_REPLY_FAILED = 0,
    START_REPLY_SUCCESS = DBUS_START_REPLY_SUCCESS,
    START_REPLY_ALREADY_RUNNING = DBUS_START_REPLY_ALREADY_RUNNING,
  } StartReply;

  typedef enum WatchStatus
  {
    WATCH_READABLE = DBUS_WATCH_READABLE,
    WATCH_WRITABLE = DBUS_WATCH_WRITABLE,
    WATCH_ERROR = DBUS_WATCH_ERROR,
    WATCH_HANGUP = DBUS_WATCH_HANGUP
  } WatchStatus;

  typedef enum PrimaryFallback
  {
    PRIMARY,
    FALLBACK
  } PrimaryFallback;

}

#endif


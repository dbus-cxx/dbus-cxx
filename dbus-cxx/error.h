/***************************************************************************
 *   Copyright (C) 2007,2008,2009,2010 by Rick L. Vinyard, Jr.             *
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
#include <exception>

#include <dbus-cxx/pointer.h>

#include <dbus/dbus.h>

#include <string>

#ifndef DBUSCXX_ERROR_H
#define DBUSCXX_ERROR_H

namespace DBus
{

  class Message;

  /**
   * @defgroup errors Errors
   *
   * A hierarchy of error objects descended from std::exception thrown when
   * exceptional conditions arise.
   */

  /**
   * Base class of a hierarchy of error objects thrown when exceptional conditions occur
   *
   * @ingroup core
   *
   * @ingroup errors
   *
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class Error : public std::exception
  {

    protected:
   
      Error();

      Error( DBusError* cobj );

      Error( const char* name, const char* message=NULL );

      Error( Message& );

    public:
      typedef DBusCxxPointer<Error> pointer;

      static pointer create();

      static pointer create( DBusError* cobj );

      static pointer create( const char* name, const char* message );

      static pointer create( Message& );

      ~Error() throw();

      const char* what() const throw();

      const char* name() const;

      const char* message() const;

      /** parameters MUST be static strings */
      void set( const char* name, const char* message );

      bool is_set() const;

      operator bool() const;

//       Error& operator=( Error& other );

      DBusError* cobj();

    protected:
      DBusError m_cobj;

  };

#define DBUSCXX_ERROR( CPPTYPE, DBUS_ERROR_CODE )            \
  class CPPTYPE : public Error {                             \
    public:                                                  \
    CPPTYPE( const char* message = NULL )                    \
        : Error( DBUS_ERROR_CODE, message ) {}               \
      typedef DBusCxxPointer<CPPTYPE> pointer;               \
      static pointer create( const char* message = NULL ) {  \
        return pointer( new CPPTYPE(message) );              \
      }                                                      \
  }

  /**
   * @class ErrorFailed
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorFailed, DBUS_ERROR_FAILED );
  
  /**
   * @class ErrorNoMemory
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorNoMemory, DBUS_ERROR_NO_MEMORY );

  /**
   * @class ErrorServiceUnknown
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorServiceUnknown, DBUS_ERROR_SERVICE_UNKNOWN );

  /**
   * @class ErrorNameHasNoOwner
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorNameHasNoOwner, DBUS_ERROR_NAME_HAS_NO_OWNER );

  /**
   * @class ErrorNoReply
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorNoReply, DBUS_ERROR_NO_REPLY );

  /**
   * @class ErrorIOError
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorIOError, DBUS_ERROR_IO_ERROR );

  /**
   * @class ErrorBadAddress
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorBadAddress, DBUS_ERROR_BAD_ADDRESS );

  /**
   * @class ErrorNotSupported
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorNotSupported, DBUS_ERROR_NOT_SUPPORTED );

  /**
   * @class ErrorLimitsExceeded
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorLimitsExceeded, DBUS_ERROR_LIMITS_EXCEEDED );

  /**
   * @class ErrorAccessDenied
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorAccessDenied, DBUS_ERROR_ACCESS_DENIED );

  /**
   * @class ErrorAuthFailed
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorAuthFailed, DBUS_ERROR_AUTH_FAILED );

  /**
   * @class ErrorNoServer
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorNoServer, DBUS_ERROR_NO_SERVER );

  /**
   * @class ErrorTimeout
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorTimeout, DBUS_ERROR_TIMEOUT );

  /**
   * @class ErrorNoNetwork
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorNoNetwork, DBUS_ERROR_NO_NETWORK );

  /**
   * @class ErrorAddressInUse
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorAddressInUse, DBUS_ERROR_ADDRESS_IN_USE );

  /**
   * @class ErrorDisconnected
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorDisconnected, DBUS_ERROR_DISCONNECTED );

  /**
   * @class ErrorInvalidArgs
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorInvalidArgs, DBUS_ERROR_INVALID_ARGS );

  /**
   * @class ErrorFileNotFound
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorFileNotFound, DBUS_ERROR_FILE_NOT_FOUND );

  /**
   * @class ErrorFileExists
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorFileExists, DBUS_ERROR_FILE_EXISTS );

  /**
   * @class ErrorUnknownMethod
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorUnknownMethod, DBUS_ERROR_UNKNOWN_METHOD );

  /**
   * @class ErrorTimedOut
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorTimedOut, DBUS_ERROR_TIMED_OUT );

  /**
   * @class ErrorMatchRuleNotFound
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorMatchRuleNotFound, DBUS_ERROR_MATCH_RULE_NOT_FOUND );

  /**
   * @class ErrorMatchRuleInvalid
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorMatchRuleInvalid, DBUS_ERROR_MATCH_RULE_INVALID );

  /**
   * @class ErrorSpawnExecFailed
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorSpawnExecFailed, DBUS_ERROR_SPAWN_EXEC_FAILED );

  /**
   * @class ErrorSpawnForkFailed
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorSpawnForkFailed, DBUS_ERROR_SPAWN_FORK_FAILED );

  /**
   * @class ErrorSpawnChildExited
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorSpawnChildExited, DBUS_ERROR_SPAWN_CHILD_EXITED );

  /**
   * @class ErrorSpawnChildSignaled
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorSpawnChildSignaled, DBUS_ERROR_SPAWN_CHILD_SIGNALED );

  /**
   * @class ErrorSpawnFailed
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorSpawnFailed, DBUS_ERROR_SPAWN_FAILED );

  /**
   * @class ErrorUnixProcessIdUnknown
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorUnixProcessIdUnknown, DBUS_ERROR_UNIX_PROCESS_ID_UNKNOWN );

  /**
   * @class ErrorInvalidSignature
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorInvalidSignature, DBUS_ERROR_INVALID_SIGNATURE );

  /**
   * @class ErrorInvalidFileContent
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorInvalidFileContent, DBUS_ERROR_INVALID_FILE_CONTENT );

  /**
   * @class ErrorSELinuxSecurityContextUnknown
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorSELinuxSecurityContextUnknown, DBUS_ERROR_SELINUX_SECURITY_CONTEXT_UNKNOWN );

  /**
   * @class ErrorInvalidCObject
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorInvalidCObject, "Invalid C Object" );

  /**
   * @class ErrorInvalidTypecast
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorInvalidTypecast, "Invalid Typecast" );

  /**
   * @class ErrorInvalidConnection
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorInvalidConnection, "DBus connection is invalid" );

  /**
   * @class ErrorInvalidMessageType
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorInvalidMessageType, "DBus message type does not match the C++ Message object type" );

  /**
   * @class ErrorNoConnection
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorNoConnection, "No connection specified and operation requires a DBus connection" );

  /**
   * @class ErrorDeadlock
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorDeadlock, "Locking a mutex a dbus-cxx thread already owns" );

  /**
   * @class ErrorNotOwner
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorNotOwner, "Unlocking a mutex a dbus-cxx thread doesn't own" );

  /**
   * @class ErrorNotInitialized
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorNotInitialized, "DBus::init(bool threadsafe=true) not called" );

  /**
   * @class ErrorInvalidAdaptee
   * @ingroup errors
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  DBUSCXX_ERROR( ErrorInvalidAdaptee, "DBus adapter method called, but adaptee is invalid" );

  /**
   * @class ErrorDispatcherInitFailed
   * @ingroup errors
   */
  DBUSCXX_ERROR( ErrorDispatcherInitFailed, "DBus::Dispatcher initialization failed" );
  

}

#endif

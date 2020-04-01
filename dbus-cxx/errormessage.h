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
#include <dbus-cxx/message.h>
#include <dbus-cxx/callmessage.h>
#include <dbus/dbus.h>
#include <memory>
#include <string>

#ifndef DBUSCXX_ERRORMESSAGE_H
#define DBUSCXX_ERRORMESSAGE_H

namespace DBus
{

  /**
   * Represents a DBus Error message
   *
   * This class inherits from Message and uses the dbus_message_* methods to
   * create an interface for an error message.
   *
   * @ingroup message
   *
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class ErrorMessage : public Message
  {
    protected:
      
      ErrorMessage();

      ErrorMessage( std::shared_ptr<const CallMessage> callMessage, const std::string& name, const std::string& message );

    public:
      static std::shared_ptr<ErrorMessage> create();

      static std::shared_ptr<ErrorMessage> create( std::shared_ptr<const CallMessage> callMessage, const std::string& name, const std::string& message );

      std::string name() const;

      void set_name( const std::string& n );

      std::string message() const;

      void set_message( const std::string& message );

      bool operator == ( const ErrorMessage& ) const;

      virtual MessageType type() const;

      bool set_reply_serial( uint32_t );

      uint32_t reply_serial() const;

      /**
       * Throw the error associated with this ErrorMessage.
       * If this error is of a well-known type, the well-known
       * type will be thrown.  Otherwise, a generic DBus::Error
       * will be thrown.
       */
      [[ noreturn ]] void throw_error();
  
  };

}

#endif

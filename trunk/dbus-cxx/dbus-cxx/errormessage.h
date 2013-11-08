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

      ErrorMessage( DBusMessage* cobj );

      ErrorMessage( Message::pointer msg );

      ErrorMessage( Message::const_pointer msg, const std::string& name, const std::string& message );

    public:

      typedef DBusCxxPointer<ErrorMessage> pointer;

      typedef DBusCxxWeakPointer<ErrorMessage> weak_pointer;

      static pointer create();

      static pointer create( DBusMessage* cobj );

      static pointer create( Message::pointer msg );

      static pointer create( Message::const_pointer msg, const std::string& name, const std::string& message );

      const char* name() const;

      bool set_name( const std::string& n );

      bool operator == ( const ErrorMessage& ) const;
  
  };

}

#endif

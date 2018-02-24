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
#include <dbus-cxx/callmessage.h>

#ifndef DBUSCXX_RETURNMESSAGE_H
#define DBUSCXX_RETURNMESSAGE_H

namespace DBus
{

  /**
   * Represents a DBus call return message
   *
   * This class inherits from Message and uses the dbus_message_* methods to
   * create an interface for a return message that is a response to a call
   * message.
   *
   * @ingroup message
   *
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class ReturnMessage : public Message
  {
    protected:
      
      ReturnMessage( );

      ReturnMessage( DBusMessage* callee );

      ReturnMessage( Message::pointer callee );

      ReturnMessage( Message::const_pointer callee );

    public:

      typedef DBusCxxPointer<ReturnMessage> pointer;

      typedef DBusCxxPointer<const ReturnMessage> const_pointer;

      typedef DBusCxxWeakPointer<ReturnMessage> weak_pointer;

      static pointer create( );

      static pointer create( DBusMessage* callee );

      static pointer create( Message::pointer callee );

      static pointer create( Message::const_pointer callee );

      const char* signature() const;

      bool has_signature( const std::string& signature ) const;

      ReturnMessage& operator=( const Message& other );

      bool set_reply_serial( uint32_t );

      uint32_t reply_serial() const;
  
  };


}

#endif

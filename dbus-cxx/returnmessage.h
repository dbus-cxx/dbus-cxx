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
#include <stdint.h>
#include <dbus-cxx/message.h>
#include <memory>
#include <string>

#ifndef DBUSCXX_RETURNMESSAGE_H
#define DBUSCXX_RETURNMESSAGE_H

namespace DBus
{

class CallMessage;

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

    public:
      static std::shared_ptr<ReturnMessage> create();

      static std::shared_ptr<ReturnMessage> create( std::shared_ptr<const CallMessage> callee );

      bool set_reply_serial( uint32_t );

      uint32_t reply_serial() const;

      virtual MessageType type() const;
  
  };


}

#endif

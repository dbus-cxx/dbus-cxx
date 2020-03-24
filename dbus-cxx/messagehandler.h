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
#include <dbus-cxx/enums.h>
#include <dbus/dbus.h>
#include <sigc++/signal.h>
#include <memory>

#ifndef DBUSCXX_MESSAGEHANDLER_H
#define DBUSCXX_MESSAGEHANDLER_H

namespace DBus
{
  class Connection;
  class Message;
  struct MessageHandlerAccumulator;

  /**
   * @ingroup objects
   * @ingroup local
   * 
   * This class provides a common base class for all message handlers
   *
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class MessageHandler : public sigc::trackable
  {
    protected:
      MessageHandler();

    public:
      /** The default implementation simply emits the message signal and returns the result */
      virtual HandlerResult handle_message( std::shared_ptr<Connection>, std::shared_ptr<const Message> );

      typedef sigc::signal<HandlerResult(std::shared_ptr<Connection>, std::shared_ptr<const Message>)>::accumulated<MessageHandlerAccumulator> MessageSignal;

      MessageSignal& signal_message();

    protected:

      MessageSignal m_signal_message;

      /**
       * When used as a callback, this method expects @param user_data to contain a pointer
       * to the MessageHandler instance.
       */
      static DBusHandlerResult message_handler_callback(DBusConnection* connection, DBusMessage* message, void* user_data);

  };

}

#endif

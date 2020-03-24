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
#include "messagehandler.h"
#include "connection.h"
#include "enums.h"
#include "message.h"
#include "accumulators.h"

namespace DBus
{

  MessageHandler::MessageHandler()
      : sigc::trackable()
  {
  }

  HandlerResult MessageHandler::handle_message(std::shared_ptr<Connection> conn, std::shared_ptr<const Message> msg)
  {
    return m_signal_message.emit(conn, msg);
  }

  MessageHandler::MessageSignal& MessageHandler::signal_message()
  {
    return m_signal_message;
  }

  DBusHandlerResult MessageHandler::message_handler_callback(DBusConnection * conn, DBusMessage * message, void * user_data)
  {
    if ( user_data == nullptr ) return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

    MessageHandler* handler = static_cast<MessageHandler*>(user_data);

    return static_cast<DBusHandlerResult>(handler->handle_message( Connection::self(conn), Message::create(message) ));
  }

}




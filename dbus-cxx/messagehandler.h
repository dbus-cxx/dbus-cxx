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
#include <memory>

#ifndef DBUSCXX_MESSAGEHANDLER_H
#define DBUSCXX_MESSAGEHANDLER_H

namespace DBus {
class Connection;
class Message;

/**
 * @ingroup objects
 * @ingroup local
 *
 * Interface indicating that the implementing classes can process messages.
 * Messages will be either CallMessage or SignalMessage.
 *
 * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
 */
class MessageHandler {
protected:
    MessageHandler();
    virtual ~MessageHandler();

public:
    /**
     * Attempt to handle the given message.  This method will only ever be called
     * when msg is either a CallMessage or a SignalMessage.
     *
     * The object that is handling the method should return a HandlerResult based
     * on the result of the message handling.  Any errors will then be automatically
     * propogated back over the bus.
     *
     * Alternatively, if the handler sends out a custom error message, return
     * HandlerResult::Handled to indicate that no errors need to be sent back.
     *
     * @param msg
     * @return
     */
    virtual HandlerResult handle_message( std::shared_ptr<const Message> msg ) = 0;

};

}

#endif

/***************************************************************************
 *   Copyright (C) 2020 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
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
#ifndef DBUSCXX_THREADDISPATCHER_H
#define DBUSCXX_THREADDISPATCHER_H

#include <memory>

namespace DBus{

class Message;
class ObjectPathHandler;

/**
 * A ThreadDispatcher is responsible for executing method calls
 * and signals on objects in the given thread.  If you are using
 * the standard DBus::Dispatcher class, you can either run all method calls
 * in the dispatcher thread, and create a ThreadDispatcher in the main
 * thread with a main loop.
 *
 * If you're using Qt, link with dbus-cxx-qt and use the QtThreadDispatcher
 * class.
 *
 * If you're using GLib, link with dbus-cxx-glib and use the GLibThreadDispatcher
 * class.
 */
class ThreadDispatcher {
public:
    virtual ~ThreadDispatcher(){}

    /**
     * When a new message comes in that needs to be processed, this method
     * is called with the object that the method needs to go to, and the
     * message to send to this object.
     *
     * Generally, this method should push the object/message onto some sort of
     * mutex-locked queue and then wakeup this thread.  The thread will
     * then lock the mutex, pop an element off of the queue, and direct
     * the message to the given object via the handle_message method
     *
     * @param object The object to send the message to
     * @param message The message to send to the object
     */
    virtual void add_message( std::shared_ptr<ObjectPathHandler> object, std::shared_ptr<const Message> message ) = 0;

};

} /* namespace DBus */

#endif /* DBUSCXX_THREADDISPATCHER_H */

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
#ifndef DBUSCXX_TRANSPORT_H
#define DBUSCXX_TRANSPORT_H

#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

namespace DBus {

class Message;

namespace priv {

class Transport {
public:
    virtual ~Transport();

    /**
     * Writes a message to the transport stream.
     *
     * @param message The message to write
     * @param serial The serial of the message to write.
     * @return The number of bytes written on success, an error code otherwise.
     */
    virtual ssize_t writeMessage( std::shared_ptr<const Message> message, uint32_t serial ) = 0;

    /**
     * Read a message from the transport stream.  If there is no message
     * to be read, or there is not enough data to read a message yet,
     * returns a default-constructed message.
     *
     * @return
     */
    virtual std::shared_ptr<Message> readMessage() = 0;

    /**
     * Check to see if this transport is valid.
     * @return
     */
    virtual bool is_valid() const = 0;

    /**
     * Returns the file descriptor that this transport acts on
     *
     * @return
     */
    virtual int fd() const = 0;

    /**
     * Open and return a transport based off of the given address.
     *
     * @param address The address to connect to, in DBus transport format
     * (e.g. unix:path=/tmp/dbus-test)
     * @return An opened file descriptor, or -1 on error(with errno set)
     */
    static std::shared_ptr<Transport> open_transport( std::string address );

protected:
    std::vector<uint8_t> m_serverAddress;

};

} /* namepsace priv */

} /* namespace DBus */

#endif /* DBUSCXX_TRANSPORT_H */

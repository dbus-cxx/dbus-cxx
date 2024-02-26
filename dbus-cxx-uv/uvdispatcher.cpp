// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
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
#include <cstring>
#include <system_error>
#include <uv.h>

#include <dbus-cxx/connection.h>
#include <dbus-cxx/dbus-cxx-private.h>

#include "uvdispatcher.h"

static const char* LOGGER_NAME = "DBus.Uv.UvDispatcher";

using DBus::Uv::UvDispatcher;

static void
close_cb(uv_handle_t* h)
{
    delete (uv_poll_t*)h;
}

static void
poll_cb(uv_poll_t* h, int status, int event);

namespace
{
class UVPollConnection
{
public:
    UVPollConnection(std::shared_ptr<DBus::Connection> connection) : connection(connection)
    {
        // uv_poll must be a naked pointer because the uv_close()
        // call completes ansynchronosuly; deletion is performed
        // in the close callback.
        uv_poll = new uv_poll_t;

        try {
            // the dbus-cxx transport always opens a socket so we use the
            // socket version of uv_poll_init. That is identical to the fd
            // call on Linux but socket specific on Windows.
            if (int r = uv_poll_init_socket(uv_default_loop(), uv_poll, connection->unix_fd()); r < 0) {
                throw std::system_error(r, std::system_category(), "uv_poll_init_socket");
            }

            uv_poll->data = this;
        }
        catch (...) {
            // despite the init failing, the loop retains a reference
            // to the handle so it must be closed.
            uv_close((uv_handle_t*)uv_poll, close_cb);

            // Re-throw the exception
            throw;
        }

        // Now the we have a valid poll handle, connect a signal to
        // the "needs dispatch" event which indicates when the
        // connection has data to write.
        connection->signal_needs_dispatch().connect(
            sigc::mem_fun(*this, &UVPollConnection::restart_writeable));

        // Finally, we can start the handle for reading
        restart();
    }

    UVPollConnection(const UVPollConnection &) = delete;
    UVPollConnection &operator=(const UVPollConnection &) = delete;

    UVPollConnection(UVPollConnection &&other) {
        uv_poll = other.uv_poll;
        other.uv_poll = nullptr;
        connection = std::move(other.connection);
    }

    virtual ~UVPollConnection()
    {
        // ensure the poll object is stopped so it doesn't try to
        // access the callback that is about to be destroyed
        uv_poll_stop(uv_poll);

        // zero out the reference for good measure
        uv_poll->data = nullptr;

        // Note: libuv owns the uv_poll allocation until the close
        // callback completes.
        uv_close((uv_handle_t*)uv_poll, close_cb);
    }

    void restart() { uv_poll_start(uv_poll, UV_READABLE, poll_cb); }

    void restart_writeable() { uv_poll_start(uv_poll, UV_READABLE | UV_WRITABLE, poll_cb); }

    std::shared_ptr<DBus::Connection> connection;

private:
    uv_poll_t* uv_poll = nullptr;
};
} // namespace


void
poll_cb(uv_poll_t* h, int status, int event)
{
    auto uvpc = static_cast<UVPollConnection*>(h->data);

    if (status == -EBADF) {
        // "bad file descrioptior" indicates the FD was a pipe
        // (FIFO) and the other end was closed
        //
        // log and ignore assuming that this is part of
        // pipeline shutdown
        //
        SIMPLELOGGER_DEBUG( LOGGER_NAME, "polled FD went bad, assmuming pipeline shutdown" );
    } else if (status < 0) {
        SIMPLELOGGER_ERROR( LOGGER_NAME, "poll_cb called with bad status: " << std::strerror(-status) << ", " << status);
    } else if (event & UV_WRITABLE) {
         // if we were polling for write-ability, we need to restart
         // the poll handle after dispatching to stop (the
         // signal_needs_dispatch handler will restart for write)
         while (uvpc->connection->dispatch() != DBus::DispatchStatus::COMPLETE) {
         }
         uvpc->restart();
     } else if (event & UV_READABLE) {
         // if there is no write-able event then there is no need to
         // restart to poll handle
         while (uvpc->connection->dispatch() != DBus::DispatchStatus::COMPLETE) {
         }
     }
}

class UvDispatcher::priv_data {
public:
    std::vector<UVPollConnection> m_connections;
};

UvDispatcher::UvDispatcher() :
    m_priv( std::make_unique<priv_data>() )
{
}

UvDispatcher::~UvDispatcher(){
}

std::shared_ptr<UvDispatcher> UvDispatcher::create(){
    return std::shared_ptr<UvDispatcher>( new UvDispatcher() );
}

std::shared_ptr<DBus::Connection> UvDispatcher::create_connection( DBus::BusType type ){
    std::shared_ptr<DBus::Connection> conn = DBus::Connection::create( type );
    if( conn->bus_register() && add_connection( conn ) ){
        return conn;
    }
    return std::shared_ptr<DBus::Connection>();
}

std::shared_ptr<DBus::Connection> UvDispatcher::create_connection( std::string address ){
    std::shared_ptr<DBus::Connection> conn = DBus::Connection::create( address );
    if( conn->bus_register() && add_connection( conn ) ){
        return conn;
    }
    return std::shared_ptr<DBus::Connection>();
}

bool UvDispatcher::add_connection( std::shared_ptr<DBus::Connection> connection ){
    if( !connection ){
        return false;
    }

    try {
        m_priv->m_connections.emplace_back(connection);
        return true;
    }
    catch (std::system_error e) {
        SIMPLELOGGER_ERROR( LOGGER_NAME, "add_connection failed: " << e.what() << ", " << e.code() );
        return false;
    }
}

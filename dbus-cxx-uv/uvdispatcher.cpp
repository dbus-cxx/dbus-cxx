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
poll_cb(uv_poll_t* h, int status, int event)
{
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
    } else if (event & (UV_READABLE | UV_WRITABLE)) {
        auto conn = reinterpret_cast<DBus::Connection*>(h->data);
        while(true) {
            if(auto dispatch_status = conn->dispatch(); dispatch_status == DBus::DispatchStatus::COMPLETE ) {
                break;
            }
        }
    }
}

namespace
{
class UVPollConnection
{
public:
    UVPollConnection(std::shared_ptr<DBus::Connection> connection) : connection(connection)
    {
        uv_poll = new uv_poll_t;

        // uv_poll must be a naked pointer because the final
        // uv_close() call triggered in the destructor completes
        // ansynchronosuly; deletion is performed in the close
        // callback.
        if (int r = uv_poll_init(uv_default_loop(), uv_poll, connection->unix_fd()); r < 0) {
            throw std::system_error(r, std::system_category(), "uv_poll_init");
        }

        uv_poll->data = connection.get();

        if (int r = uv_poll_start(uv_poll, UV_READABLE | UV_WRITABLE, poll_cb); r < 0) {
            throw std::system_error(r, std::system_category(), "uv_poll_start");
        }
    }

    virtual ~UVPollConnection()
    {
        uv_poll_stop(uv_poll);
        // Note: libuv owns the uv_poll allocation until the close
        // callback completes.
        uv_close((uv_handle_t*)uv_poll, close_cb);
    }

private:
    uv_poll_t* uv_poll = nullptr;
    std::shared_ptr<DBus::Connection> connection;
};
} // namespace

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
        SIMPLELOGGER_ERROR( LOGGER_NAME, "connection failed: " << e.what() << ", " << e.code() );
        return false;
    }
}

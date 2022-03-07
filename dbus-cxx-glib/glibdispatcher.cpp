// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
/***************************************************************************
 *   Copyright (C) 2007,2008,2009,2010 by Rick L. Vinyard, Jr.             *
 *   rvinyard@cs.nmsu.edu                                                  *
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
#include "glibdispatcher.h"

#include <dbus-cxx/connection.h>
#include <dbus-cxx/dbus-cxx-private.h>
#include <vector>
#include <map>
#include <glib.h>

static const char* LOGGER_NAME = "DBus.GLib.GLibDispatcher";

using DBus::GLib::GLibDispatcher;

class GLibDispatcher::priv_data {
public:
    std::map<GIOChannel*, std::shared_ptr<Connection>> m_channelToConnection;
};

GLibDispatcher::GLibDispatcher() :
    m_priv( std::make_unique<priv_data>() ){

}

GLibDispatcher::~GLibDispatcher(){
    for( auto const& [key,val] : m_priv->m_channelToConnection ){
        g_io_channel_unref( key );
    }
}

std::shared_ptr<GLibDispatcher> GLibDispatcher::create(){
    return std::shared_ptr<GLibDispatcher>( new GLibDispatcher() );
}

std::shared_ptr<DBus::Connection> GLibDispatcher::create_connection( BusType type ){
    std::shared_ptr<DBus::Connection> conn = DBus::Connection::create( type );
    if( conn->bus_register() && add_connection( conn ) ){
        return conn;
    }
    return std::shared_ptr<DBus::Connection>();
}

std::shared_ptr<DBus::Connection> GLibDispatcher::create_connection( std::string address ){
    std::shared_ptr<DBus::Connection> conn = DBus::Connection::create( address );
    if( conn->bus_register() && add_connection( conn ) ){
        return conn;
    }
    return std::shared_ptr<DBus::Connection>();
}

bool GLibDispatcher::add_connection( std::shared_ptr<Connection> connection ){
    if( !connection ){
        return false;
    }

    GIOChannel* newChannel = g_io_channel_unix_new( connection->unix_fd() );
    m_priv->m_channelToConnection[ newChannel ] = connection;
    guint sourceId = g_io_add_watch( newChannel, G_IO_IN, &GLibDispatcher::channel_data_cb, this );

    SIMPLELOGGER_TRACE( LOGGER_NAME, "Adding connection" );
    return true;
}

gboolean GLibDispatcher::channel_has_data(GIOChannel* channel, GIOCondition condition ){
    std::shared_ptr<Connection> conn = m_priv->m_channelToConnection[ channel ];
    DBus::DispatchStatus status;

    SIMPLELOGGER_TRACE( LOGGER_NAME, "channel has data" );

    if( !conn ){
        return FALSE;
    }

    do{
        status = conn->dispatch();
    }while( status != DBus::DispatchStatus::COMPLETE );

    return TRUE;
}

gboolean GLibDispatcher::channel_data_cb(GIOChannel* channel, GIOCondition condition, gpointer data ){
    GLibDispatcher* disp = static_cast<GLibDispatcher*>( data );

    if( !disp ){
        return FALSE;
    }

    return disp->channel_has_data( channel, condition );
}

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
#include <dbus-cxx/dispatcher.h>
#include <glib.h>

#ifndef DBUSCXX_GLIB_DISPATCHER_H
#define DBUSCXX_GLIB_DISPATCHER_H

namespace DBus
{
namespace GLib {

/**
 * Does all dispatching in the main GLib context.
 */
class GLibDispatcher : public ::DBus::Dispatcher
{
private:
    GLibDispatcher();

public:
    ~GLibDispatcher();

    static std::shared_ptr<GLibDispatcher> create();

    std::shared_ptr<Connection> create_connection( BusType type );

    std::shared_ptr<Connection> create_connection( std::string address );

    bool add_connection( std::shared_ptr<Connection> connection );

private:
    gboolean channel_has_data(GIOChannel* channel, GIOCondition condition );
    static gboolean channel_data_cb(GIOChannel* channel, GIOCondition condition, gpointer data );

private:
    class priv_data;

    DBUS_CXX_PROPAGATE_CONST(std::unique_ptr<priv_data>) m_priv;
};

} /* namespace GLib */
} /* namespace DBus */

#endif

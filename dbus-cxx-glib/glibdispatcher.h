// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
/***************************************************************************
 *   Copyright (C) 2007,2008,2009,2010 by Rick L. Vinyard, Jr.             *
 *   rvinyard@cs.nmsu.edu                                                  *
 *   Copyright (C) 2020 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
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

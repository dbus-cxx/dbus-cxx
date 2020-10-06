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
#include "objectpathhandler.h"
#include "connection.h"
#include "dbus-cxx-config.h"
#include "dbus-cxx-private.h"
#include "error.h"
#include "message.h"
namespace sigc { template <typename T_return, typename ...T_arg> class signal; }

namespace DBus {

class ObjectPathHandler::priv_data {
public:
    priv_data() {}

    std::weak_ptr<Connection> m_connection;
    Path m_path;
    sigc::signal<void( std::shared_ptr<Connection> ) > m_signal_registered;
    sigc::signal<void( std::shared_ptr<Connection> ) > m_signal_unregistered;
};

ObjectPathHandler::ObjectPathHandler( const std::string& path ):
    m_priv( std::make_unique<priv_data>() ) {
    m_priv->m_path = path;
}

ObjectPathHandler::~ObjectPathHandler() {
}

const Path& ObjectPathHandler::path() const {
    return m_priv->m_path;
}

std::weak_ptr< Connection > ObjectPathHandler::connection() const {
    return m_priv->m_connection;
}

bool ObjectPathHandler::register_with_connection( std::shared_ptr<Connection> conn ) {
    m_priv->m_connection = conn;

    return true;
}

bool ObjectPathHandler::unregister() {
    std::shared_ptr connection = m_priv->m_connection.lock();

    if( !connection ) { return true; }

    return connection->unregister_object( m_priv->m_path );
}

sigc::signal< void( std::shared_ptr<Connection> )>& ObjectPathHandler::signal_registered() {
    return m_priv->m_signal_registered;
}

sigc::signal< void( std::shared_ptr<Connection> )>& ObjectPathHandler::signal_unregistered() {
    return m_priv->m_signal_unregistered;
}

void ObjectPathHandler::set_connection( std::shared_ptr<Connection> conn ) {
    unregister();
    m_priv->m_connection = conn;
}

}


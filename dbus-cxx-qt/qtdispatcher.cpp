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
#include <QMap>
#include <QVector>
#include <QSocketNotifier>
#include <dbus-cxx/connection.h>

#include "qtdispatcher.h"

using DBus::Qt::QtDispatcher;

class QtDispatcher::priv_data {
public:
    QMap<int,std::shared_ptr<DBus::Connection>> m_fdToConnection;
    QVector<std::shared_ptr<QSocketNotifier>> m_socketNotifiers;
};

QtDispatcher::QtDispatcher() :
    QObject( nullptr ),
    m_priv( std::make_unique<priv_data>() )
{

}

QtDispatcher::~QtDispatcher(){

}

std::shared_ptr<QtDispatcher> QtDispatcher::create(){
    return std::shared_ptr<QtDispatcher>( new QtDispatcher() );
}

std::shared_ptr<DBus::Connection> QtDispatcher::create_connection( DBus::BusType type ){
    std::shared_ptr<DBus::Connection> conn = DBus::Connection::create( type );
    if( conn->bus_register() && add_connection( conn ) ){
        return conn;
    }
    return std::shared_ptr<DBus::Connection>();
}

std::shared_ptr<DBus::Connection> QtDispatcher::create_connection( std::string address ){
    std::shared_ptr<DBus::Connection> conn = DBus::Connection::create( address );
    if( conn->bus_register() && add_connection( conn ) ){
        return conn;
    }
    return std::shared_ptr<DBus::Connection>();
}

bool QtDispatcher::add_connection( std::shared_ptr<DBus::Connection> connection ){
    if( !connection ){
        return false;
    }

    int fd = connection->unix_fd();
    m_priv->m_fdToConnection[ fd ] = connection;

    // Now add in the socket notifier so that when data comes in we will dispatch the connection
    std::shared_ptr<QSocketNotifier> socketNotify = std::make_shared<QSocketNotifier>( fd, QSocketNotifier::Read );
    m_priv->m_socketNotifiers.push_back( socketNotify );

    connect( socketNotify.get(), &QSocketNotifier::activated,
             this, &QtDispatcher::activated );

    return true;
}

void QtDispatcher::activated( int fd ){
    std::shared_ptr<DBus::Connection> conn = m_priv->m_fdToConnection[ fd ];
    DBus::DispatchStatus status;

    if( !conn ){
        return;
    }

    do{
        status = conn->dispatch();
    }while( status != DBus::DispatchStatus::COMPLETE );
}

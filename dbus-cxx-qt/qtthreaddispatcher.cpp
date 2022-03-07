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
#include <QVector>
#include <QMutex>
#include <QQueue>
#include <QMutexLocker>
#include <dbus-cxx/signalproxy.h>
#include <dbus-cxx/object.h>

#include "qtthreaddispatcher.h"

using DBus::Qt::QtThreadDispatcher;

struct ObjectAndMessage{
    std::shared_ptr<DBus::Object> object;
    std::shared_ptr<const DBus::CallMessage> message;
};

class QtThreadDispatcher::priv_data{
public:
    QMutex m_objMessageMutex;
    QQueue<ObjectAndMessage> m_objMessageQueue;
    QMutex m_signalHandlerMutex;
    QVector<std::shared_ptr<DBus::SignalProxyBase>> m_signalHandlers;
    QMutex m_signalsMutex;
    QQueue<std::shared_ptr<const SignalMessage>> m_signalsQueue;
};

QtThreadDispatcher::QtThreadDispatcher() :
    QObject( nullptr ),
    m_priv( std::make_unique<priv_data>() ){
    connect( this, &QtThreadDispatcher::notifyMainThread,
             this, &QtThreadDispatcher::sendMessages,
             ::Qt::QueuedConnection );
}

QtThreadDispatcher::~QtThreadDispatcher(){

}

std::shared_ptr<QtThreadDispatcher> QtThreadDispatcher::create(){
    return std::shared_ptr<QtThreadDispatcher>( new QtThreadDispatcher() );
}

void QtThreadDispatcher::add_message( std::shared_ptr<Object> object, std::shared_ptr<const CallMessage> message ){
    struct ObjectAndMessage objMsg;

    objMsg.object = object;
    objMsg.message = message;

    QMutexLocker lock( &m_priv->m_objMessageMutex );
    m_priv->m_objMessageQueue.push_back( objMsg );

    Q_EMIT notifyMainThread();
}

void QtThreadDispatcher::add_signal_proxy( std::shared_ptr<SignalProxyBase> handler ){
    QMutexLocker lock( &m_priv->m_objMessageMutex );
    m_priv->m_signalHandlers.push_back( handler );
}

bool QtThreadDispatcher::remove_signal_proxy( std::shared_ptr<SignalProxyBase> handler ){
    QMutexLocker lock( &m_priv->m_objMessageMutex );
    QVector<std::shared_ptr<DBus::SignalProxyBase>>::iterator it;

    it = std::find( m_priv->m_signalHandlers.begin(), m_priv->m_signalHandlers.end(), handler );
    if( it != m_priv->m_signalHandlers.end() ){
        m_priv->m_signalHandlers.erase( it );
        return true;
    }

    return false;
}

void QtThreadDispatcher::add_signal( std::shared_ptr<const SignalMessage> message ){
    QMutexLocker lock( &m_priv->m_signalsMutex );

    m_priv->m_signalsQueue.push_back( message );

    Q_EMIT notifyMainThread();
}

void QtThreadDispatcher::sendMessages(){
    {
        QMutexLocker lock( &m_priv->m_objMessageMutex );

        while( m_priv->m_objMessageQueue.size() ){
            ObjectAndMessage objMsg = m_priv->m_objMessageQueue.front();
            m_priv->m_objMessageQueue.pop_front();

            objMsg.object->handle_message( objMsg.message );
        }
    }

    {
        QMutexLocker lock( &m_priv->m_signalsMutex );

        while( m_priv->m_signalsQueue.size() ){
            std::shared_ptr<const SignalMessage> signal = m_priv->m_signalsQueue.front();
            m_priv->m_signalsQueue.pop_front();

            for( std::shared_ptr<DBus::SignalProxyBase> proxy : m_priv->m_signalHandlers ){
                proxy->handle_signal( signal );
            }
        }
    }
}

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
#include "dbus-cxx-qt.h"

using DBus::Qt::QtThreadDispatcher;

class QtThreadDispatcher::priv_data{
public:

};

QtThreadDispatcher::QtThreadDispatcher() :
    QObject( nullptr ),
    m_priv( std::make_unique<priv_data>() ){
}

QtThreadDispatcher::~QtThreadDispatcher(){

}

std::shared_ptr<QtThreadDispatcher> QtThreadDispatcher::create(){
    return std::shared_ptr<QtThreadDispatcher>( new QtThreadDispatcher() );
}

void QtThreadDispatcher::add_message( std::shared_ptr<ObjectPathHandler> object, std::shared_ptr<const CallMessage> message ){

}

void QtThreadDispatcher::add_signal_proxy( std::shared_ptr<signal_proxy_base> handler ){

}

bool QtThreadDispatcher::remove_signal_proxy( std::shared_ptr<signal_proxy_base> handler ){

}

void QtThreadDispatcher::add_signal( std::shared_ptr<const SignalMessage> message ){

}

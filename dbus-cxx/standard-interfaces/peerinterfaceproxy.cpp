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
#include "peerinterfaceproxy.h"

using DBus::PeerInterfaceProxy;

PeerInterfaceProxy::PeerInterfaceProxy() :
    DBus::InterfaceProxy( DBUS_CXX_PEER_INTERFACE ){
    m_ping_method = this->create_method<void()>( "Ping" );
    m_get_machine_method = this->create_method<std::string()>( "GetMachineId" );
}

std::shared_ptr<PeerInterfaceProxy> PeerInterfaceProxy::create(){
    return std::shared_ptr<PeerInterfaceProxy>( new PeerInterfaceProxy() );
}


void PeerInterfaceProxy::Ping(){
    (*m_ping_method)();
}

std::string PeerInterfaceProxy::GetMachineId(){
    return (*m_get_machine_method)();
}

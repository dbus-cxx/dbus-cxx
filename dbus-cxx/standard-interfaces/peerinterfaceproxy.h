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
#ifndef DBUSCXX_PEER_INTERFACE_PROXY_H
#define DBUSCXX_PEER_INTERFACE_PROXY_H

#include "../interfaceproxy.h"
#include <memory>

namespace DBus{

class PeerInterfaceProxy : public InterfaceProxy {
private:
    PeerInterfaceProxy();

public:
    static std::shared_ptr<PeerInterfaceProxy> create();

    void Ping();

    std::string GetMachineId();

private:
    std::shared_ptr<MethodProxy<void()>> m_ping_method;
    std::shared_ptr<MethodProxy<std::string()>> m_get_machine_method;
};


}

#endif /* DBUSCXX_PEER_INTERFACE_PROXY_H */

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
#ifndef DBUSCXX_PROPERTIES_INTERFACE_PROXY_H
#define DBUSCXX_PROPERTIES_INTERFACE_PROXY_H

#include "../interfaceproxy.h"
#include <memory>
#include <string>
#include <map>

namespace DBus{

class Variant;

class PropertiesInterfaceProxy : public InterfaceProxy {
private:
    PropertiesInterfaceProxy();

public:
    static std::shared_ptr<PropertiesInterfaceProxy> create();

    Variant Get( std::string interface_name, std::string property_name );

    void Set( std::string interface_name, std::string property_name, Variant property_value );

    std::map<std::string,Variant> GetAll( std::string interface_name );

private:
    std::shared_ptr<MethodProxy<Variant(std::string,std::string)>> m_get_method;
    std::shared_ptr<MethodProxy<void(std::string,std::string,Variant)>> m_set_method;
    std::shared_ptr<MethodProxy<std::map<std::string,Variant>(std::string)>> m_getall_method;
};

}

#endif /* DBUSCXX_PROPERTIES_INTERFACE_PROXY_H */

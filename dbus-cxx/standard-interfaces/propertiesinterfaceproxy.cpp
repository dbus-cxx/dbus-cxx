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
#include "propertiesinterfaceproxy.h"
#include "../variant.h"

using DBus::PropertiesInterfaceProxy;

PropertiesInterfaceProxy::PropertiesInterfaceProxy() :
    InterfaceProxy ( DBUS_CXX_PROPERTIES_INTERFACE ){
    m_get_method = this->create_method<Variant(std::string,std::string)>( "Get" );
    m_set_method = this->create_method<void(std::string,std::string,Variant)>( "Set" );
    m_getall_method = this->create_method<std::map<std::string,Variant>(std::string)>( "GetAll" );
}


std::shared_ptr<PropertiesInterfaceProxy> PropertiesInterfaceProxy::create(){
    return std::shared_ptr<PropertiesInterfaceProxy>( new PropertiesInterfaceProxy() );
}

DBus::Variant PropertiesInterfaceProxy::Get( std::string interface_name, std::string property_name ){
    return (*m_get_method)( interface_name, property_name );
}

void PropertiesInterfaceProxy::Set( std::string interface_name, std::string property_name, Variant property_value ){
    return (*m_set_method)( interface_name, property_name, property_value );
}

std::map<std::string,DBus::Variant> PropertiesInterfaceProxy::PropertiesInterfaceProxy::GetAll( std::string interface_name ){
    return (*m_getall_method)( interface_name );
}

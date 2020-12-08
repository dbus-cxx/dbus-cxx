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
#include "node-info.h"

NodeInfo::NodeInfo(){

}

NodeInfo::NodeInfo( std::string name ) :
    m_name( name ){

}

std::string NodeInfo::name() const{
    return m_name;
}

void NodeInfo::setCppname( std::string cppname ){
    m_cppname = cppname;
}

std::string NodeInfo::cppname() const{
    return m_cppname;
}

void NodeInfo::setNamespace( std::string gen_namespace ){
    m_namespace = gen_namespace;
}

std::string NodeInfo::genNamespace() const{
    return m_namespace;
}

void NodeInfo::setDest( std::string dest ){
    m_dest = dest;
}

std::string NodeInfo::destination() const{
    return m_dest;
}

void NodeInfo::addInterfaceInfo( InterfaceInfo i ){
    m_interfaces.push_back( i );
}

std::vector<InterfaceInfo> NodeInfo::interfaces() const{
    return m_interfaces;
}

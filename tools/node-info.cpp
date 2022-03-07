/***************************************************************************
 *   Copyright (C) 2020 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
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

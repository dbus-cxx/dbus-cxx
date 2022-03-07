/***************************************************************************
 *   Copyright (C) 2020 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 ***************************************************************************/
#include "interface-info.h"

InterfaceInfo::InterfaceInfo(){

}

InterfaceInfo::InterfaceInfo( std::string name ) :
    m_name( name ){

}

std::string InterfaceInfo::name() const{
    return m_name;
}

void InterfaceInfo::setCppname( std::string cppname ){
    m_cppname = cppname;
}

std::string InterfaceInfo::cppname() const{
    if( m_cppname.length() ){
        return m_cppname;
    }

    return m_name;
}

void InterfaceInfo::addMethodInfo( MethodInfo m ){
    m_methods.push_back( m );
}

std::vector<MethodInfo> InterfaceInfo::methods() const{
    return m_methods;
}

void InterfaceInfo::addPropertyInfo( PropertyInfo p ){
    m_properties.push_back( p );
}

std::vector<PropertyInfo> InterfaceInfo::properties() const{
    return m_properties;
}

void InterfaceInfo::addSignalInfo( SignalInfo s ){
    m_signals.push_back( s );
}

std::vector<SignalInfo> InterfaceInfo::signals() const{
    return m_signals;
}

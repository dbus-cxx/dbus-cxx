/***************************************************************************
 *   Copyright (C) 2020 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 ***************************************************************************/
#include "property-info.h"

PropertyInfo::PropertyInfo(){

}

PropertyInfo::PropertyInfo( std::string name, std::string cpptype, std::string access ) :
    m_name( name ),
    m_cpptype( cpptype ),
    m_access( access ){

}

std::string PropertyInfo::name() const{
    return m_name;
}

std::string PropertyInfo::cpptype() const{
    return m_cpptype;
}

std::string PropertyInfo::access() const{
    return m_access;
}

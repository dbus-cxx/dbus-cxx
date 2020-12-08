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
#include "method-info.h"

MethodInfo::MethodInfo(){}

MethodInfo::MethodInfo( std::string name ) :
    m_name( name ),
    m_returnType( "void" )
{}

void MethodInfo::addArgument( cppgenerate::Argument arg ){
    m_arguments.push_back( arg );
}

std::vector<cppgenerate::Argument> MethodInfo::arguments() const {
    return m_arguments;
}

std::string MethodInfo::name() const {
    return m_name;
}

std::string MethodInfo::returnType() const{
    return m_returnType;
}

void MethodInfo::setReturnType( std::string type ){
    m_returnType = type;
}

std::string MethodInfo::returnArgName() const{
    return m_returnArgName;
}

void MethodInfo::setReturnArgName( std::string name ){
    m_returnArgName = name;
}

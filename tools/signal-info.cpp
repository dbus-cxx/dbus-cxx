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
#include "signal-info.h"

SignalInfo::SignalInfo(){}

SignalInfo::SignalInfo( std::string name ) :
	m_name( name )
{}

void SignalInfo::addArgument( cppgenerate::Argument arg ){
	m_arguments.push_back( arg );
}

std::vector<cppgenerate::Argument> SignalInfo::arguments() const {
	return m_arguments;
}

std::string SignalInfo::name() const {
	return m_name;
}

void SignalInfo::addIncludes( std::vector<std::string> includes ){
    for( std::string include : includes ){
        m_includes.push_back( include );
    }
}

std::vector<std::string> SignalInfo::includes() const{
    return m_includes;
}

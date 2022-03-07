/***************************************************************************
 *   Copyright (C) 2020 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
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

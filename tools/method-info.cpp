/***************************************************************************
 *   Copyright (C) 2020 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 ***************************************************************************/
#include "method-info.h"
#include <sstream>
#include <iterator>

MethodInfo::MethodInfo(){}

MethodInfo::MethodInfo( std::string name ) :
    m_name( name )
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
    if(m_returnType.size() > 1){
	    std::stringstream returnType;
	    returnType << "DBus::MultipleReturn<";
	    std::copy(m_returnType.begin(), m_returnType.end()-1, std::ostream_iterator<std::string>(returnType, ","));
	    returnType << m_returnType.back();
	    returnType << ">";
	    return returnType.str();
    }
    else if(m_returnType.size() == 1){
      return m_returnType.front();
    }
    else{
	    return "void";
    }
}

void MethodInfo::addReturnValue( std::string type ){
    m_returnType.push_back(type);
}

std::string MethodInfo::returnArgName() const{
    return m_returnArgName;
}

void MethodInfo::setReturnArgName( std::string name ){
    m_returnArgName = name;
}

void MethodInfo::addIncludes( std::vector<std::string> includes ){
    for( std::string include : includes ){
        m_includes.push_back( include );
    }
}

std::vector<std::string> MethodInfo::includes() const{
    return m_includes;
}

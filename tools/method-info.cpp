/***************************************************************************
 *   Copyright (C) 2020 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
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

void MethodInfo::addIncludes( std::vector<std::string> includes ){
    for( std::string include : includes ){
        m_includes.push_back( include );
    }
}

std::vector<std::string> MethodInfo::includes() const{
    return m_includes;
}

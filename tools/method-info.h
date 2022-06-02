// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
/***************************************************************************
 *   Copyright (C) 2020 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 ***************************************************************************/
#ifndef METHOD_INFO_H
#define METHOD_INFO_H

#include <string>
#include <vector>
#include <cppgenerate/argument.h>

/**
 * Represents parsed information from the introspection XML on a DBus method
 */
class MethodInfo {
public:
        MethodInfo();

        MethodInfo( std::string name );

        void addArgument( cppgenerate::Argument arg );

        std::vector<cppgenerate::Argument> arguments() const;

        std::string name() const;

        std::string returnType() const;

        void addReturnValue( std::string type );

        std::string returnArgName() const;

        void setReturnArgName( std::string name );

        void addIncludes( std::vector<std::string> includes );

        std::vector<std::string> includes() const;

private:
        std::string m_name;
        std::vector<cppgenerate::Argument> m_arguments;
        std::vector<std::string> m_returnType;
        std::string m_returnArgName;
        std::vector<std::string> m_includes;
};

#endif

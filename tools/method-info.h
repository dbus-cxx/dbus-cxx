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

        void setReturnType( std::string type );

        std::string returnArgName() const;

        void setReturnArgName( std::string name );

private:
        std::string m_name;
        std::vector<cppgenerate::Argument> m_arguments;
        std::string m_returnType;
        std::string m_returnArgName;
};

#endif

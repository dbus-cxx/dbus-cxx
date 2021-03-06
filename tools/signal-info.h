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
#ifndef SIGNAL_INFO_H
#define SIGNAL_INFO_H

#include <string>
#include <vector>
#include <cppgenerate/argument.h>

/**
 * Represents parsed information from the introspection XML on a DBus signal
 */
class SignalInfo {
public:
	SignalInfo();

	SignalInfo( std::string name );

	void addArgument( cppgenerate::Argument arg );

	std::vector<cppgenerate::Argument> arguments() const;

	std::string name() const;

    void addIncludes( std::vector<std::string> includes );

    std::vector<std::string> includes() const;

private:
	std::string m_name;
	std::vector<cppgenerate::Argument> m_arguments;
    std::vector<std::string> m_includes;
};

#endif

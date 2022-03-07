// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
/***************************************************************************
 *   Copyright (C) 2020 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
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

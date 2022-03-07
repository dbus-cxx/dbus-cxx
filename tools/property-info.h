// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
/***************************************************************************
 *   Copyright (C) 2020 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 ***************************************************************************/
#ifndef PROPERTY_INFO_H
#define PROPERTY_INFO_H

#include <string>
#include <vector>

/**
 * Represents parsed information from the introspection XML on a DBus property
 */
class PropertyInfo {
public:
        PropertyInfo();

        PropertyInfo( std::string name, std::string cpptype, std::string access );

        std::string name() const;

        std::string cpptype() const;

        std::string access() const;

private:
        std::string m_name;
        std::string m_cpptype;
        std::string m_access;
};

#endif

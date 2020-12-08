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

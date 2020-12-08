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
#ifndef NODE_INFO_H
#define NODE_INFO_H

#include <string>
#include <vector>

#include "interface-info.h"

/**
 * Represents parsed information from the introspection XML on a DBus node
 */
class NodeInfo {
public:
        NodeInfo();

        NodeInfo( std::string name );

        std::string name() const;

        void setCppname( std::string cppname );

        std::string cppname() const;

        void setNamespace( std::string gen_namespace );

        std::string genNamespace() const;

        void setDest( std::string dest );

        std::string destination() const;

        void addInterfaceInfo( InterfaceInfo i );

        std::vector<InterfaceInfo> interfaces() const;

private:
        std::string m_name;
        std::string m_cppname;
        std::string m_namespace;
        std::string m_dest;
        std::vector<InterfaceInfo> m_interfaces;
};

#endif

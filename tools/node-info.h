// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
/***************************************************************************
 *   Copyright (C) 2020 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
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

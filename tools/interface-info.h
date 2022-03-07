// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
/***************************************************************************
 *   Copyright (C) 2020 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 ***************************************************************************/
#ifndef INTERFACE_INFO_H
#define INTERFACE_INFO_H

#include <string>
#include <vector>

#include "method-info.h"
#include "property-info.h"
#include "signal-info.h"

/**
 * Represents parsed information from the introspection XML on a DBus intefface
 */
class InterfaceInfo {
public:
        InterfaceInfo();

        InterfaceInfo( std::string name );

        std::string name() const;

        void setCppname( std::string cppname );

        std::string cppname() const;

        void addMethodInfo( MethodInfo m );

        std::vector<MethodInfo> methods() const;

        void addPropertyInfo( PropertyInfo p );

        std::vector<PropertyInfo> properties() const;

        void addSignalInfo( SignalInfo s );

        std::vector<SignalInfo> signals() const;

private:
        std::string m_name;
        std::string m_cppname;
        std::vector<MethodInfo> m_methods;
        std::vector<PropertyInfo> m_properties;
        std::vector<SignalInfo> m_signals;
};

#endif

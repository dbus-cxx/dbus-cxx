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
#include <dbus-cxx/property.h>
#include <dbus-cxx/variant.h>
#include <dbus-cxx/interface.h>
#include "property.h"

using DBus::PropertyBase;

class PropertyBase::priv_data {
public:
    priv_data( std::string name, PropertyAccess access, PropertyUpdateType update ) :
        m_name( name ),
        m_propertyUpdate( update ),
        m_interface( nullptr ),
        m_access( access )
    {}

    std::string m_name;
    PropertyUpdateType m_propertyUpdate;
    sigc::signal<void(DBus::Variant)> m_propertyChangedSignal;
    Interface* m_interface;
    Variant m_value;
    PropertyAccess m_access;
};

PropertyBase::PropertyBase( std::string name, PropertyAccess access, PropertyUpdateType update ) :
    m_priv( std::make_unique<priv_data>( name, access, update ) ) {

}

PropertyBase::~PropertyBase(){

}

std::string PropertyBase::name() const {
    return m_priv->m_name;
}

DBus::Variant PropertyBase::variant_value() const {
    return m_priv->m_value;
}

DBus::PropertyUpdateType PropertyBase::update_type() const {
    return m_priv->m_propertyUpdate;
}

void PropertyBase::set_value( DBus::Variant value ) {
    m_priv->m_value = value;
}

DBus::PropertyAccess PropertyBase::access_type() const {
    return m_priv->m_access;
}

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
#include <dbus-cxx/propertyproxy.h>
#include <dbus-cxx/variant.h>
#include <dbus-cxx/interfaceproxy.h>
#include "property.h"

using DBus::PropertyProxyBase;

class PropertyProxyBase::priv_data {
public:
    priv_data( std::string name, PropertyUpdateType update) :
        m_name( name ),
        m_propertyUpdate( update ),
        m_interface( nullptr )
    {}

    std::string m_name;
    PropertyUpdateType m_propertyUpdate;
    sigc::signal<void(DBus::Variant)> m_propertyChangedSignal;
    InterfaceProxy* m_interface;
    Variant m_value;
};

PropertyProxyBase::PropertyProxyBase( std::string name, PropertyUpdateType update ) :
    m_priv( std::make_unique<priv_data>( name, update ) ) {

}

std::string PropertyProxyBase::name() const {
    return m_priv->m_name;
}

DBus::Variant PropertyProxyBase::variant_value() const {

}

DBus::PropertyUpdateType PropertyProxyBase::update_type() const {
    return m_priv->m_propertyUpdate;
}

sigc::signal<void(DBus::Variant)> PropertyProxyBase::signal_generic_property_changed() {
    return m_priv->m_propertyChangedSignal;
}

void PropertyProxyBase::set_value( DBus::Variant value ) {

}

void PropertyProxyBase::set_interface( InterfaceProxy *proxy ){
    m_priv->m_interface = proxy;
}

DBus::InterfaceProxy* PropertyProxyBase::interface_name() const {
    return m_priv->m_interface;
}

void PropertyProxyBase::updated_value(Variant value){
    m_priv->m_value = value;
}

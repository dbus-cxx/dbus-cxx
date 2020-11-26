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
#include <dbus-cxx/enums.h>
#include <dbus-cxx/dbus-cxx-config.h>
#include <dbus-cxx/variant.h>
#include <dbus-cxx/signature.h>
#include <dbus-cxx/utility.h>
#include <sigc++/sigc++.h>
#include <memory>
#include <sstream>

#ifndef DBUSCXX_PROPERTY_H
#define DBUSCXX_PROPERTY_H

namespace DBus {

/**
 * Base type of Property to allow for storage in e.g. a vector.
 */
class PropertyBase {
protected:
    PropertyBase( std::string name, PropertyAccess access, PropertyUpdateType update );

    ~PropertyBase();

public:

    /**
     * Get the name of this propery.
     * @return
     */
    std::string name() const;

    /**
     * Get the value of this property as a Variant.
     *
     * @return
     */
    Variant variant_value() const;

    PropertyUpdateType update_type() const;

    PropertyAccess access_type() const;

    /**
     * Set the value of this property.
     *
     * When used on a remote property(a proxy), this will attempt to set
     * the value on the remote object.  If the property is READONLY, this
     * acts as a No-op.
     *
     * When used on a local property(adapter), this will emit the PropertyChanged
     * DBus signal in order to notify clients that the property has updated.
     * Note that the exact value of the PropertyUpdateType will determine what is
     * emitted(invalidated, new value, or invalidation)
     *
     * @param value The new value to set
     */
    void set_value( Variant value );

    virtual std::string introspect( int spaces ){ return std::string(); }

private:
    class priv_data;

    DBUS_CXX_PROPAGATE_CONST( std::unique_ptr<priv_data> ) m_priv;
};

/**
 * Represents a local DBus property.
 *
 * Properties can be Read, Write, or Readonly.  When they change, a signal
 * may be emitted on the bus to listeners.
 */
template <typename T_type>
class Property : public PropertyBase {
private:
    Property( std::string name, PropertyAccess access, PropertyUpdateType update ) :
        PropertyBase( name, access, update ) {}

public:
    static std::shared_ptr<Property<T_type>> create( std::string name, PropertyAccess access, PropertyUpdateType update ) {
        return std::shared_ptr<Property<T_type>>( new Property<T_type>( name, access, update ) );
    }

    void set_value( T_type t ) {
        PropertyBase::set_value( Variant( t ) );
    }

    T_type value() const {
        T_type t = variant_value();
        return t;
    }

    virtual std::string introspect( int space_depth ) {
        std::ostringstream sout;
        std::string spaces;
        DBus::priv::dbus_signature<T_type> sig;

        for( int i = 0; i < space_depth; i++ ) { spaces += " "; }

        sout << spaces << "<property"
             << " name=\"" << name() << "\""
             << " type=\"" << sig.dbus_sig() << "\""
             << " access=\"";
        switch( access_type() ){
        case DBus::PropertyAccess::ReadOnly:
            sout << "read";
            break;
        case DBus::PropertyAccess::WriteOnly:
            sout << "write";
            break;
        case DBus::PropertyAccess::ReadWrite:
            sout << "readwrite";
            break;
        }
        sout << "\" ";

        switch( update_type() ){
        case DBus::PropertyUpdateType::Const:
            sout << DBUS_CXX_PROPERTY_EMITS_CHANGE_SIGNAL_ANNOTATION << "=\"const\"";
            break;
        case DBus::PropertyUpdateType::Invalidates:
            sout << DBUS_CXX_PROPERTY_EMITS_CHANGE_SIGNAL_ANNOTATION << "=\"invalidates\"";
            break;
        case DBus::PropertyUpdateType::DoesNotUpdate:
            sout << DBUS_CXX_PROPERTY_EMITS_CHANGE_SIGNAL_ANNOTATION << "=\"false\"";
            break;
        }
        sout << "/>\n";

        return sout.str();
    }
};

} /* namespace DBus */

#endif /* DBUSCXX_PROPERTY_H */

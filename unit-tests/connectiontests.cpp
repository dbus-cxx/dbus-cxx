// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
/***************************************************************************
 *   Copyright (C) 2019 by Robert Middleton                                *
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
#include <dbus-cxx.h>

#include "test_macros.h"

static std::shared_ptr<DBus::Dispatcher> dispatch;

bool connection_create_int_signal() {
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    std::shared_ptr<DBus::Signal<void(int)>> signal = conn->create_free_signal<void(int)>( "/some/path", "signal.type", "Member" );

    return true;
}

bool connection_create_void_signal() {
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    std::shared_ptr<DBus::Signal<void()>> signal = conn->create_free_signal<void()>( "/some/path", "signal.type", "Member" );

    return true;
}

bool connection_create_signal_proxy() {
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    std::shared_ptr<DBus::SignalProxy<void()>> proxy = conn->create_free_signal_proxy<void()>(
                DBus::MatchRuleBuilder::create()
                .set_interface( "interface.name" )
                .set_member( "myname" )
                .as_signal_match(),
                DBus::ThreadForCalling::DispatcherThread );

    std::vector<std::shared_ptr<DBus::SignalProxyBase>> map = conn->get_free_signal_proxies();
    // Note: there are a few built-in signal proxies that dbus-cxx uses, so this must be at least 1
    TEST_ASSERT_RET_FAIL( map.size() >= 1 );

    std::vector<std::shared_ptr<DBus::SignalProxyBase>> by_iface_name = conn->get_free_signal_proxies( "interface.name" );
    TEST_ASSERT_RET_FAIL( by_iface_name.size() == 1 );

    std::vector<std::shared_ptr<DBus::SignalProxyBase>> by_iface_and_member = conn->get_free_signal_proxies( "interface.name", "myname" );
    TEST_ASSERT_RET_FAIL( by_iface_and_member.size() == 1 );

    return true;
}

bool connection_get_signal_proxy_by_iface() {
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    std::shared_ptr<DBus::SignalProxy<void()>> proxy = conn->create_free_signal_proxy<void()>(
                DBus::MatchRuleBuilder::create()
                .set_interface( "interface.name" )
                .set_member( "myname" )
                .as_signal_match(),
                DBus::ThreadForCalling::DispatcherThread );

    std::vector<std::shared_ptr<DBus::SignalProxyBase>> signals = conn->get_free_signal_proxies( "interface.name" );
    TEST_ASSERT_RET_FAIL( signals.size() == 1 );

    return true;
}

bool connection_get_signal_proxy_by_iface_and_name() {
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    std::shared_ptr<DBus::SignalProxy<void()>> proxy = conn->create_free_signal_proxy<void()>(
                DBus::MatchRuleBuilder::create()
                .set_interface( "interface.name" )
                .set_member( "myname" )
                .as_signal_match(),
                DBus::ThreadForCalling::DispatcherThread );


    std::vector<std::shared_ptr<DBus::SignalProxyBase>> signals = conn->get_free_signal_proxies( "interface.name", "myname" );
    TEST_ASSERT_RET_FAIL( signals.size() == 1 );
    TEST_ASSERT_RET_FAIL( signals.front() == proxy );

    return true;
}

static double add( double a, double b ) {
    return a + b;
}

bool connection_test_method_2arg() {
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    std::shared_ptr<DBus::Object> object = conn->create_object( "/dbuscxx/example/Calculator", DBus::ThreadForCalling::DispatcherThread );

    object->create_method<double( double, double )>( "Calculator.Basic", "add", sigc::ptr_fun( add ) );

    return true;
}

bool connection_remove_object_proxy(){
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    std::weak_ptr<DBus::ObjectProxy> object_weak;
    {
        // Check to make sure that once delete the object proxy, dbus-cxx does not still have a refernce to it
        std::shared_ptr<DBus::ObjectProxy> object = conn->create_object_proxy( "/dbuscxx/example/Calculator", DBus::ThreadForCalling::DispatcherThread );
        object_weak = object;
    }

    if( object_weak.lock() ){
        // removing the object proxy should remove the handling of the object in dbus-cxx,
        // so with no more hard references it should be destroyed at this point.
        return false;
    }
    return true;
}

#define ADD_TEST(name) do{ if( test_name == STRINGIFY(name) ){ \
            ret = connection_##name();\
        } \
    } while( 0 )

int main( int argc, char** argv ) {
    if( argc < 1 ) {
        return 1;
    }

    std::string test_name = argv[1];
    bool ret = false;

    DBus::set_logging_function( DBus::log_std_err );
    DBus::set_log_level( SL_TRACE );
    dispatch = DBus::StandaloneDispatcher::create();

    ADD_TEST( create_signal_proxy );
    ADD_TEST( get_signal_proxy_by_iface );
    ADD_TEST( get_signal_proxy_by_iface_and_name );
    ADD_TEST( create_void_signal );
    ADD_TEST( create_int_signal );
    ADD_TEST( remove_object_proxy );

    return !ret;
}

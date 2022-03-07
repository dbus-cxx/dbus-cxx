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
#include <unistd.h>
#include <iostream>

#include "test_macros.h"

static std::shared_ptr<DBus::Dispatcher> dispatch;
static std::string signal_value;
static int num_rx = 0;

void sigHandle( std::string value ) {
    signal_value = value;
}

void voidSigHandle() {
    num_rx++;
}

bool signal_create() {
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    std::shared_ptr<DBus::Signal<void(std::string)>> signal = conn->create_free_signal<void(std::string)>( "/test/signal", "test.signal.type", "Path" );

    TEST_ASSERT_RET_FAIL( signal );
    return true;
}

bool signal_tx_rx() {
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    std::shared_ptr<DBus::Signal<void(std::string)>> signal = conn->create_free_signal<void(std::string)>( "/test/signal", "test.signal.type", "Path" );
    std::shared_ptr<DBus::SignalProxy<void(std::string)>> proxy = conn->create_free_signal_proxy<void(std::string)>(
                DBus::MatchRuleBuilder::create()
                .set_path( "/test/signal" )
                .set_interface( "test.signal.type" )
                .set_member( "Path" )
                .as_signal_match(),
                DBus::ThreadForCalling::DispatcherThread );

    proxy->connect( sigc::ptr_fun( sigHandle ) );

    signal->emit( "TestSignal" );
    sleep( 1 );

    TEST_ASSERT_RET_FAIL( signal_value.compare( "TestSignal" ) == 0 );
    return true;
}

bool signal_void_txrx() {
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    std::shared_ptr<DBus::Signal<void()>> signal = conn->create_free_signal<void()>( "/test/signal", "test.signal.type", "ExampleMember" );
    std::shared_ptr<DBus::SignalProxy<void()>> proxy = conn->create_free_signal_proxy<void()>(
                DBus::MatchRuleBuilder::create()
                .set_path( "/test/signal" )
                .set_interface( "test.signal.type" )
                .set_member( "ExampleMember" )
                .as_signal_match(),
                DBus::ThreadForCalling::DispatcherThread );

    proxy->connect( sigc::ptr_fun( voidSigHandle ) );

    signal->emit();
    sleep( 1 );

    TEST_ASSERT_RET_FAIL( num_rx == 1 );
    return true;
}

bool signal_path_match_only() {
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    std::shared_ptr<DBus::Signal<void()>> signal = conn->create_free_signal<void()>( "/test/signal", "test.signal.type", "ExampleMember" );
    std::shared_ptr<DBus::SignalProxy<void()>> proxy = conn->create_free_signal_proxy<void()>(
                DBus::MatchRuleBuilder::create()
                .set_path( "/test/signal" )
                .as_signal_match(),
                DBus::ThreadForCalling::DispatcherThread );

    proxy->connect( sigc::ptr_fun( voidSigHandle ) );

    signal->emit();
    sleep( 1 );

    TEST_ASSERT_RET_FAIL( num_rx == 1 );
    return true;
}

bool signal_interface_match_only() {
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    std::shared_ptr<DBus::Signal<void()>> signal = conn->create_free_signal<void()>( "/test/signal", "test.signal.type", "ExampleMember" );
    std::shared_ptr<DBus::SignalProxy<void()>> proxy = conn->create_free_signal_proxy<void()>(
                DBus::MatchRuleBuilder::create()
                .set_interface( "test.signal.type" )
                .as_signal_match(),
                DBus::ThreadForCalling::DispatcherThread );

    proxy->connect( sigc::ptr_fun( voidSigHandle ) );

    signal->emit();
    sleep( 1 );

    TEST_ASSERT_RET_FAIL( num_rx == 1 );
    return true;
}

bool signal_member_match_only() {
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    std::shared_ptr<DBus::Signal<void()>> signal = conn->create_free_signal<void()>( "/test/signal", "test.signal.type", "ExampleMember" );
    std::shared_ptr<DBus::SignalProxy<void()>> proxy = conn->create_free_signal_proxy<void()>(
                DBus::MatchRuleBuilder::create()
                .set_member( "ExampleMember" )
                .as_signal_match(),
                DBus::ThreadForCalling::DispatcherThread );

    proxy->connect( sigc::ptr_fun( voidSigHandle ) );

    signal->emit();
    sleep( 1 );

    TEST_ASSERT_RET_FAIL( num_rx == 1 );
    return true;
}

bool signal_multiple_handlers() {
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    std::shared_ptr<DBus::Signal<void()>> signal = conn->create_free_signal<void()>( "/test/signal", "test.signal.type", "ExampleMember" );
    std::shared_ptr<DBus::SignalProxy<void()>> proxy = conn->create_free_signal_proxy<void()>(
                DBus::MatchRuleBuilder::create()
                .set_path( "/test/signal" )
                .set_interface( "test.signal.type" )
                .set_member( "ExampleMember" )
                .as_signal_match(),
                DBus::ThreadForCalling::DispatcherThread );
    std::shared_ptr<DBus::SignalProxy<void()>> proxy2 = conn->create_free_signal_proxy<void()>(
                DBus::MatchRuleBuilder::create()
                .set_member( "ExampleMember" )
                .as_signal_match(),
                DBus::ThreadForCalling::DispatcherThread );

    proxy->connect( sigc::ptr_fun( voidSigHandle ) );
    proxy2->connect( sigc::ptr_fun( voidSigHandle ) );

    signal->emit();
    sleep( 1 );

    TEST_ASSERT_RET_FAIL( num_rx == 2 );
    return true;
}

bool signal_remove_handler() {
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    std::shared_ptr<DBus::Signal<void()>> signal = conn->create_free_signal<void()>( "/test/signal", "test.signal.type", "ExampleMember" );
    std::shared_ptr<DBus::SignalProxy<void()>> proxy = conn->create_free_signal_proxy<void()>(
                DBus::MatchRuleBuilder::create()
                .set_path( "/test/signal" )
                .set_interface( "test.signal.type" )
                .set_member( "ExampleMember" )
                .as_signal_match(),
                DBus::ThreadForCalling::DispatcherThread );
    std::shared_ptr<DBus::SignalProxy<void()>> proxy2 = conn->create_free_signal_proxy<void()>(
                DBus::MatchRuleBuilder::create()
                .set_member( "ExampleMember" )
                .as_signal_match(),
                DBus::ThreadForCalling::DispatcherThread );

    proxy->connect( sigc::ptr_fun( voidSigHandle ) );
    proxy2->connect( sigc::ptr_fun( voidSigHandle ) );

    signal->emit();
    sleep( 1 );

    TEST_ASSERT_RET_FAIL( num_rx == 2 );

    TEST_ASSERT_RET_FAIL( conn->remove_free_signal_proxy( proxy ) );

    signal->emit();
    sleep( 1 );
    TEST_ASSERT_RET_FAIL( num_rx == 3 );

    return true;
}

#define ADD_TEST(name) do{ if( test_name == STRINGIFY(name) ){ \
            ret = signal_##name();\
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

    ADD_TEST( create );
    ADD_TEST( tx_rx );
    ADD_TEST( void_txrx );
    ADD_TEST( path_match_only );
    ADD_TEST( interface_match_only );
    ADD_TEST( member_match_only );
    ADD_TEST( multiple_handlers );
    ADD_TEST( remove_handler );

    return !ret;
}

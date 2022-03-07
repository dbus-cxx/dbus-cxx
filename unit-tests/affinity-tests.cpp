// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
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
#include <dbus-cxx.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <chrono>

#include "test_macros.h"

static std::shared_ptr<DBus::Dispatcher> dispatch;
static std::thread::id mainThreadId;
static std::thread::id rxThread;
static bool rxSignal = false;
static bool rxMessage = false;

class AffinityThreadDispatcher : public DBus::ThreadDispatcher {
public:
    struct incoming_message {
        std::shared_ptr<DBus::Object> object;
        std::shared_ptr<const DBus::CallMessage> message;
    };

    std::vector<std::shared_ptr<DBus::SignalProxyBase>> m_handlers;
    std::vector<std::shared_ptr<const DBus::SignalMessage>> m_signalMessages;
    std::vector<incoming_message> m_messages;

    void add_message( std::shared_ptr<DBus::Object> object, std::shared_ptr<const DBus::CallMessage> message ) {
        incoming_message incoming;
        incoming.object = object;
        incoming.message = message;

        m_messages.push_back( incoming );
    }

    void add_signal_proxy( std::shared_ptr<DBus::SignalProxyBase> handler ) {
        m_handlers.push_back( handler );
    }

    bool remove_signal_proxy( std::shared_ptr<DBus::SignalProxyBase> handler ) {
        return true;
    }

    void add_signal( std::shared_ptr<const DBus::SignalMessage> message ) {
        m_signalMessages.push_back( message );
    }

    // Call this from the main thread
    void processMessages() {
        for( std::shared_ptr<const DBus::SignalMessage> message : m_signalMessages ) {
            for( std::shared_ptr<DBus::SignalProxyBase> base : m_handlers ) {
                base->handle_signal( message );
            }
        }

        for( incoming_message incoming : m_messages ) {
            incoming.object->handle_message( incoming.message );
        }
    }
};

static void receiveSignal() {
    rxThread = std::this_thread::get_id();
    rxSignal = true;
}

static void receiveMethodCall() {
    rxThread = std::this_thread::get_id();
    rxMessage = true;
}

bool affinity_signal_dispatcher_thread() {
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    std::shared_ptr<DBus::SignalProxy<void()>> proxy = conn->create_free_signal_proxy<void()>(
                DBus::MatchRuleBuilder::create()
                .set_interface( "interface.name" )
                .set_member( "myname" )
                .as_signal_match(),
                DBus::ThreadForCalling::DispatcherThread );

    proxy->connect( sigc::ptr_fun( receiveSignal ) );

    std::shared_ptr<DBus::Signal<void()>> signal = conn->create_free_signal<void()>( "/", "interface.name", "myname" );

    signal->emit();

    std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

    if( rxSignal && ( mainThreadId != rxThread ) ) {
        return true;
    }

    return false;
}

bool affinity_signal_main_thread() {
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );
    std::shared_ptr<AffinityThreadDispatcher> afDisp = std::shared_ptr<AffinityThreadDispatcher>( new AffinityThreadDispatcher );
    conn->add_thread_dispatcher( afDisp );

    std::shared_ptr<DBus::SignalProxy<void()>> proxy = conn->create_free_signal_proxy<void()>(
                DBus::MatchRuleBuilder::create()
                .set_interface( "interface.name" )
                .set_member( "myname" )
                .as_signal_match(),
                DBus::ThreadForCalling::CurrentThread );

    proxy->connect( sigc::ptr_fun( receiveSignal ) );

    std::shared_ptr<DBus::Signal<void()>> signal = conn->create_free_signal<void()>( "/", "interface.name", "myname" );

    signal->emit();

    std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
    afDisp->processMessages();

    if( rxSignal && ( mainThreadId == rxThread ) ) {
        return true;
    }

    return false;
}

bool affinity_message_dispatch_thread() {
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );
    conn->request_name( "dbuscxx.test" );

    std::shared_ptr<DBus::Object> object = conn->create_object( "/test", DBus::ThreadForCalling::DispatcherThread );

    object->create_method<void()>( "test.for.dbuscxx", "rxMethod", sigc::ptr_fun( receiveMethodCall ) );

    std::shared_ptr<DBus::ObjectProxy> remote = conn->create_object_proxy( "dbuscxx.test", "/test" );
    std::shared_ptr<DBus::MethodProxy<void()>> remoteMethod =
            remote->create_method<void()>( "test.for.dbuscxx", "rxMethod" );

    ( *remoteMethod )();

    if( rxMessage && ( mainThreadId != rxThread ) ) {
        return true;
    }

    return false;
}

bool affinity_message_main_thread() {
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );
    std::shared_ptr<AffinityThreadDispatcher> afDisp = std::shared_ptr<AffinityThreadDispatcher>( new AffinityThreadDispatcher );
    conn->add_thread_dispatcher( afDisp );
    conn->request_name( "dbuscxx.test" );

    std::shared_ptr<DBus::Object> object = conn->create_object( "/test", DBus::ThreadForCalling::CurrentThread );

    object->create_method<void()>( "test.for.dbuscxx", "rxMethod", sigc::ptr_fun( receiveMethodCall ) );

    std::shared_ptr<DBus::ObjectProxy> remote = conn->create_object_proxy( "dbuscxx.test", "/test" );
    std::shared_ptr<DBus::MethodProxy<void()>> remoteMethod =
            remote->create_method<void()>( "test.for.dbuscxx", "rxMethod" );

    std::future<void> result = remoteMethod->call_async();

    // Stupid async trick!  We need to process the messages before
    // we can wait for the response, as wait() will not return unless
    // the remote method has finished!
    std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
    afDisp->processMessages();
    result.wait();

    if( rxMessage && ( mainThreadId == rxThread ) ) {
        return true;
    }

    return false;
}

bool affinity_message_change_thread() {
    bool dispatcherThreadOk = false;
    bool mainThreadOk = false;

    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );
    std::shared_ptr<AffinityThreadDispatcher> afDisp = std::shared_ptr<AffinityThreadDispatcher>( new AffinityThreadDispatcher );
    conn->add_thread_dispatcher( afDisp );
    conn->request_name( "dbuscxx.test" );

    std::shared_ptr<DBus::Object> object = conn->create_object( "/test", DBus::ThreadForCalling::DispatcherThread );

    object->create_method<void()>( "test.for.dbuscxx", "rxMethod", sigc::ptr_fun( receiveMethodCall ) );

    std::shared_ptr<DBus::ObjectProxy> remote = conn->create_object_proxy( "dbuscxx.test", "/test" );
    std::shared_ptr<DBus::MethodProxy<void()>> remoteMethod =
            remote->create_method<void()>( "test.for.dbuscxx", "rxMethod" );

    ( *remoteMethod )();

    if( rxMessage && ( mainThreadId != rxThread ) ) {
        dispatcherThreadOk = true;
    }

    conn->change_object_calling_thread( object, DBus::ThreadForCalling::CurrentThread );

    std::future<void> result = remoteMethod->call_async();

    // Stupid async trick!  We need to process the messages before
    // we can wait for the response, as wait() will not return unless
    // the remote method has finished!
    std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
    afDisp->processMessages();
    result.wait();

    if( rxMessage && ( mainThreadId == rxThread ) ) {
        mainThreadOk = true;
    }

    return dispatcherThreadOk && mainThreadOk;
}

#define ADD_TEST(name) do{ if( test_name == STRINGIFY(name) ){ \
            ret = affinity_##name();\
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

    mainThreadId = std::this_thread::get_id();
    dispatch = DBus::StandaloneDispatcher::create();

    ADD_TEST( signal_dispatcher_thread );
    ADD_TEST( signal_main_thread );
    ADD_TEST( message_dispatch_thread );
    ADD_TEST( message_main_thread );
    ADD_TEST( message_change_thread );

    return !ret;
}

// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
#include <dbus-cxx-glib.h>

#include "calleeclass.h"

static int retval = 0;

static int doexit( gpointer user_data ){
    GMainLoop* loop = static_cast<GMainLoop*>( user_data );
    g_main_loop_quit( loop );
    return 0;
}

static int callmethod( gpointer user_data ){
    DBus::MethodProxy<int(int,int)>* proxy = static_cast<DBus::MethodProxy<int(int,int)>*>( user_data );

    retval = (*proxy)( 5, 10 );

    return 0;
}

int main(int argc, char** argv){
    std::shared_ptr<DBus::Dispatcher> disp;
    GMainLoop* mainLoop;

    DBus::set_logging_function( DBus::log_std_err );
    DBus::set_log_level( SL_TRACE );

    mainLoop = g_main_loop_new( nullptr, false );

    disp = DBus::GLib::GLibDispatcher::create();

    std::shared_ptr<DBus::Connection> conn = disp->create_connection( DBus::BusType::SESSION );
    std::shared_ptr<DBus::ObjectProxy> proxy = conn->create_object_proxy( "dbuscxx.test", "/test" );
    std::shared_ptr<DBus::MethodProxy<int(int,int)>> methodref = proxy->create_method<int(int,int)>( "test.foo", "add" );


    g_timeout_add( 100, callmethod, methodref.get() );

    g_timeout_add( 1000, doexit, mainLoop );

    g_main_loop_run( mainLoop );

    g_main_loop_unref( mainLoop );

    return !(retval == 15);
}

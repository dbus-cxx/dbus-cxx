// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
#include <stdio.h>
#include <dbus-cxx.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <time.h>

using namespace std;

int times;
int numSignalsRx;

void voidMethod( std::string str ) {
    times++;
}

void signalRxMethod( int a, int b, std::string c ) {
    numSignalsRx++;
}

int randomWaitingMethod() {
    int msToWait = rand() % 50;

    std::this_thread::sleep_for( std::chrono::milliseconds( msToWait ) );

    return msToWait;
}

int main( int argc, char** argv ) {
    times = 0;
    numSignalsRx = 0;
    srand( time( nullptr ) );

    DBus::set_log_level( SL_DEBUG );
    DBus::set_logging_function( DBus::log_std_err );

    //The dispatcher sends us information.  Make sure that it doesn't go out of scope or bad things will happen.
    std::shared_ptr<DBus::Dispatcher> dispatcher = DBus::StandaloneDispatcher::create();
    //Create a connection to the session bus
    std::shared_ptr<DBus::Connection> connection = dispatcher->create_connection( DBus::BusType::SESSION );

    //Request a unique name on the session bus, check for an error.
    DBus::RequestNameResponse ret = connection->request_name( "com.rm5248", DBUSCXX_NAME_FLAG_REPLACE_EXISTING );

    if( DBus::RequestNameResponse::PrimaryOwner != ret ) { return 1; }

    //Create a path to export methods on
    std::shared_ptr<DBus::Object> object = connection->create_object( "/", DBus::ThreadForCalling::DispatcherThread );
    object->create_method<void( std::string )>( "com.rm5248.ReceiveInterface", "voidMethod", sigc::ptr_fun( voidMethod ) );
    object->create_method<int()>( "com.rm5248.ReceiveInterface", "randomWaitingMethod", sigc::ptr_fun( randomWaitingMethod ) );

    std::shared_ptr<DBus::SignalProxy<void(int, int, std::string)>> proxy = connection->create_free_signal_proxy<void(int, int, std::string)>(
                DBus::MatchRuleBuilder::create()
                    .set_path( "/" )
                    .set_interface( "com.rm5248.ReceiveInterface" )
                    .set_member( "SignalName" )
                    .as_signal_match(),
                DBus::ThreadForCalling::DispatcherThread );

    proxy->connect( sigc::ptr_fun( signalRxMethod ) );

    while( times < 80000 * 4 ) {
        sleep( 5 );
    }

    std::cout << "Exiting, have recieved " << times << " messages" << std::endl;
    std::cout << numSignalsRx << " signals received" << std::endl;

    return 0;
}

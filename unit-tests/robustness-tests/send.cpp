// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
#include <stdio.h>
#include <dbus-cxx.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main( int argc, char** argv ) {
    int currentTimes = 0;
    int maxTimes = 80000;

    srand( time( nullptr ) );

    //The dispatcher sends us information.  Make sure that it doesn't go out of scope or bad things will happen.
    std::shared_ptr<DBus::Dispatcher> dispatcher = DBus::StandaloneDispatcher::create();
    //Create a connection to the session bus
    std::shared_ptr<DBus::Connection> connection = dispatcher->create_connection( DBus::BusType::SESSION );

    //Create a pointer to a remote object with the specified interface and path.
    std::shared_ptr<DBus::ObjectProxy> object = connection->create_object_proxy( "com.rm5248", "/" );

    DBus::MethodProxy<void( std::string )>& voidMethod = *( object->create_method<void( std::string )>
            ( "com.rm5248.ReceiveInterface", "voidMethod" ) );
    DBus::MethodProxy<int()>& randomWaitingMethod = *( object->create_method<int()>
            ( "com.rm5248.ReceiveInterface", "randomWaitingMethod" ) );

    std::shared_ptr<DBus::Object> localObj = connection->create_object( "/", DBus::ThreadForCalling::DispatcherThread );
    std::shared_ptr<DBus::Signal<void(int, int, std::string)>> sigSend =
            localObj->create_signal<void(int, int, std::string)>( "com.rm5248.ReceiveInterface", "SignalName" );

    try {
        while( currentTimes++ < maxTimes ) {
            voidMethod( "one" );
            voidMethod( "two" );
            voidMethod( "three" );
            voidMethod( "four" );

            if( currentTimes % 100 == 0 ) {
                randomWaitingMethod.call_async();
            }

            if( rand() % 4 == 0 ) {
                sigSend->emit( rand(), rand() % 1000, "foooooooooooooooooooooooooooooobar" );
            }
        }
    } catch( DBus::Error err ) {
        std::cerr << "Exiting due to error: " << err.what() << std::endl;
        std::cerr << "Message: " << err.message() << std::endl;
        std::cerr << "Message name: " << err.name() << std::endl;
        return 1;
    }

    return 0;
}

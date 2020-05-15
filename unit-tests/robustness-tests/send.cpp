#include <stdio.h>
#include <dbus-cxx.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main(int argc, char** argv){
    int currentTimes = 0;
    int maxTimes = 80000;

	//The dispatcher sends us information.  Make sure that it doesn't go out of scope or bad things will happen.
    std::shared_ptr<DBus::Dispatcher> dispatcher = DBus::Dispatcher::create();
	//Create a connection to the session bus
    std::shared_ptr<DBus::Connection> connection = dispatcher->create_connection( DBus::BusType::SESSION );

	//Create a pointer to a remote object with the specified interface and path.
    std::shared_ptr<DBus::ObjectProxy> object = connection->create_object_proxy("com.rm5248", "/");

    DBus::MethodProxy<void(std::string)>& voidMethod = *(object->create_method<void(std::string)>
		("com.rm5248.ReceiveInterface", "voidMethod"));

    try{
        while( currentTimes++ < maxTimes ){
            voidMethod("one");
            voidMethod("two");
            voidMethod("three");
            voidMethod("four");
        }
    }catch( DBus::Error err ){
        std::cerr << "Exiting due to error: " << err.what() << std::endl;
        std::cerr << "Message: " << err.message() << std::endl;
        std::cerr << "Message name: " << err.name() << std::endl;
        return 1;
    }

  	return 0;
}

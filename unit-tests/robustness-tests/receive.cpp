#include <stdio.h>
#include <dbus-cxx.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

using namespace std;
int times;

void voidMethod( std::string str ){
    times++;
}


int main(int argc, char** argv){
    times = 0;

	//The dispatcher sends us information.  Make sure that it doesn't go out of scope or bad things will happen.
    std::shared_ptr<DBus::Dispatcher> dispatcher = DBus::Dispatcher::create();
	//Create a connection to the session bus
    std::shared_ptr<DBus::Connection> connection = dispatcher->create_connection( DBus::BusType::SESSION );

	//Request a unique name on the session bus, check for an error.
    DBus::RequestNameResponse ret = connection->request_name( "com.rm5248", DBUSCXX_NAME_FLAG_REPLACE_EXISTING );
    if (DBus::RequestNameResponse::PrimaryOwner != ret) return 1;

	//Create a path to export methods on
    std::shared_ptr<DBus::Object> object = connection->create_object("/", DBus::ThreadForCalling::DispatcherThread);

    object->create_method<void(std::string)>("com.rm5248.ReceiveInterface","voidMethod", sigc::ptr_fun(voidMethod) );
  	
    while( times < 80000 * 4 ){
        sleep(5);
  	}

    std::cout << "Exiting, have recieved " << times << " messages" << std::endl;
  	
  	return 0;
}

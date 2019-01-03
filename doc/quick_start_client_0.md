Quick Start Client 0 {#quick_start_client_0}
==

\section quick_start_server_0 A simple client

\par
This section provides an example of a simple client for our simple server.

\par Summary
This is an overview of the code we will need in our example client:
-# Include headers
-# Initialize the dbus-cxx library
-# Create a dispatcher to manage threads, timeouts and I/O watches
-# Create a connection to the D-Bus session bus
-# Create a proxy object for the server's \c quickstart_0 object
-# Create a proxy \c add method for our proxy object
-# Call our proxy method
-# Print out the results

\section quick_start_client_0_code Simple client 0 code

\par
Here is the code in full, followed by a section-by-section discussion.

\include quickstart_client_0.cpp

\section quick_start_client_0_discussion Simple client 0 code discussion

\par Include headers
First, we need to include the main dbus-cxx library header. We'll also
include the \c iostream header since we'll print out the results
of our addition.
\code
#include <dbus-cxx.h>
#include <iostream>
\endcode

\par
And now for the beginning of our \c main() function.
\code
int main()
{
\endcode

\par Initialize the dbus-cxx library
Before we do anything in dbus-cxx we must call the library's \c init() method
which will take care of various initializations including the threading system.
\code DBus::init(); \endcode

\par Create a dispatcher to manage threads, timeouts and I/O watches
Now, we will create a dispatcher to handle incoming and outgoing messages.
Handling incoming and outgoing messages can be messy and the Dispatcher
class handles much of this for us.
\code DBus::Dispatcher::pointer dispatcher = DBus::Dispatcher::create(); \endcode

\par Create a connection to the D-Bus session bus
Now that we have a dispatcher we need to create a connection to the session bus.
\code DBus::Connection::pointer conn = dispatcher->create_connection(DBus::BUS_SESSION); \endcode

\par
\par Request a name for our application on the bus
Next, we need to request a name that will identify our application on the
session bus.
\code
  ret = conn->request_name( "dbuscxx.quickstart_0.server", DBUS_NAME_FLAG_REPLACE_EXISTING );
  if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) return 1;
\endcode

\par Create a proxy object for the server's \c quickstart_0 object
You'll notice that up to this point the code for the client and the server
has been nearly identical. This is the point at which they will diverge.
We will now create a proxy object for the object on the server.
\code
  DBus::ObjectProxy::pointer object;
  object = connection->create_object_proxy("dbuscxx.quickstart_0.server", "/dbuscxx/quickstart_0");
\endcode
Note that we must use the name the server requested as well as the object
path the server used when it created it's object when we create the
proxy object.

\par Create a proxy \c add method for our proxy object
Now we create a proxy \c add method for the server object's \c add method.
This will allow us to use function notation to call the method. When
creating the proxy method we must use the same interface
(\c "dbuscxx.Quickstart" ) and method name ( \c "add" ) as the server used.
\code
  DBus::MethodProxy<double,double,double>& add_proxy
    = *(object->create_method<double,double,double>("dbuscxx.Quickstart","add"));
\endcode

\par Call our proxy method
Now we can use our proxy method to call the method on the server using
function notation. We'll keep things simple and just add \c 1.1 \c + \c 2.2 .
\code
  double answer;
  answer = add_proxy( 1.1, 2.2 );
\endcode

\par Print out the results
Finally, we can print out the results.
\code
  std::cout << "1.1 + 2.2 = " << answer << std::endl;

  return 0;
}
\endcode

<b>Go Back:</b> \ref quick_start


Quick-start {#quick-start}
===

[TOC]

Once you have installed dbus-cxx, you are now ready to start using it!
If you're unfamiliar with DBus in general, you should read https://rm5248.com/d-bus-tutorial/ first.

# Using dbus-cxx with your build system

The first thing that you must do is to properly setup your build system in
order to properly find and link with the dbus-cxx library.  This varies
depending on the buildsystem that you use, but here are some general
directions.

## pkg-config files

When you build and install dbus-cxx, there are several pkg-config files
that are provided.  They are as follows:

|.pc file|Used for|
|--------|--------|
|dbus-cxx-2.0.pc|Using the library directly.  If you're not using a framework/library, this is what you want to use|
|dbus-cxx-qt-2.0.pc|Integrating dbus-cxx with Qt.  Depends on dbus-cxx-2.0.pc|
|dbus-cxx-glib-2.0.pc|Integrating dbus-cxx with GLib.  Depends on dbus-cxx-2.0.pc|


## CMake + dbus-cxx

There are two ways to configure your CMake build to use dbus-cxx.  The first
way is to use the pkg-config files with the `FindPkgConfig` module.  To do
this, simply add the following lines to your CMakeLists.txt:

```
include( FindPkgConfig )
pkg_check_modules( dbus-cxx REQUIRED IMPORTED_TARGET dbus-cxx-2.0 )

target_link_libraries( exe PkgConfig::dbus-cxx )
```

If you want to use the `find_package` directive of CMake, your CMakeLists.txt file
should look something like the following:

```
include( FindPkgConfig )
pkg_check_modules( sigc REQUIRED IMPORTED_TARGET sigc++-3.0 )
find_package( dbus-cxx REQUIRED )
find_package( Threads )

target_link_libraries( exe dbus-cxx::dbus-cxx Threads::Threads PkgConfig::sigc )
```

Note that you need to use both pkg-config and the `find_package` directive,
because libsigc++ does not provide a CMake package at this point.

## qmake + dbus-cxx

Because of Qt's signal/slot mechanism, dbus-cxx will not work directly with Qt.  Assuming that you are using QMake,
you will need to add the following lines to your .pro file:
```
CONFIG += no_keywords
unix:CONFIG += link_pkgconfig
unix:PKGCONFIG += dbus-cxx-qt-2.0
```
Note that this will also allow you to use the Qt integration with dbus-cxx.

At any point in your Qt program, you need to use Qt signals or slots, use the macros Q_SIGNALS or Q_SLOTS to define
your signals/slots.

## Autotools + dbus-cxx

If you are using autotools, modify configure.ac (or configure.in ) with the following lines:
```
PKG_CHECK_MODULES(PROJECT_DBUSCXX,[dbus-cxx-2.0 >= 2.0.0])
AC_SUBST(PROJECT_DBUSCXX_LIBS)
AC_SUBST(PROJECT_DBUSCXX_CFLAGS)
```

You can then use the symbols `PROJECT_DBUSCXX_LIBS` and `PROJECT_DBUSCXX_CFLAGS` in your Makefile.am files.
For example, if you have a application named fooapp your Makefile.am might look like this:
```
INCLUDES = $(PROJECT_DBUSCXX_CFLAGS)
bin_PROGRAMS = fooapp
fooapp_SOURCES = fooapp.cpp
fooapp_LDADD = $(PROJECT_DBUSCXX_LIBS)
```

# Initial Concepts {#quick_start_initial_concepts}


A dbus-cxx [Object](@ref DBus.Object) is a collection of methods and signals. A method is an
operation that may be invoked on a object and may or may not return a value.
A signal is emitted by the object and does not have a return value.

A dbus-cxx [ObjectProxy](@ref DBus.ObjectProxy) provides an abstration of a remote Object to a client
application. The ObjectProxy provides method and signal proxies that have
signatures equivalent to their server-side counterparts.

A [method proxy](@ref DBus.MethodProxy) assembles the parameters of the method call into a D-Bus
message and transmits to the server. The response (if any) is returned in
the form of a D-Bus message which is then unpacked and returned to the
application as the return value from the proxy call.

A [signal proxy](@ref DBus.SignalProxy) is different from a method proxy in that it watches an Object
for signal events. These signal events arrive as a D-Bus signal message.
When a signal event arrives the parameters are unpacked and emitted locally
through the signal proxy callback.

## sigc++ signal and slot library

dbus-cxx relies heavily on libsigc++'s signal and slot system for both
Objects and their proxies.

On the server side and object's methods rely upon slots to provide the
underlying functionality. When an incoming method call is invoked, the
parameters of the call are unpacked and passed to the slot associated
with the method.

Slots are similar to function pointers in C, but can also be a method
of a specific class.

### Free Functions

Let's say that we have a free function that we want to have called:

```{.cpp}
void foo(){
  std::cout << "Hello foo!\n";
}
```

We would then connect this function using the `sigc::ptr_fun()` method like so:

```{.cpp}
std::shared_ptr<DBus::Object> obj =
        conn->create_object( "/foo/bar", DBus::ThreadForCalling::DispatcherThread );

obj->create_method<void()>( "what.blah", "foo", sigc::ptr_fun( foo ) );
```

### Member Functions

If we have a function on a class that we want to call instead, we use `sigc::mem_fun` in
order to connect this code to be called.  Assuming something similar as above, our code
may look something like the following:

```{.cpp}
class Foobar {
public:
    void foo(){
        std::cout << "Hello Foobar::foo!\n";
    }
};

int main(){
    Foobar f;
    std::shared_ptr<DBus::Dispatcher> dispatcher = DBus::StandaloneDispatcher::create();
    std::shared_ptr<DBus::Connection> connection = dispatcher->create_connection( DBus::BusType::SESSION );

    std::shared_ptr<DBus::Object> obj =
          connection->create_object( "/foo/bar", DBus::ThreadForCalling::DispatcherThread );
    obj->create_method<void()>( "what.blah", "foo", sigc::mem_fun(f, &Foobar::foo) );

    connection->request_name( "dbuscxx.foo.bar" );

    std::this_thread::sleep_for( std::chrono::seconds( 30 ) );

    return 0;
}
```

## Smart pointers and create()

DBus-cxx makes extensive use of smart pointers to help manage the memory of a
class.  In order for this scheme to work properly, most classes cannot be
initialized on their own(constructors are private).  To create an instance
of a class, use the `create()` method on that class:

```
std::shared_ptr<DBus::Dispatcher> disp = DBus::StandaloneDispatcher::create();
```

# Quick-start example

This example will demonstrate a simple server and client pair.

The server will only offer one method named add that will take two
double parameters and return the sum as a double.

The client will use a proxy object for the server as well as a proxy for the
add method.

## Server code

This section provides an example of a simple server. The server will run
for 10 seconds and then automatically shut-down.

### Summary
This is an overview of the code we will need in our example server:
* Include headers
* Create a dispatcher to manage threads, timeouts and I/O watches
* Create a connection to the D-Bus session bus
* Request a name for our application on the bus
* Create an object on the bus
* Add a method named `add` to our object
* Wait for incoming calls

Here is the code in full, followed by a section-by-section discussion.

\include quickstart_server_0.cpp

### Simple server 0 code discussion

First, we need to include the main dbus-cxx library header:
```{.cpp}
#include <dbus-cxx.h>
```

Then we will define a function that will be our workhorse on the server side.
This function will be the actual function that will be called when the dbus
`add` method is invoked, so we will name our function similarly. But, note
that the actual name of the function in our program and the name in our
dbus server do not have to match.
```{.cpp}
double add( double param1, double param2 )      { return param1 + param2; }
```

And now for the beginning of our `main()` function. We'll also declare a
variable `ret` that can be used to check the return value of functions.
```{.cpp}
int main()
{
  int ret;
```

### Create a dispatcher to manage threads, timeouts and I/O watches
Now, we will create a dispatcher to handle incoming and outgoing messages.
Handling incoming and outgoing messages can be messy and the Dispatcher
class handles much of this for us.

Since this is just a standalone example, we will use the standalone dispatcher

```{.cpp}
std::shared_ptr<DBus::Dispatcher> dispatcher = DBus::StandaloneDispatcher::create();
```

### Create a connection to the D-Bus session bus
Now that we have a dispatcher we need to create a connection to the session bus.
```{.cpp}
std::shared_ptr<DBus::Connection> conn = dispatcher->create_connection(DBus::BusType::SESSION);
```

### Request a name for our application on the bus
Next, we need to request a name that will identify our application on the
session bus.
```{.cpp}
  if( conn->request_name( "dbuscxx.quickstart_0.server", DBUSCXX_NAME_FLAG_REPLACE_EXISTING ) != DBus::RequestNameResponse::PrimaryOwner )
      return 1;
```

### Create an object on the bus
Now that our application has a name on the bus we need to add an object with
its path.
```{.cpp}
std::shared_ptr<DBus::Object> object = conn->create_object("/dbuscxx/quickstart_0", DBus::ThreadForCalling::DispatcherThread);
```

### Add a method named add to our object
We will now create a method named `add` for our object. The functionality
of the method will be provided by the function we declared above also named
`add()`. We must add this to an interface, and the D-Bus specification
required that interface names must contain at least one `.` (period)
character so we will use the interface name `"dbuscxx.Quickstart"`.

Note that our DBus object method named `add` has nothing to do with C++
class methods. In essence we are creating virtual objects on the D-Bus
and can choose to provide their functionality with either C++ class methods
and/or plain functions.
```{.cpp}
object->create_method<double(double,double)>("dbuscxx.Quickstart", "add", sigc::ptr_fun(add) );
```

### Wait for incoming calls
Finally, we need to wait for incoming calls. We will sleep for 10 seconds in
our `main()` function and the calls will be handled in the dispatcher's
threads.
```{.cpp}
  std::this_thread::sleep_for(std::chrono::seconds(10));

  return 0;
}
```

## Client code

This section provides an example of a simple client for our simple server.

This is an overview of the code we will need in our example client:
* Include headers
* Create a dispatcher to manage threads, timeouts and I/O watches
* Create a connection to the D-Bus session bus
* Create a proxy object for the server's `quickstart_0` object
* Create a proxy `add` method for our proxy object
* Call our proxy method
* Print out the results

Here is the code in full, followed by a section-by-section discussion.

\include quickstart_client_0.cpp

### Include headers
First, we need to include the main dbus-cxx library header. We'll also
include the `iostream` header since we'll print out the results
of our addition.
```
#include <dbus-cxx.h>
#include <iostream>
```

### Initializing the Dispatcher and Connection

Now, we will create a dispatcher to handle incoming and outgoing messages.
Handling incoming and outgoing messages can be messy and the Dispatcher
class handles much of this for us.  Because this is a standalone example,
we will use a StandloneDispatcher to do all of this work for us.
```{.cpp}
std::shared_ptr<DBus::Dispatcher> dispatcher = DBus::StandaloneDispatcher::create();
```

Now that we have a dispatcher we need to create a connection to the session bus.
```{.cpp}
std::shared_ptr<DBus::Connection> connection = dispatcher->create_connection( DBus::BusType::SESSION );
```
Note that unlike the server example, we do not request a name on the bus.  This is because
we do not need to have a well-known name on the bus in order to call methods on other objects.

### Creating a Proxy object

You'll notice that up to this point the code for the client and the server
has been very similar. This is the point at which they will diverge.
We will now create a proxy object for the object on the server.
```{.cpp}
std::shared_ptr<DBus::ObjectProxy> object = connection->create_object_proxy("dbuscxx.quickstart_0.server", "/dbuscxx/quickstart_0");
```
Note that we must use the name the server requested as well as the object
path the server used when it created its object when we create the
proxy object.

Now we create a proxy `add` method for the server object's `add` method.
This will allow us to use function notation to call the method. When
creating the proxy method we must use the same interface
( `"dbuscxx.Quickstart"` ) and method name ( `"add"` ) as the server used.
```{.cpp}
DBus::MethodProxy<double(double,double)>& add_proxy
    = *(object->create_method<double(double,double)>("dbuscxx.Quickstart","add"));
```

### Call our proxy method
Now we can use our proxy method to call the method on the server using
function notation. We'll keep things simple and just add `1.1` + `2.2` .
```{.cpp}
  double answer;
  answer = add_proxy( 1.1, 2.2 );
```

### Print out the results
Finally, we can print out the results.
```{.cpp}
  std::cout << "1.1 + 2.2 = " << answer << std::endl;

  return 0;
}
```

------------------------
The following pages provide the quick-start for dbus-cxx:

1. @subpage quick_start_client_0.md
2. @subpage quick_start_cmake.md
3. @subpage quick_start_example_0.md
4. @subpage quick_start_initial_concepts.md
5. quick_start.md
6. quick_start_pkgconfig.md
7. quick_start_server_0.md

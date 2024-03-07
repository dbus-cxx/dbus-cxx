Dispatching in dbus-cxx {#dispatching}
===

Within the context of dbus-cxx, the concept of 'dispatching' refers
to the reading and writing of messages to the bus.

Each application that uses dbus-cxx needs to have an implementation
of a `Dispatcher` in order to work correctly.  In general, there
only needs to be one dispatcher per application.

If the `Dispatcher` is running in a separate thread, you must also
define a `ThreadDispatcher` which will handle calling methods and
emitting signals from the correct thread, should you choose to recieve
method calls and receive signals from a thread that is not the main
dispatcher thread.

Depending on your implementation, the `Dispatcher` may not be in a
separate thread.  If you have a case where the `Dispatcher` is in
the main thread of a single-threaded application, then the enum values
`DBus::ThreadForCalling::DispatcherThread` and
`DBus::ThreadForCalling::CurrentThread` are effectively the same.

# Standalone Dispatcher

The basic `Dispatcher` that is always available is the
`StandaloneDispatcher` class.  This class will create a new thread
that handles all of the reading and writing to the bus.

This implementation of the `Dispatcher` class is intended to work
with any library that is used and should be fully thread-safe.

If you are using multiple threads, you will need to implement
an object of type `ThreadDispatcher` if you need the methods to
be called from a specific thread.

Example:

```{.cpp}
/* Create a new dispatcher that does the reading/writing from a separate thread */
std::shared_ptr<DBus::Dispatcher> dispatch = DBus::StandaloneDispatcher::create();
std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

/* Create an object whose methods will be called from the DispatcherThread created by the StandaloneDispatcher */
std::shared_ptr<DBus::Object> obj =
                conn->create_object( "/example", DBus::ThreadForCalling::DispatcherThread );

/* The method created here will be called from the DispatcherThread */
obj->create_method<...>( "dbuscxx.interface", "example_method", sigc::[mem|ptr]_fun(...) );
```

# Dispatching with Qt

If you are using Qt, an implementation of a `Dispatcher` that runs in the main thread
has already been created for you.  As well, there is also an implementation of a
`ThreadDispatcher` to ensure that you may use a separate `QThread` in order to
multi-thread your application.

Example with dispatching in main thread:

```{.cpp}
#include <dbus-cxx-qt.h>

...

std::shared_ptr<DBus::Dispatcher> disp = DBus::Qt::QtDispatcher::create();
std::shared_ptr<DBus::Connection> conn = disp->create_connection( DBus::BusType::SESSION );

/* Create an object whose methods will be called from the main thread(which also is the dispatcher thread) */
std::shared_ptr<DBus::Object> obj =
                conn->create_object( "/example", DBus::ThreadForCalling::DispatcherThread );

/* The method created here will be called from the main thread(which is also the dispatcher thread) */
obj->create_method<...>( "dbuscxx.interface", "example_method", sigc::[mem|ptr]_fun(...) );
```

Example with the StandaloneDispatcher:

```{.cpp}
std::shared_ptr<DBus::Dispatcher> disp = DBus::StandaloneDispatcher::create();
std::shared_ptr<DBus::Connection> conn = disp->create_connection( DBus::BusType::SESSION );

/* Since the StandaloneDispatcher creates a new thread, we will need a QtThreadDispatcher to ensure
 * that we have proper cross-thread communications
 */
std::shared_ptr<DBus::Qt::QtThreadDispatcher> thrDisp = DBus::Qt::QtThreadDispatcher::create();
conn->add_thread_dispatcher( thrDisp );


/* Create an object whose methods will be called from the main thread(which is not the dispatcher thread) */
std::shared_ptr<DBus::Object> obj =
                conn->create_object( "/example", DBus::ThreadForCalling::CurrentThread );

/* The method created here will be called from the main thread thanks to the QtThreadDispatcher we created */
obj->create_method<...>( "dbuscxx.interface", "example_method", sigc::[mem|ptr]_fun(...) );
```

# Dispatching with GLib

When using GLib, the `GLibDispatcher` class may be used in a similar manner as the
`QtDispatcher` class.  Note that due to how GLib works at the moment, this has only
been testing using GLib, not GLibMM(libsigc++ versioning problem).

There is currently no implementation of a `ThreadDispatcher` for GLib.

Example:

```{.cpp}
std::shared_ptr<DBus::Dispatcher> disp = DBus::GLib::GLibDispatcher::create();
std::shared_ptr<DBus::Connection> conn = disp->create_connection( DBus::BusType::SESSION );

/* Other parts are the same */
```

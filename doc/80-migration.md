dbus-cxx 2.0 Migration Guide {#dbus-cxx-2-0-migration}
===

# Migration
Migrating code to the 2.0 version of dbus-cxx should be pretty straightforward.
The library must now be built with the libsigc++ version 3.0.  This also means
that you need a compiler that supports C++17.

This now means that argument lists of an arbitrary length can be used, instead
of the previous hard limit of 7.

Most typedefs have also been removed, and you use `std::shared_ptr` directly
to handle smart pointers.

The syntax for declaring methods has now also changed slightly: instead of
having the first template parameter be the return value, the types now correspond
to method signatures.  This is for compatability with libsigc++ version 3.0.

You must also explicitly define what thread methods need to be called from to ensure
proper multithreading.

## Migration Example
Given the following code with version 1:
```{.cpp}
    DBus::Object::pointer object = conn->create_object("/dbuscxx/example/Calculator");

    object->create_method<double,double,double>("Calculator.Basic", "add", sigc::ptr_fun(add) );
```

This code now becomes:
```{.cpp}
    std::shared_ptr<DBus::Object> object = conn->create_object("/dbuscxx/example/Calculator", DBus::ThreadForCalling::DispatcherThread);

    object->create_method<double(double,double)>("Calculator.Basic", "add", sigc::ptr_fun(add) );
```

You can use the [auto-upgrade.sh](auto-upgrade.sh) script to help automate this

## Type Changes

For consistency with data types that can be sent over the DBus, types such as `float` and
`char` have been removed.  `float` was removed as only IEEE-754 doubles may be sent over
the bus; likewise, `char` is not a type that can be sent over, it is a `uint8_t`, or single
byte.

The previous `DBus::Variant<>` templated class has been removed and replaced with a version
that is not templated.

DBus structs map to `std::tuple`.  Previously, there was no type for `struct`.

## Backend Changes

DBus-cxx is no longer a binding to `libdbus`, it is now a complete implementation.  This
was done partially to make it possible to store arrays(`std::vector`) and dictionaries
(`std::map`) inside of `DBus::Variant`.

This also should allow for the fixing of known multithreading issues in `libdbus`.

## API Changes

A number of libsigc++ signals have been removed, as it was never defined what thread
they would be emitted from, and they were not useful.  For example, it was possible
to change the name of interfaces and methods at run-time; this ability has been removed
and the associated signals for the change were removed as well.

A number of methods were removed from the `DBus::Connection` class, as they just mapped
directly to `libdbus` functionality that has not been re-implemented.  If needed,
methods can be brought back for needed use-cases.

When exporting objects, you must now be explicit about which thread to call the methods
on this object from.  This also necessitates a change to how the `DBus::Dispatcher`
class works.

## API and ABI Compatibility

As of version 2, it is intended that the API will be stable(or at least backwards-
compatible), and that the library itself will be ABI compatible.

# Major changes for dbus-cxx version 2.0

* libdbus is no longer used; there is a new ground-up implementation for talking to the bus
* C++17 is required to compile, due to using many C++17 features
* Templated types now take in the template for the function signature(sigc++-3.0), and also
 can have an arbitrary number of arguments.
* The library should now be ABI compatible, making it possible to easily upgrade
* A new DBus::priv namespace has been introduced; this namespace is intended for internal
 use only, and is not gauranteed to maintain any API/ABI compatible changes.  Use any
 of the classes in this namespace at your own risk!
* The threading model has been overhauled to ensure that methods will be called in
 the correct thread.  You must now explicity state which thread methods should be called in.
* New enums have been introduced to make programming more explicit
* The introduction of `std::tuple` for a DBus struct should eliminate the need for custom
 types
* dbus-cxx-xml2cpp has been overhauled(again).  This should allow for better code generation.
* Added a `call_async` method to all `DBus::MethodProxy`s to make it easier to call methods
 in an async manner.
* Fixed some circular dependencies for library classes - some classes could never be
 deleted due to circular `std::shared_ptr` references.
* Added `operator<<` for many classes and enums to make debugging easier
* Refactored code that used `pthread` objects to use `std::mutex`, `std::thread`,
 `std::unique_lock` as appropriate.
* Added a large number of tests to ensure we don't break anything
* Code generation will now output templated types correctly
* Debug code for calling methods will demangle C++ names, so it should help identify
 what exactly method is being called
* Exceptions thrown are no longer `std::shared_ptr`s
* Many typedefs have been removed(see [this thread][1] for justifications)

# Features Removed

The following features have been removed.  Generally, these features were removed
because they are not believed to be widely used.  If there is demand, they may
be reimplmented in a new version

* Likely remove primary vs. fallback handling - doesn't really work well in an OO
 environment.  If you want to have one object on multiple paths, you can export
 the object multiple times
* Message filtering for an incoming message.  Exactly how this was supposed to work
 was never well defined.
* Having the same object exported on multiple connections.  In all likelyhood
 this never worked properly in the first place, but there are some areas in the
 code which refer to it and is structured to potentially allow for it.
* Many signals that give information about lifecycle state have been removed.
 Part of this is due to the signals potentially being emitted on the wrong
 thread, and part of this has to do with how often the signals are used.  Note
 that 'signals' for this bullet point is talking about internal libsigc++ signals,
 not DBus signals received from the bus.

[1]: https://yarchive.net/comp/linux/typedefs.html

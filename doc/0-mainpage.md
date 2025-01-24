dbus-cxx Library {#mainpage}
===


# About
dbus-cxx is a C++ implementation of the %DBus protocol. It does not use the reference C
implementation of libdbus at all in order to provide a tight C++-like interface, and to
fix known multithreading problems with libdbus.  libsigc++ is used to help provide an
OO interface to the bus.
Also included is dbus-cxx-xml2cpp to generate proxy and adapter interfaces from %DBus XML
introspection-like documents.

## Features

dbus-cxx has a number of features that gives the user a large number of ways of solving
problems.  Other dbus libraries that exist do not have quite the same features.  The
following is a list of some of the best features provided by the library:

* Native C++ library - no C bindings required
* Object-Oriented interface - clearly separate various concerns
* Typesafe operations over the bus - methods and signals are declared using
 templates, ensuring that types are properly represented
* Minimal boilerplate required - calling a function over the bus looks exactly
 like calling a local method.  Methods that can be called over the bus need no
 special boilerplate to demarshal the data from the bus
* Designed with thread safety in mind - methods can be called from the bus on
 the %DBus thread or in a separate thread
* Hooks into your existing event loop - use Qt, GLib, or libuv as your main
 event loop without requiring a new thread
* Automatic introspection data generation - define your API in C++ by exporting
 objects, and the introspection XML will be automatically generated for you
* Modern memory management using `shared_ptr` - ensures that resources are not
 destroyed until they are no longer being used
* Multiple return values supported
* dbus types are represented as standard C++ types(e.g. std::map, std::vector, std::tuple)

### Easy method calling and automatic XML generation for your API

This is in my opinion the best feature of dbus-cxx, making it very easy to add methods and
easily expand the API without worrying about customizing the introspection XML or defining
C structs that define your API.

Let's say that we have a method that we want to export on the bus so that it can be called
by another process:

```cpp
double add( double param1, double param2 )      { return param1 + param2; }
```

This method can then be exported onto the bus with a snigle line of code(apart from
standard boilerplate to setup the bus):

```cpp
// ... standard boilerplate here ...


// Assume that 'Object' is of type std::shared_ptr<DBus::Object>
object->create_method<double(double,double)>("Calculator.Basic", "add", sigc::ptr_fun(add) );
```

Congratulations!  We have now exported a method out onto the bus that can be called,
and the implementation of the method is _exactly the same_ as if it is not a method
that needs to be called over the bus.  Since there is no DBus-specific boilerplate
code required in the method that is being called, the implementation of the
function proceeeds the same no matter if the method is called from the bus or not.

When we export the object onto the bus, it is automatically introspectable.
Given the above `create_method` call, our (summarised) introspection XML now looks like:

```
$ dbus-send --session --dest=dbuscxx.example.calculator.server --print-reply /dbuscxx/example/Calculator org.freedesktop.DBus.Introspectable.Introspect
method return time=1737689252.147296 sender=:1.562 -> destination=:1.563 serial=3 reply_serial=2
   string "<!DOCTYPE node PUBLIC "-//freedesktop//DTD D-BUS Object Introspection 1.0//EN"
"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd">
<node name="/dbuscxx/example/Calculator">
  <!-- ... boilerplate here ... -->
  <interface name="Calculator.Basic">
    <method name="add">
      <arg type="d" direction="out"/>
      <arg type="d" direction="in"/>
      <arg type="d" direction="in"/>
    </method>
    <!-- ... more methods here ... -->
  </interface>
</node>
"
```

A similar mechanism can be used to create a proxy function - a function that when
called, will in reality send a message over the %DBus and wait for the return.
This requires only a few lines of code to accomplish:

```cpp
// ... boilerplate here ...

// Assume that `object` is of type std::shared_ptr<DBus::ObjectProxy>
DBus::MethodProxy<double(double,double)>& methodref = *(object->create_method<double(double,double)>("Calculator.Basic", "add"));

double answer;
answer = methodref( 1.0, 2.0 );
```

As you can see, calling the method looks _exactly the same_ as calling a local
method, without requiring the caller to marshal any data into a message or
explicitly wait for a reply.  Replies can always be done in an asynchronous
manner, ensuring that the calling thread does not block.

While these examples show only doubles, all standard types are supported.  See the
[DBus Types](@ref dbus-types) page for more information on the different types
that are supported.

For more information on these examples, look in the `examples/basics/methods` directory.
 
---
## Download dbus-cxx source code

**NOTE:** Pre-built binary packages used to be provided through a private apt server, but this is no longer the case.
If you are willing to help provide packages such as this, please get in touch by using the 'discussions' tab on Github!
 

Releases - (.bz2, .gz, .zip) can be found on [Sourceforge](http://sourceforge.net/projects/dbus-cxx/files)
and [Github](https://github.com/dbus-cxx/dbus-cxx/releases)

[Source building directions](@ref building-from-source)

### Git Repository
You can browse the git repository at this url:

https://github.com/dbus-cxx/dbus-cxx

You can also check out a copy of the repository with this command:
```
git clone https://github.com/dbus-cxx/dbus-cxx.git
```

---
Dependencies... and where to get them

dbus: http://dbus.freedesktop.org - Should be installed by default (runtime dependency) <br/>
libsigc++ https://github.com/libsigcplusplus/libsigcplusplus (compile dependency) <br/>
See the README file for more information on compile-time dependencies <br/>
libcppgenerate https://github.com/rm5248/libcppgenerate (compile dependency, bundled)

---

## Documentation, Tutorials, Guides, Quick Start, et. al.

If you want to jump into the code quickly check out the [quick start](@ref quick-start) guide.
 
dbus-cxx-xml2cpp Reference
Documentation on using  [dbus-cxx-xml2cpp](@ref xml2cpp) to convert %DBus XML introspection
documents into C++ proxies and adapters.
 
The API documentation (including the pages you are reading now) have been generated
with Doxygen.

The most current documentation for dbus-cxx is available online here:
- dbus-cxx - <a href="https://dbus-cxx.github.io">https://dbus-cxx.github.io</a>

Key sections within the API reference
- Namespaces
  - DBus
- Detailed class documentation in the <a href="hierarchy.html">Class Hierarchy</a>
- <a href="annotated.html">Class List</a>
- <a href="examples.html">Examples</a>

### Getting help
If you run into problems, you have several options for getting in touch.

Github is the best forum to initiate discussion of some issues that you may have.

1. Github - [Discussions](https://github.com/dbus-cxx/dbus-cxx/discussions) and [Issues](https://github.com/dbus-cxx/dbus-cxx/issues).  General help should go to the Discussions, while bugs should go to issues.
2. Mailing lists.  These are very old, but they should still work

 - Google group(preferred mailing list)
   - <a href="https://groups.google.com/forum/#!forum/dbus-cxx">Dbus-cxx Google Group</a>
 - Sourceforge Users List (for those using dbus-cxx in their applications)
   - <a href="http://sourceforge.net/mailarchive/forum.php?forum_name=dbus-cxx-users">Archives</a>
   - <a href="http://lists.sourceforge.net/mailman/listinfo/dbus-cxx-users">Subscribe/Unsubscribe</a>
 - Sourceforge Development List (for discussion related to the development of dbus-cxx itself)
   - <a href="http://sourceforge.net/mailarchive/forum.php?forum_name=dbus-cxx-devel">Archives</a>
   - <a href="http://lists.sourceforge.net/mailman/listinfo/dbus-cxx-devel">Subscribe/Unsubscribe</a>


# License
dbus-cxx is released under the LGPL-3.0 and/or BSD-3-Clause

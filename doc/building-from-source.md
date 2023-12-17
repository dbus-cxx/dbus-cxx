Building dbus-cxx from source {#building-from-source}
==

## 1. What are the requirements to build from source?

DBus-cxx requires support of at least C++17.  As of version 2.0, it is also a full
implementation of the DBus protocol, and does not depend on libdbus.

The requirements are intended to be very low.  In order to build, you will need the 
following tools to be installed:

* cmake(>=3.8)
* make
* g++

DBus-cxx depends on the following libraries:

* libsigc++(>=3.0)
* expat(not needed if tests and tools are disabled; tests are enabled by default)

(See section 4 for information on optional dependencies)

On Debian-based systems, you should be able to install with the following
commands(as root/sudo):

```
    apt-get install libdbus-1-dev cmake libexpat1-dev
```

Note that as of the writing of this guide(October 2020), libsigc++-3.0 has not yet
been added to the Debian repositoreis, and you will need to build this dependency
manually.

When checking out from GIT, the easiest way to build is as such:

```
    mkdir build
    cd build
    cmake ..
    make
    sudo make install
```

## 2. What are the requirements to build the tools?

In order to build the tools, you will need the following libraries, in addition
to the above:

* libpopt
* libcppgenerate
* libcppgenerate(bundled version included)

On Debian-based systems, you should be able to install popt with the following 
commands(as root/sudo):

    apt-get install libpopt-dev

In order to configure the tools, set -DENABLE\_TOOLS=ON when calling CMake

libcppgenerate can be found here: https://github.com/rm5248/libcppgenerate

libcppgenerate is bundled with dbus-cxx; to disable using the bundled version,
pass -DTOOLS\_BUNDLED\_CPPGENERATE=off when configuring.

## 3. Smart Pointer information

DBus-cxx requires at least C++17 to work properly.  Because the library makes
extensive use of smart pointers, it will use the standard C++ shared\_ptr for 
the implementation.

## 4. Documentation Generation

To generate the documentation, you will need:

* doxygen
* graphviz
* xsltproc

On Debian-based systems, you should be able to install with the following 
commands(as root/sudo):

    apt-get install doxygen graphviz xsltproc

## 6. Tools

There are two tools provided with dbus-cxx:

[dbus-cxx-xml2cpp](@ref xml2cpp) - this generates code based off of a DBus introspection file.
  An adapter is created when you want to implement a service, while a proxy
  is created when you want to talk with a remote object

dbus-cxx-introspect - print out the introspection XML for a specified service

## 7. Building a .deb package

If you are on a Debian/Ubuntu based system, you can build the .deb packages for DBus-cxx:

```
dpkg-buildpackage -us -uc -b
```

Newer versions of Debian(12+) and Ubuntu(23.04+) contain libsigc++-3.0, so this dependency
can be installed through apt.

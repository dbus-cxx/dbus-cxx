Building dbus-cxx from source {#building-from-source}
==

## 1. What are the requirements to build from source?

DBus-cxx requires support of at least C++11.

The requirements are very low.  In order to build, you will need the 
following tools to be installed:

* cmake(>=3.1)
* make
* g++

DBus-cxx depends on only two libraries:

* libsigc++
* dbus-1

(See section 4 for information on optional dependencies)

On Debian-based systems, you should be able to install with the following
commands(as root/sudo):

    apt-get install libsigc++-1.2-dev libdbus-1-dev cmake

NOTE: On Debian 8(Jessie) and below, you will have to enable the 
jessie-backports repository to install cmake:

    apt-get install cmake -t jessie-backports

When checking out from GIT, the easiest way to build is as such:

    mkdir build
    cd build
    cmake ..
    make
    sudo make install

## 2. What are the requirements to build the tools?

In order to build the tools, you will need the following libraries, in addition
to the above:

libpopt
libcppgenerate

On Debian-based systems, you should be able to install popt with the following 
commands(as root/sudo):

    apt-get install libpopt-dev

In order to configure the tools, set -DENABLE_TOOLS=ON when calling CMake

libcppgenerate can be found here: https://github.com/rm5248/libcppgenerate

## 3. Smart Pointer information

DBus-cxx requires at least C++11 to work properly.  Because the library makes
extensive use of smart pointers, it will use C++11 for the implementation.

## 4. Documentation Generation

To generate the documentation, you will need:

doxygen
graphviz
xsltproc


On Debian-based systems, you should be able to install with the following 
commands(as root/sudo):

    apt-get install doxygen graphviz xsltproc

## 6. Tools

There are two tools provided with dbus-cxx:

[dbus-cxx-xml2cpp](@ref xml2cpp) - this generates code based off of a DBus introspection file.
  An adapter is created when you want to implement a server, while a proxy
  is created when you want to talk with a remote object

dbus-cxx-introspect - print out the introspection XML for a specified service

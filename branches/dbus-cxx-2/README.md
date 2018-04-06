DBus-cxx

Table of contents:

1. What is DBus-cxx?
2. What are the requirements to build from source?
3. What are the requirements to build the tools?
4. Smart Pointer information
5. Documentation Generation

======================================
1. What is DBus-cxx?
======================================

DBus-cxx is a C++ wrapper for DBus.  DBus is a Linux-specific RPC(remote
procedure call) and IPC(inter-process communications) mechanism.  
DBus-cxx provides an object-oriented view of all programs on the DBus, and
allows an object-oriented way of exporting methods onto the DBus.


======================================
2. What are the requirements to build from source?
======================================

DBus-cxx requires support of at least C++11.

The requirements are very low.  In order to build, you will need the 
following tools to be installed:

cmake(>=3.1)
make
g++

DBus-cxx depends on only two libraries:

libsigc++
dbus-1

(See section 4 for information on optional dependencies)

On Debian-based systems, you should be able to install with the following
commands(as root/sudo):

```
apt-get install libsigc++-1.2-dev libdbus-1-dev cmake
```

NOTE: On Debian 8(Jessie) and below, you will have to enable the 
jessie-backports repository to install cmake:

```
apt-get install cmake -t jessie-backports
```

When checking out from SVN, build as such:

```
mkdir build
cd build
cmake ..
make
make install
```

======================================
3. What are the requirements to build the tools?
======================================

In order to build the tools, you will need the following libraries, in addition
to the above:

libpopt

On Debian-based systems, you should be able to install with the following 
commands(as root/sudo):

apt-get install libpopt-dev

In order to configure the tools, when doing ./configure from the main 
directory, make sure to pass --enable-tools to the configure script

======================================
4. Smart Pointer information
======================================

By default, DBus-cxx will compile with C++0x support for smart pointers.
If this is not the desired behavior, when running the configure script
pass --without-cpp as an argument.  DBus-cxx will then attempt to configure
itself with smart pointers from Boost and TR1.

======================================
5. Documentation Generation
======================================

To generate the documentation, you will need:

doxygen
graphviz
xsltproc


On Debian-based systems, you should be able to install with the following 
commands(as root/sudo):

```
apt-get install doxygen graphviz xsltproc
```

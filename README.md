# DBus-cxx

## What is DBus-cxx?

DBus-cxx is a C++ implementation for DBus.  DBus is a Linux-specific RPC(remote
procedure call) and IPC(inter-process communications) mechanism.  
DBus-cxx provides an object-oriented view of all programs on the DBus, and
allows an object-oriented way of exporting methods onto the DBus.

This new 2.0 version(that you are looking at now) is a complete ground-up
implementation of the DBus protocol, without libdbus, in part to fix a
number of known multithreading issues with libdbus.  This also allows for
templates to be used throughout the system, for methods, signals, and
properties.

Quick facts:

License: LGPL 3.0 or BSD-3-Clause

Website: http://dbus-cxx.github.io

### 3rd party components

In order to provide an easy build environment, a number of
dependencies are included in the repository.  The following table lists
these components that are distributed with dbus-cxx:

|Location|Usage|License|Original Location|
|--------|-----|-------|-----------------|
|cmake-modules|Provides certain CMake modules that are used in the build|Boost|https://github.com/bilke/cmake-modules|
|tools/libcppgenerate|Code generation library|Apache 2.0|https://github.com/rm5248/libcppgenerate|

# Legacy branch

This branch is for dbus-cxx-2; if you require a 0.y version, please look
at the [dbus-cxx-0.y] branch.

[dbus-cxx-0.y]: https://github.com/dbus-cxx/dbus-cxx/tree/dbus-cxx-0.y

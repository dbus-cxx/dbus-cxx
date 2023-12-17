dbus-cxx Library {#mainpage}
===


# About
dbus-cxx is a C++ implementation of the DBus protocol. It does not use the reference C
implementation of libdbus at all in order to provide a tight C++-like interface, and to
fix known multithreading problems with libdbus.  libsigc++ is used to help provide an
OO interface to the bus.
Also included is dbus-cxx-xml2cpp to generate proxy and adapter interfaces from DBus XML
introspection-like documents.

 
---
The dbus-cxx Github project page can be found here:

https://dbus-cxx.github.io

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
Documentation on using  [dbus-cxx-xml2cpp](@ref xml2cpp) to convert DBus XML introspection
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

Source: dbus-cxx
Priority: extra
Maintainer: Rick L. Vinyard, Jr. <rvinyard@cs.nmsu.edu>
XSBC-Original-Maintainer: Rick L. Vinyard, Jr. <rvinyard@cs.nmsu.edu>
Build-Depends: cdbs, quilt, debhelper (>= 7), autotools-dev, dbus, libdbus-1-dev, libsigc++-2.0-dev, libglibmm-2.4-dev, libpopt-dev, libexpat1-dev, doxygen, xsltproc
Standards-Version: 3.8.0
Section: libs
Homepage: http://dbus-cxx.sourceforge.net/

Package: dbus-cxx0
Section: libs
Architecture: any
Depends: ${shlibs:Depends}, ${misc:Depends}
Description: C++ bindings for the D-Bus library
 C++ bindings for the D-Bus library

Package: dbus-cxx-dev
Section: libdevel
Architecture: any
Depends: dbus-cxx0 (= ${binary:Version})
Description: C++ development headers for dbus-cxx
 This package contains the C++ libraries and header files needed for
 developing dbus-cxx applications.
 dbus-cxx provides C++ bindings for the D-Bus library.

Package: dbus-cxx-doc
Section: doc
Architecture: any
Depends: dbus-cxx-dev (= ${binary:Version})
Description: Developer's documentation for the dbus-cxx library
 This package contains developer's documentation for the dbus-cxx
 library. dbus-cxx provides C++ based bindings for the C based
 dbus library.
 The documentation can be viewed either through the devhelp
 documentation browser or through a web browser.
 If using a web browser the documentation is installed in the gtk-doc
 hierarchy and can be found at /usr/share/gtk-doc/html/dbus-cxx-API_VERSION

Package: dbus-cxx-tools
Section: utils
Architecture: any
Depends: dbus-cxx0 (= ${binary:Version}), ${shlibs:Depends}, ${misc:Depends}
Description: Tools to support dbus-cxx application development
 This package contains tools to assist with dbus-cxx application development.
 * dbus-cxx-xml2cpp:
   This application can be used to create external class proxies and adaptors
   from a D-Bus Introspection-like xml specification.
 * dbus-cxx-introspect:
   Command line utility to simplify introspection

Package: dbus-cxx-glibmm0
Section: libs
Architecture: any
Depends: dbus-cxx0 (= ${binary:Version}), ${shlibs:Depends}, ${misc:Depends}
Description: Adds support to dbus-cxx (C++ bindings for dbus) for glibmm integration
 Adds support to dbus-cxx (C++ bindings for dbus) for glibmm integration

Package: dbus-cxx-glibmm-dev
Section: libdevel
Architecture: any
Depends: dbus-cxx-glibmm0 (= ${binary:Version})
Description: Headers for developing programs that will use dbus-cxx-glibmm
 dbus-cxx provides C++ bindings for the dbus library. dbus-cxx-glibmm
 provides dbus-cxx a means of integrating with glibmm.
 Since gtkmm relies on glibmm for main-loop operations this library
 can also be used to integrate dbus-cxx with gtkmm applications.
 This package contains the libraries and header files needed for
 developing dbus-cxx-glibmm applications.

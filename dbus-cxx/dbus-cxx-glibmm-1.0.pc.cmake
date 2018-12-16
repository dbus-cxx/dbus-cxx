Name: dbus-cxx-glibmm
Description: C++ bindings for dbus(glibmm support)
Version: ${PKG_VERSION}
Requires: ${PKG_CONFIG_REQUIRES}, dbus-cxx-1.0 = ${PKG_VERSION}
prefix=${CMAKE_INSTALL_PREFIX}
includedir=${PKG_CONFIG_INCLUDEDIR}
libdir=${PKG_CONFIG_LIBDIR}
Libs: ${PKG_CONFIG_LIBS_MM}
Cflags: ${PKG_CONFIG_CFLAGS}

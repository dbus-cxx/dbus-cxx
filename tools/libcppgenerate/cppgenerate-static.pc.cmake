Name: cppgenerate-static
Description: A library for generating C++ code from code
Version: ${PROJECT_VERSION}
Requires: ${PKG_CONFIG_REQUIRES}
prefix=${CMAKE_INSTALL_PREFIX}
includedir=${PKG_CONFIG_INCLUDEDIR}
libdir=${PKG_CONFIG_LIBDIR}
Libs: ${PKG_CONFIG_STATIC_LIBS}
Cflags: ${PKG_CONFIG_CFLAGS}

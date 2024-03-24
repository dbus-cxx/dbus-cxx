#   This file is part of the dbus-cxx library.
#
#   The dbus-cxx library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public License
#   version 3 as published by the Free Software Foundation.
#
#   The dbus-cxx library is distributed in the hope that it will be
#   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
#   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#   General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this software. If not see <http://www.gnu.org/licenses/>.

pkg_check_modules( glib REQUIRED IMPORTED_TARGET glib-2.0 )

set( dbus-cxx-glib-headers dbus-cxx-glib.h glibdispatcher.h )
set( dbus-cxx-glib-sources glibdispatcher.cpp )

add_library( dbus-cxx-glib SHARED ${dbus-cxx-glib-sources} ${dbus-cxx-glib-headers} )
set_target_properties( dbus-cxx-glib PROPERTIES
    VERSION 2.0.0 SOVERSION 2
    PUBLIC_HEADER "${dbus-cxx-glib-headers}"
)
target_link_libraries( dbus-cxx-glib PUBLIC dbus-cxx PkgConfig::sigc ${LIBRT} )
target_include_directories( dbus-cxx-glib INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/dbus-cxx-glib-${DBUS_CXX_INCLUDE_VERSION}>
)
target_link_libraries( dbus-cxx-glib PUBLIC PkgConfig::glib )

set_property( TARGET dbus-cxx-glib PROPERTY CXX_STANDARD 17 )

if( BUILD_TESTING )
    add_subdirectory( unit-tests )
endif( BUILD_TESTING )

#
# pkg-config script creation and install
#
SET(PKG_CONFIG_LIBDIR
    "\${prefix}/lib"
)
SET(PKG_CONFIG_INCLUDEDIR
    "\${prefix}/include/dbus-cxx-glib-${DBUS_CXX_INCLUDE_VERSION}"
)
SET(PKG_CONFIG_LIBS
    "-L\${libdir} -ldbus-cxx-glib"
)
SET(PKG_CONFIG_CFLAGS
    "-I\${includedir}"
)
SET(PKG_CONFIG_REQUIRES
    "dbus-cxx-2.0, glib-2.0"
)

CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/dbus-cxx-glib-2.0.pc.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/dbus-cxx-glib-2.0.pc"
)
INSTALL( FILES "${CMAKE_CURRENT_BINARY_DIR}/dbus-cxx-glib-2.0.pc"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

#
# Library install information
#
install( TARGETS dbus-cxx-glib
    EXPORT dbus-cxxTargets
    COMPONENT glib
    PUBLIC_HEADER DESTINATION include/dbus-cxx-glib-${DBUS_CXX_INCLUDE_VERSION}
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" )

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

pkg_search_module( LIBUV REQUIRED libuv )

set( dbus-cxx-uv-headers dbus-cxx-uv.h uvdispatcher.h )
set( dbus-cxx-uv-sources dbus-cxx-uv.cpp uvdispatcher.cpp )

add_library( dbus-cxx-uv SHARED ${dbus-cxx-uv-sources} ${dbus-cxx-uv-headers} )
set_target_properties( dbus-cxx-uv PROPERTIES
    VERSION 2.0.0 SOVERSION 2
    PUBLIC_HEADER "${dbus-cxx-uv-headers}"
)
target_link_libraries( dbus-cxx-uv PUBLIC dbus-cxx ${sigc_LIBS} -lrt )
target_include_directories( dbus-cxx-uv INTERFACE
    $<INSTALL_INTERFACE:include/dbus-cxx-uv-${DBUS_CXX_INCLUDE_VERSION}>
)
target_link_libraries( dbus-cxx-uv PUBLIC ${LIBUV_LIBRARIES} )

set_property( TARGET dbus-cxx-uv PROPERTY CXX_STANDARD 17 )

if( BUILD_TESTING )
    #add_subdirectory( unit-tests )
endif( BUILD_TESTING )

#
# pkg-config script creation and install
#
SET(PKG_CONFIG_LIBDIR
    "\${prefix}/lib"
)
SET(PKG_CONFIG_INCLUDEDIR
    "\${prefix}/include/dbus-cxx-uv-${DBUS_CXX_INCLUDE_VERSION}"
)
SET(PKG_CONFIG_LIBS
    "-L\${libdir} -ldbus-cxx-uv"
)
SET(PKG_CONFIG_CFLAGS
    "-I\${includedir}"
)
SET(PKG_CONFIG_REQUIRES
    "dbus-cxx-2.0, libuv"
)

CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/dbus-cxx-uv-2.0.pc.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/dbus-cxx-uv-2.0.pc"
)

INSTALL( FILES "${CMAKE_CURRENT_BINARY_DIR}/dbus-cxx-uv-2.0.pc"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

#
# Library install information
#
install( TARGETS dbus-cxx-uv
    EXPORT dbus-cxxTargets
    COMPONENT uv
    PUBLIC_HEADER DESTINATION include/dbus-cxx-uv-${DBUS_CXX_INCLUDE_VERSION}
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" )

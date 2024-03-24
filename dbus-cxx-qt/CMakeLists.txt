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

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core REQUIRED)

set( dbus-cxx-qt-headers dbus-cxx-qt.h qtthreaddispatcher.h qtdispatcher.h )
set( dbus-cxx-qt-sources dbus-cxx-qt.cpp qtthreaddispatcher.cpp qtdispatcher.cpp )

add_compile_definitions( "QT_NO_KEYWORDS" )

add_library( dbus-cxx-qt SHARED ${dbus-cxx-qt-sources} ${dbus-cxx-qt-headers} )
set_target_properties( dbus-cxx-qt PROPERTIES
    VERSION 2.0.0 SOVERSION 2
    PUBLIC_HEADER "${dbus-cxx-qt-headers}"
)
target_link_libraries( dbus-cxx-qt PUBLIC Qt5::Core dbus-cxx PkgConfig::sigc ${LIBRT} )
target_include_directories( dbus-cxx-qt INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/dbus-cxx-qt-${DBUS_CXX_INCLUDE_VERSION}>
)

set_property( TARGET dbus-cxx-qt PROPERTY CXX_STANDARD 17 )

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
    "\${prefix}/include/dbus-cxx-qt-${DBUS_CXX_INCLUDE_VERSION}"
)
SET(PKG_CONFIG_LIBS
    "-L\${libdir} -ldbus-cxx-qt"
)
SET(PKG_CONFIG_CFLAGS
    "-I\${includedir}"
)
SET(PKG_CONFIG_REQUIRES
    "dbus-cxx-2.0"
)

CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/dbus-cxx-qt-2.0.pc.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/dbus-cxx-qt-2.0.pc"
)

INSTALL( FILES "${CMAKE_CURRENT_BINARY_DIR}/dbus-cxx-qt-2.0.pc"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

#
# Library install information
#
install( TARGETS dbus-cxx-qt
    EXPORT dbus-cxxTargets
    COMPONENT qt
    PUBLIC_HEADER DESTINATION include/dbus-cxx-qt-${DBUS_CXX_INCLUDE_VERSION}
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" )

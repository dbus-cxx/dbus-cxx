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

pkg_check_modules( expat REQUIRED IMPORTED_TARGET expat )
pkg_check_modules( popt REQUIRED IMPORTED_TARGET popt )

set( TEST_INCLUDES ${cppgenerate_INCLUDE_DIRS} )
set( TEST_LINK dbus-cxx dbus-cxx-glib PkgConfig::glib PkgConfig::sigc PkgConfig::popt PkgConfig::expat ${LIBRT} )

link_directories( ${CMAKE_BINARY_DIR} )

include_directories( ${CMAKE_SOURCE_DIR}/dbus-cxx
    ${CMAKE_BINARY_DIR}/dbus-cxx
    ${CMAKE_SOURCE_DIR}/dbus-cxx-glib )

add_executable( test-glib-dispatcher test-glib-dispatcher.cpp calleeclass.cpp )
target_link_libraries( test-glib-dispatcher ${TEST_LINK} )
target_include_directories( test-glib-dispatcher PUBLIC ${CMAKE_SOURCE_DIR} )
target_include_directories( test-glib-dispatcher PUBLIC ${CMAKE_CURRENT_BINARY_DIR} )
set_property( TARGET test-glib-dispatcher PROPERTY CXX_STANDARD 17 )

add_executable( glib-caller glib-caller.cpp )
target_link_libraries( glib-caller ${TEST_LINK} )
target_include_directories( glib-caller PUBLIC ${CMAKE_SOURCE_DIR} )
target_include_directories( glib-caller PUBLIC ${CMAKE_CURRENT_BINARY_DIR} )
set_property( TARGET glib-caller PROPERTY CXX_STANDARD 17 )

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/dbus-wrapper-glib-tests.sh
    ${CMAKE_CURRENT_BINARY_DIR}/dbus-wrapper-glib-tests.sh COPYONLY)

add_test( NAME glib-dispatcher COMMAND dbus-wrapper-glib-tests.sh glib-dispatcher)
#add_test( NAME glib-threaddispatcher COMMAND dbus-wrapper-glib-tests.sh standalone)

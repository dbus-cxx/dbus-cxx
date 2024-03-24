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

pkg_check_modules( expat REQUIRED IMPORTED_TARGET expat )
pkg_check_modules( popt REQUIRED IMPORTED_TARGET popt )

set( TEST_INCLUDES ${cppgenerate_INCLUDE_DIRS} )
set( TEST_LINK dbus-cxx dbus-cxx-qt Qt5::Core PkgConfig::sigc PkgConfig::popt PkgConfig::expat ${LIBRT} )

link_directories( ${CMAKE_BINARY_DIR} )

include_directories( ${CMAKE_SOURCE_DIR}/dbus-cxx
    ${CMAKE_BINARY_DIR}/dbus-cxx
    ${CMAKE_SOURCE_DIR}/dbus-cxx-qt )

add_executable( test-qt-dispatcher test-qt-dispatcher.cpp calleeclass.cpp )
target_link_libraries( test-qt-dispatcher ${TEST_LINK} )
target_include_directories( test-qt-dispatcher PUBLIC ${CMAKE_SOURCE_DIR} )
target_include_directories( test-qt-dispatcher PUBLIC ${CMAKE_CURRENT_BINARY_DIR} )
set_property( TARGET test-qt-dispatcher PROPERTY CXX_STANDARD 17 )

add_executable( qt-caller qt-caller.cpp )
target_link_libraries( qt-caller ${TEST_LINK} )
target_include_directories( qt-caller PUBLIC ${CMAKE_SOURCE_DIR} )
target_include_directories( qt-caller PUBLIC ${CMAKE_CURRENT_BINARY_DIR} )
set_property( TARGET qt-caller PROPERTY CXX_STANDARD 17 )

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/dbus-wrapper-qt-tests.sh
    ${CMAKE_CURRENT_BINARY_DIR}/dbus-wrapper-qt-tests.sh COPYONLY)

add_test( NAME qt-dispatcher COMMAND dbus-wrapper-qt-tests.sh qt-dispatcher)
add_test( NAME qt-threaddispatcher COMMAND dbus-wrapper-qt-tests.sh standalone)

#
# Recursive tests using the Qt dispatcher
#
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/dbus-wrapper-recursive-signal.sh
    ${CMAKE_CURRENT_BINARY_DIR}/dbus-wrapper-recursive-signal.sh COPYONLY)
add_executable( test-recursive-signal-qt recursive-signal-test-qt.cpp )
target_link_libraries( test-recursive-signal-qt ${TEST_LINK} )
target_include_directories( test-recursive-signal-qt PUBLIC ${CMAKE_SOURCE_DIR} )
target_include_directories( test-recursive-signal-qt PUBLIC ${CMAKE_CURRENT_BINARY_DIR} )
set_property( TARGET test-recursive-signal-qt PROPERTY CXX_STANDARD 17 )

add_test( NAME recursive-from-signal-qt COMMAND dbus-wrapper-recursive-signal.sh test)

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

add_library( dbus-cxx-qt SHARED ${dbus-cxx-qt-sources} )
set_target_properties( dbus-cxx-qt PROPERTIES VERSION 2.0.0 SOVERSION 2 )
target_link_libraries( dbus-cxx-qt Qt5::Core dbus-cxx ${sigc_LIBS} -lrt )

set_property( TARGET dbus-cxx-qt PROPERTY CXX_STANDARD 17 )

if( BUILD_TESTING )
    add_subdirectory( unit-tests )
endif( BUILD_TESTING )

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

project( dbus-cxx )

cmake_minimum_required( VERSION 3.1 )

include( CheckIncludeFiles )
include( GNUInstallDirs )
include( FindPkgConfig )

# version information
set( DBUS_CXX_PACKAGE_MAJOR_VERSION 0 )
set( DBUS_CXX_PACKAGE_MINOR_VERSION 10 )
set( DBUS_CXX_PACKAGE_MICRO_VERSION 0 )
set( PKG_VERSION ${DBUS_CXX_PACKAGE_MAJOR_VERSION}.${DBUS_CXX_PACKAGE_MINOR_VERSION}.${DBUS_CXX_PACKAGE_MICRO_VERSION} )
set( DBUS_CXX_MAJMIN_VERSION ${DBUS_CXX_PACKAGE_MAJOR_VERSION}.${DBUS_CXX_PACKAGE_MINOR_VERSION} )

# Our required dependencies are libdbus and libsigc++
pkg_check_modules( dbus REQUIRED dbus-1>=1.3 )
pkg_check_modules( sigc REQUIRED sigc++-2.0 )

# Dbus-cxx requires at least C++11
set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

#
# Check our options
#

#
# Configure our compile options
#
configure_file( dbus-cxx-config.h.cmake dbus-cxx/dbus-cxx-config.h )

# 
# All sources
#
set( DBUS_CXX_SOURCES
    dbus-cxx/callmessage.cpp
    dbus-cxx/connection.cpp
    dbus-cxx/dispatcher.cpp
    dbus-cxx/error.cpp
    dbus-cxx/errormessage.cpp
    dbus-cxx/interface.cpp
    dbus-cxx/interfaceproxy.cpp
    dbus-cxx/messageappenditerator.cpp
    dbus-cxx/message.cpp
    dbus-cxx/messagefilter.cpp
    dbus-cxx/messagehandler.cpp
    dbus-cxx/messageiterator.cpp
    dbus-cxx/methodbase.cpp
    dbus-cxx/methodproxybase.cpp
    dbus-cxx/object.cpp
    dbus-cxx/objectpathhandler.cpp
    dbus-cxx/objectproxy.cpp
    dbus-cxx/path.cpp
    dbus-cxx/pendingcall.cpp
    dbus-cxx/returnmessage.cpp
    dbus-cxx/signal_base.cpp
    dbus-cxx/signalmessage.cpp
    dbus-cxx/signal_proxy_base.cpp
    dbus-cxx/signalreceiver.cpp
    dbus-cxx/signature.cpp
    dbus-cxx/signatureiterator.cpp
    dbus-cxx/timeout.cpp
    dbus-cxx/utility.cpp
    dbus-cxx/watch.cpp )

# headers that need to go in the include/ directory
set( DBUS_CXX_HEADERS1
    dbus-cxx.h
)

# headers that need to go in the include/dbus-cxx directory
set( DBUS_CXX_HEADERS
    dbus-cxx/connection.h
    dbus-cxx/object.h

    dbus-cxx/accumulators.h
    dbus-cxx/callmessage.h
    dbus-cxx/dbus-cxx-private.h
    dbus-cxx/dispatcher.h
    dbus-cxx/enums.h
    dbus-cxx/error.h
    dbus-cxx/errormessage.h
    dbus-cxx/filedescriptor.h
    dbus-cxx/forward_decls.h
    dbus-cxx/headerlog.h
    dbus-cxx/messageappenditerator.h
    dbus-cxx/messagefilter.h
    dbus-cxx/message.h
    dbus-cxx/messagehandler.h
    dbus-cxx/messageiterator.h
    dbus-cxx/methodbase.h
    dbus-cxx/objectpathhandler.h
    dbus-cxx/path.h
    dbus-cxx/pendingcall.h
    dbus-cxx/pointer.h
    dbus-cxx/returnmessage.h
    dbus-cxx/signal_base.h
    dbus-cxx/signalmessage.h
    dbus-cxx/signal_proxy_base.h
    dbus-cxx/signalreceiver.h
    dbus-cxx/signature.h
    dbus-cxx/signatureiterator.h
    dbus-cxx/simplelogger_defs.h
    dbus-cxx/simplelogger.h
    dbus-cxx/timeout.h
    dbus-cxx/types.h
    dbus-cxx/utility.h
    dbus-cxx/variant.h
    dbus-cxx/watch.h
    ${CMAKE_CURRENT_BINARY_DIR}/dbus-cxx/dbus-cxx-config.h
)

# We need to run m4 on some files
#foreach( filename 
#    dbus-cxx/dbus_signal.h.m4
#    dbus-cxx/interface.h.m4
#    dbus-cxx/interfaceproxy.h.m4
#    dbus-cxx/method.h.m4
#    dbus-cxx/method_impl.h.m4
#    dbus-cxx/methodproxybase.h.m4
#    dbus-cxx/methodproxy.h.m4
#    dbus-cxx/objectproxy.h.m4
#    dbus-cxx/signal_proxy.h.m4
#    dbus-cxx/template.macros.m4 )
#
#    string( REPLACE ".m4" "" sanitized_file ${filename} )
#    # Due to how the dependency tracking works, we must generate the m4 files when cmake is run
#    execute_process( COMMAND m4 ${CMAKE_CURRENT_SOURCE_DIR}/${filename} 
#        OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${sanitized_file} )
    # But we also add it to a custom command if the file changes
    #add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${sanitized_file}
    #    COMMAND m4 -P -s ${filename} > ${CMAKE_CURRENT_BINARY_DIR}/${sanitized_file}
    #    MAIN_DEPENDENCY ${filename}
    #    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    #    VERBATIM
    #)
#endforeach( filename )

set( DBUS_CXX_INCLUDE_DIRECTORIES 
    ${CMAKE_CURRENT_SOURCE_DIR} 
    ${CMAKE_CURRENT_SOURCE_DIR}/dbus-cxx
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/dbus-cxx )
include_directories( ${DBUS_CXX_INCLUDE_DIRECTORIES} 
    ${dbus_INCLUDE_DIRS} 
    ${sigc_INCLUDE_DIRS} )

#
# Target for the library
#
add_library( dbus-cxx SHARED ${DBUS_CXX_SOURCES} )
#add_library( dbus-cxx-static STATIC ${DBUS_CXX_SOURCES} )
set_target_properties( dbus-cxx PROPERTIES VERSION 1.0.0 SOVERSION 1 )

#
# Library install information
#
set( install_targets ${install_targets} dbus-cxx )
install( TARGETS ${install_targets}
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" )
install( FILES ${DBUS_CXX_HEADERS} DESTINATION include/dbus-cxx-${DBUS_CXX_MAJMIN_VERSION}/dbus-cxx )
install( FILES ${DBUS_CXX_HEADERS1} DESTINATION include/dbus-cxx-${DBUS_CXX_MAJMIN_VERSION} )

#
# pkg-config script creation and install
#
SET(PKG_CONFIG_LIBDIR
    "\${prefix}/lib"
)
SET(PKG_CONFIG_INCLUDEDIR
    "\${prefix}/include/dbus-cxx-${DBUS_CXX_MAJMIN_VERSION}"
)
SET(PKG_CONFIG_LIBS
    "-L\${libdir} -ldbus-cxx"
)
SET(PKG_CONFIG_CFLAGS
    "-I\${includedir} -std=c++11"
)
SET(PKG_CONFIG_REQUIRES
    "dbus-1 >= 1.3, sigc++-2.0"
)

CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/dbus-cxx-2.0.pc.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/dbus-cxx-2.0.pc"
)
INSTALL( FILES "${CMAKE_BINARY_DIR}/dbus-cxx-2.0.pc"
        DESTINATION lib/pkgconfig)

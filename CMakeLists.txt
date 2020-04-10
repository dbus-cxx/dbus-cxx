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

cmake_minimum_required( VERSION 3.8 )

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules")

include( CheckIncludeFiles )
include( GNUInstallDirs )
include( FindPkgConfig )
include( CheckTypeSize )
include( CTest )
include( CheckCXXSymbolExists )
IF( CMAKE_BUILD_TYPE MATCHES Debug )
include( CodeCoverage )
ENDIF( CMAKE_BUILD_TYPE MATCHES Debug )

# version information
set( DBUS_CXX_PACKAGE_MAJOR_VERSION 2 )
set( DBUS_CXX_PACKAGE_MINOR_VERSION 0 )
set( DBUS_CXX_PACKAGE_MICRO_VERSION 0 )
set( PKG_VERSION ${DBUS_CXX_PACKAGE_MAJOR_VERSION}.${DBUS_CXX_PACKAGE_MINOR_VERSION}.${DBUS_CXX_PACKAGE_MICRO_VERSION} )
set( DBUS_CXX_MAJMIN_VERSION ${DBUS_CXX_PACKAGE_MAJOR_VERSION}.${DBUS_CXX_PACKAGE_MINOR_VERSION} )

# Our required dependencies are libdbus and libsigc++
pkg_check_modules( dbus REQUIRED dbus-1>=1.3 )
pkg_check_modules( sigc REQUIRED sigc++-3.0 )

#
# Check our options
#
option( ENABLE_EXAMPLES "Enable the examples" OFF )
option( ENABLE_TOOLS "Enable dbus-cxx tools" OFF )
option( ENABLE_GLIBMM "Enable GLibMM support" OFF )
option( BUILD_SITE "Build the dbus-cxx website reference" OFF )
option( TOOLS_BUNDLED_CPPGENERATE "Use bundled libcppgenerate" ON )
option( ENABLE_CODE_COVERAGE_REPORT "Enable code coverage report" OFF )
if( CMAKE_BUILD_TYPE MATCHES Debug )
option( ENABLE_ASAN "Enable ASAN for finding memory bugs" OFF )
endif()

#
# Configure our compile options
#
set( DBUS_CXX_HAVE_DBUS_12 1 )
CHECK_TYPE_SIZE( "long int" DBUS_CXX_SIZEOF_LONG_INT )
configure_file( dbus-cxx-config.h.cmake dbus-cxx/dbus-cxx-config.h )
if( ${ENABLE_ASAN} )
	set( CMAKE_CXX_FLAGS "-fsanitize=address -fsanitize=null -fsanitize=return -fsanitize=object-size -fsanitize=bool -fsanitize=enum" )
endif()

# Check for cxxabi.h so we can demangle symbols
check_include_file_cxx( "cxxabi.h" DBUS_CXX_HAS_CXXABI_H )
# Check to make sure the demangle symbol exists
if( ${DBUS_CXX_HAS_CXXABI_H} )
	check_cxx_symbol_exists( "abi::__cxa_demangle" "cxxabi.h" DBUS_CXX_HAS_CXA_DEMANGLE )
endif( ${DBUS_CXX_HAS_CXXABI_H} )

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
    dbus-cxx/signature.cpp
    dbus-cxx/signatureiterator.cpp
    dbus-cxx/utility.cpp
    dbus-cxx/types.cpp
    dbus-cxx/variant.cpp
    dbus-cxx/marshaling.cpp
    dbus-cxx/demarshaling.cpp
    dbus-cxx/simpletransport.cpp
    dbus-cxx/sendmsgtransport.cpp
    dbus-cxx/transport.cpp
    dbus-cxx/sasl.cpp
    dbus-cxx/validator.cpp
    dbus-cxx/daemon-proxy/DBusDaemonProxy.cpp
)

# headers that need to go in the include/ directory
set( DBUS_CXX_HEADERS1
    dbus-cxx.h
)

# headers that need to go in the include/dbus-cxx directory
set( DBUS_CXX_HEADERS
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
    dbus-cxx/returnmessage.h
    dbus-cxx/signal_base.h
    dbus-cxx/signalmessage.h
    dbus-cxx/signal_proxy_base.h
    dbus-cxx/signature.h
    dbus-cxx/signatureiterator.h
    dbus-cxx/simplelogger_defs.h
    dbus-cxx/simplelogger.h
    dbus-cxx/types.h
    dbus-cxx/utility.h
    dbus-cxx/demangle.h
    dbus-cxx/dbus_signal.h
    dbus-cxx/interface.h
    dbus-cxx/interfaceproxy.h
    dbus-cxx/methodproxybase.h
    dbus-cxx/objectproxy.h
    dbus-cxx/connection.h
    dbus-cxx/object.h
    dbus-cxx/variant.h
    dbus-cxx/transport.h
    dbus-cxx/simpletransport.h
    dbus-cxx/sendmsgtransport.h
    dbus-cxx/marshaling.h
    dbus-cxx/demarshaling.h
    dbus-cxx/sasl.h
    dbus-cxx/dbus-error.h
    dbus-cxx/threaddispatcher.h
    dbus-cxx/validator.h
    ${CMAKE_CURRENT_BINARY_DIR}/dbus-cxx/dbus-cxx-config.h
)

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
set_target_properties( dbus-cxx PROPERTIES VERSION 2.0.0 SOVERSION 2 )
target_link_libraries( dbus-cxx ${dbus_LIBS} ${sigc_LIBS} -lrt )

# Dbus-cxx requires at least C++17 due to libsigc++-3.0
set_property( TARGET dbus-cxx PROPERTY CXX_STANDARD 17 )

#
# If GLIBMM is used, add in the appropriate packages
#
if( ENABLE_GLIBMM )
    add_library( dbus-cxx-glib SHARED dbus-cxx-glib/dispatcher.cpp )
    set_target_properties( dbus-cxx-glib PROPERTIES VERSION 2.0.0 SOVERSION 2 )
    #target_include_directories( dbus-cxx-glib PRIVATE  )
    add_dependencies( dbus-cxx-glib dbus-cxx )
    set_property( TARGET dbus-cxx-glib PROPERTY CXX_STANDARD 17 )

    set( install_targets ${install_targets} dbus-cxx-glib )
    install( TARGETS ${install_targets}
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" )
    install( FILES dbus-cxx-glib.h DESTINATION include/dbus-cxx-${DBUS_CXX_MAJMIN_VERSION} )
    install( FILES dbus-cxx-glib/dispatcher.h DESTINATION include/dbus-cxx-${DBUS_CXX_MAJMIN_VERSION}/dbus-cxx-glib/ )
endif( ENABLE_GLIBMM )

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
SET(PKG_CONFIG_LIBS_GLIB
    "-L\${libdir} -ldbus-cxx-glib"
)
SET(PKG_CONFIG_CFLAGS
    "-I\${includedir}"
)
SET(PKG_CONFIG_REQUIRES
    "dbus-1 >= 1.3, sigc++-3.0"
)

CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/dbus-cxx-2.0.pc.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/dbus-cxx-2.0.pc"
)
INSTALL( FILES "${CMAKE_BINARY_DIR}/dbus-cxx-2.0.pc"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

if( ENABLE_GLIBMM )
    CONFIGURE_FILE(
        "${CMAKE_CURRENT_SOURCE_DIR}/dbus-cxx-glib-2.0.pc.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/dbus-cxx-glib-2.0.pc"
    )
    INSTALL( FILES "${CMAKE_BINARY_DIR}/dbus-cxx-glib-2.0.pc"
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
endif( ENABLE_GLIBMM )

#
# Include the directory for the examples
#
if( ENABLE_EXAMPLES )
    add_subdirectory( examples )
endif( ENABLE_EXAMPLES )

#
# Include the directory for the tools
#
if( ENABLE_TOOLS )
    pkg_check_modules( expat REQUIRED expat )
    add_subdirectory( tools )
endif( ENABLE_TOOLS )

#
# If we want to build the site, we must have doxygen
#
if( BUILD_SITE )
    find_package( Doxygen 
                  REQUIRED dot )
    file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/doc/
          DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/doc/ )
    configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile
                    ${CMAKE_CURRENT_BINARY_DIR}/doc @ONLY )

    add_custom_target( doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
        DEPENDS dbus-cxx
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
    add_custom_target( tar_site ALL
        COMMAND tar -czf dbus-cxx-website-${PKG_VERSION}.tar.gz -C doc/reference/html .
        DEPENDS doc_doxygen
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        VERBATIM )
endif( BUILD_SITE )

#
# Check if tests are enabled
#
if( BUILD_TESTING )
    pkg_check_modules( expat REQUIRED expat )
    add_subdirectory( unit-tests )
    if( ENABLE_TOOLS )
        add_subdirectory( unit-tests/tools-tests )
    endif( ENABLE_TOOLS )
endif( BUILD_TESTING )

#
# See if code coverage is enabled
#
if( ENABLE_CODE_COVERAGE_REPORT )
    APPEND_COVERAGE_COMPILER_FLAGS()
    set(COVERAGE_LCOV_EXCLUDES 'unit-tests/*' '/usr/include/*' '/usr/local/include/*' )
    SETUP_TARGET_FOR_COVERAGE_LCOV(
        NAME dbus-cxx-coverage
        EXECUTABLE "ctest" )
endif( ENABLE_CODE_COVERAGE_REPORT )

#
# Cpack for creating the dist files
#
set(CPACK_SOURCE_PACKAGE_FILE_NAME "dbus-cxx-${PKG_VERSION}")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES ".git/;build/")
include(CPack)

#
# Print configuration information for the user
#
message(STATUS "")
message(STATUS "")
message(STATUS "dbus-cxx configuration summary:")
message(STATUS "")

message(STATUS "  Build type ...................... : ${CMAKE_BUILD_TYPE}")
message(STATUS "  Install prefix .................. : ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Library location ................ : ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
message(STATUS "  C++ compiler .................... : ${CMAKE_CXX_COMPILER}")
message(STATUS "  Build examples .................. : ${ENABLE_EXAMPLES}")
message(STATUS "  Build tests ..................... : ${BUILD_TESTING}")
message(STATUS "  Build tools ..................... : ${ENABLE_TOOLS}")
message(STATUS "  Use bundled cppgenerate ......... : ${TOOLS_BUNDLED_CPPGENERATE}")
message(STATUS "  Enable GLibmm support ........... : ${ENABLE_GLIBMM}")
message(STATUS "  Build website ................... : ${BUILD_SITE}")
message(STATUS "  Enable code coverage report ..... : ${ENABLE_CODE_COVERAGE_REPORT}")
if( CMAKE_BUILD_TYPE MATCHES Debug )
message(STATUS "  libasan enabled ................. : ${ENABLE_ASAN}")
endif()

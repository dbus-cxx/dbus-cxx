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
cmake_minimum_required( VERSION 3.12 )
project( dbus-cxx VERSION 2.4.0 )

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake-modules")

include( CheckIncludeFiles )
include( GNUInstallDirs )
include( FindPkgConfig )
include( CheckTypeSize )
include( CTest )
include( CheckCXXSymbolExists )
include( CheckCXXCompilerFlag )
IF( CMAKE_BUILD_TYPE MATCHES Debug )
include( CodeCoverage )
ENDIF( CMAKE_BUILD_TYPE MATCHES Debug )

# The INCLUDE_VERSION variable is used for includes
# it is related to, but not the same as, the normal project version
set( DBUS_CXX_INCLUDE_VERSION 2.0 )

# version information
set( PKG_VERSION ${dbus-cxx_VERSION} )

# Our required dependencies: libsigc++ 3.0
pkg_check_modules( sigc REQUIRED sigc++-3.0 )

#
# Check our options
#
option( ENABLE_EXAMPLES "Enable the examples" OFF )
option( ENABLE_TOOLS "Enable dbus-cxx tools" OFF )
option( ENABLE_GLIB_SUPPORT "Enable GLib support" OFF )
option( BUILD_SITE "Build the dbus-cxx website reference" OFF )
option( TOOLS_BUNDLED_CPPGENERATE "Use bundled libcppgenerate" ON )
option( ENABLE_CODE_COVERAGE_REPORT "Enable code coverage report" OFF )
if( CMAKE_BUILD_TYPE MATCHES Debug )
option( ENABLE_ASAN "Enable ASAN for finding memory bugs" OFF )
endif()
if( BUILD_TESTING )
option( ENABLE_ROBUSTNESS_TESTS "Enable extended robustness tests.  These can be long-running tests." OFF)
endif( BUILD_TESTING )
option( ENABLE_QT_SUPPORT "Build libdbuscxx-qt for integration with Qt applications" OFF )
option( ENABLE_UV_SUPPORT "Build libdbuscxx-uv for integration with libuv applications" OFF )

#
# Configure our compile options
#
configure_file( dbus-cxx-config.h.cmake dbus-cxx/dbus-cxx-config.h )
if( ${ENABLE_ASAN} )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
            -fsanitize=address \
            -fsanitize=null \
            -fsanitize=return \
            -fsanitize=object-size \
            -fsanitize=bool \
            -fsanitize=enum" )
endif()

if( UNIX )
    # WIN32 has '#define interface struct', so we'll do the same thing.
    # This should not affect anything in the code, but it will cause it to not compile on
    # non-Windows systems
    add_compile_definitions( interface=struct )

    # MSVC doesn't allow for keywords like 'and' and 'not', so check to see if we
    # can turn that on with GCC/Clang
    check_cxx_compiler_flag( "-fno-operator-names" NO_OP_NAMES )
    if( ${NO_OP_NAMES} )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-operator-names" )
    endif( ${NO_OP_NAMES} )
endif( UNIX )

# Check for cxxabi.h so we can demangle symbols
check_include_file_cxx( "cxxabi.h" DBUS_CXX_HAS_CXXABI_H )
# Check to make sure the demangle symbol exists
if( ${DBUS_CXX_HAS_CXXABI_H} )
	check_cxx_symbol_exists( "abi::__cxa_demangle" "cxxabi.h" DBUS_CXX_HAS_CXA_DEMANGLE )
endif( ${DBUS_CXX_HAS_CXXABI_H} )

# Check for std::propaogate_const
try_compile( DBUS_CXX_HAS_PROP_CONST
    "${PROJECT_BINARY_DIR}/temp"
    "${PROJECT_SOURCE_DIR}/cmake-tests/propagate-const.cpp"
    CMAKE_FLAGS -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_STANDARD_REQUIRED=ON
)

# Check for compiler flags that we want
set( UNUSED_RESULT 0 )
check_cxx_compiler_flag( "-Wunused-result" UNUSED_RESULT )
if( ${UNUSED_RESULT} )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-result" )
endif( ${UNUSED_RESULT} )

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
    dbus-cxx/messageiterator.cpp
    dbus-cxx/methodbase.cpp
    dbus-cxx/methodproxybase.cpp
    dbus-cxx/object.cpp
    dbus-cxx/objectproxy.cpp
    dbus-cxx/path.cpp
    dbus-cxx/pendingcall.cpp
    dbus-cxx/returnmessage.cpp
    dbus-cxx/signalbase.cpp
    dbus-cxx/signalmessage.cpp
    dbus-cxx/signalproxy.cpp
    dbus-cxx/signature.cpp
    dbus-cxx/signatureiterator.cpp
    dbus-cxx/standalonedispatcher.cpp
    dbus-cxx/utility.cpp
    dbus-cxx/types.cpp
    dbus-cxx/variant.cpp
    dbus-cxx/marshaling.cpp
    dbus-cxx/demarshaling.cpp
    dbus-cxx/simpletransport.cpp
    dbus-cxx/sendmsgtransport.cpp
    dbus-cxx/transport.cpp
    dbus-cxx/threaddispatcher.cpp
    dbus-cxx/sasl.cpp
    dbus-cxx/validator.cpp
    dbus-cxx/daemon-proxy/DBusDaemonProxy.cpp
    dbus-cxx/variantappenditerator.cpp
    dbus-cxx/variantiterator.cpp
    dbus-cxx/property.cpp
    dbus-cxx/propertyproxy.cpp
    dbus-cxx/matchrule.cpp
    dbus-cxx/standard-interfaces/peerinterfaceproxy.cpp
    dbus-cxx/standard-interfaces/introspectableinterfaceproxy.cpp
    dbus-cxx/standard-interfaces/propertiesinterfaceproxy.cpp
)

# headers that need to go in the include/dbus-cxx directory
set( DBUS_CXX_HEADERS
    dbus-cxx.h
    dbus-cxx/callmessage.h
    dbus-cxx/dispatcher.h
    dbus-cxx/enums.h
    dbus-cxx/error.h
    dbus-cxx/errormessage.h
    dbus-cxx/filedescriptor.h
    dbus-cxx/headerlog.h
    dbus-cxx/messageappenditerator.h
    dbus-cxx/message.h
    dbus-cxx/messageiterator.h
    dbus-cxx/methodbase.h
    dbus-cxx/path.h
    dbus-cxx/pendingcall.h
    dbus-cxx/returnmessage.h
    dbus-cxx/signalbase.h
    dbus-cxx/signalmessage.h
    dbus-cxx/signalproxy.h
    dbus-cxx/signature.h
    dbus-cxx/signatureiterator.h
    dbus-cxx/simplelogger_defs.h
    dbus-cxx/simplelogger.h
    dbus-cxx/types.h
    dbus-cxx/utility.h
    dbus-cxx/demangle.h
    dbus-cxx/signal.h
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
    dbus-cxx/standalonedispatcher.h
    dbus-cxx/marshaling.h
    dbus-cxx/demarshaling.h
    dbus-cxx/sasl.h
    dbus-cxx/dbus-error.h
    dbus-cxx/threaddispatcher.h
    dbus-cxx/validator.h
    dbus-cxx/variantappenditerator.h
    dbus-cxx/variantiterator.h
    dbus-cxx/property.h
    dbus-cxx/propertyproxy.h
    dbus-cxx/matchrule.h
    dbus-cxx/standard-interfaces/peerinterfaceproxy.h
    dbus-cxx/standard-interfaces/introspectableinterfaceproxy.h
    dbus-cxx/standard-interfaces/propertiesinterfaceproxy.h
    dbus-cxx/daemon-proxy/DBusDaemonProxy.h
    dbus-cxx/multiplereturn.h
)

set( DBUS_CXX_INCLUDE_DIRECTORIES 
    ${PROJECT_SOURCE_DIR} 
    ${PROJECT_BINARY_DIR} )
include_directories( ${DBUS_CXX_INCLUDE_DIRECTORIES} 
    ${dbus_INCLUDE_DIRS} 
    ${sigc_INCLUDE_DIRS} )


#
# Dependencies
#
set( THREADS_PREFER_PTHREAD_FLAG ON )
find_package( Threads REQUIRED )
set( DBUS_CXX_LIBRARIES ${sigc_LIBRARIES} Threads::Threads )

find_library( LIBRT rt )
if( LIBRT )
	list( APPEND DBUS_CXX_LIBRARIES ${LIBRT} )
endif( LIBRT )


#
# Target for the library
#
link_directories( ${sigc_LIBRARY_DIRS} )
add_library( dbus-cxx SHARED ${DBUS_CXX_SOURCES} ${DBUS_CXX_HEADERS} )
set_target_properties( dbus-cxx PROPERTIES VERSION 2.0.0 SOVERSION 2 )
target_link_libraries( dbus-cxx ${DBUS_CXX_LIBRARIES} )
target_include_directories( dbus-cxx INTERFACE
    $<INSTALL_INTERFACE:include/dbus-cxx-${DBUS_CXX_INCLUDE_VERSION}>
)

if( WIN32 )
    if( CMAKE_SYSTEM_VERSION )
        set( ver ${CMAKE_SYSTEM_VERSION} )
        string( REGEX MATCH "^([0-9]+).([0-9])" ver ${ver} )
        string( REGEX MATCH "^([0-9]+)" verMajor ${ver} )
        # Check for Windows 10, b/c we'll need to convert to hex 'A'.
        if( "${verMajor}" MATCHES "10" )
            set( verMajor "A" )
            string( REGEX REPLACE "^([0-9]+)" ${verMajor} ver ${ver} )
        endif()
        # Remove all remaining '.' characters.
        string( REPLACE "." "" ver ${ver} )
        # Prepend each digit with a zero.
        string( REGEX REPLACE "([0-9A-Z])" "0\\1" ver ${ver} )
        set( WIN32_VER "0x${ver}" )
    endif( CMAKE_SYSTEM_VERSION )
    add_definitions( -D_WIN32_WINNT=${WIN32_VER} )
    set( CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON )

    if( MSVC AND (MSVC_VERSION GREATER_EQUAL 1914) )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus" )
    endif( MSVC AND (MSVC_VERSION GREATER_EQUAL 1914) )

    add_subdirectory( compat )
endif( WIN32 )

# Dbus-cxx requires at least C++17 due to libsigc++-3.0
set_property( TARGET dbus-cxx PROPERTY CXX_STANDARD 17 )

#
# If GLIB is used, add in the appropriate code
#
if( ENABLE_GLIB_SUPPORT )
    add_subdirectory( dbus-cxx-glib )
endif( ENABLE_GLIB_SUPPORT )

#
# If Qt should be supported, add in the appropriate code
#
if( ENABLE_QT_SUPPORT )
    add_subdirectory( dbus-cxx-qt )
endif( ENABLE_QT_SUPPORT )

#
# If libuv should be supported, add in the appropriate code
#
if( ENABLE_UV_SUPPORT )
    add_subdirectory( dbus-cxx-uv )
endif( ENABLE_UV_SUPPORT )

#
# Library install information
#
install( TARGETS dbus-cxx
    COMPONENT dbus-cxx
    EXPORT dbus-cxxTargets
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" )
foreach(FILE ${DBUS_CXX_HEADERS} )
    get_filename_component(DIR ${FILE} DIRECTORY)
    install( FILES ${FILE} DESTINATION include/dbus-cxx-${DBUS_CXX_INCLUDE_VERSION}/${DIR} )
endforeach()
install( FILES
    ${PROJECT_BINARY_DIR}/dbus-cxx/dbus-cxx-config.h
    DESTINATION include/dbus-cxx-${DBUS_CXX_INCLUDE_VERSION}/dbus-cxx )

#
# pkg-config script creation and install
#
SET(PKG_CONFIG_LIBDIR
    "\${prefix}/lib"
)
SET(PKG_CONFIG_INCLUDEDIR
    "\${prefix}/include/dbus-cxx-${DBUS_CXX_INCLUDE_VERSION}"
)
SET(PKG_CONFIG_LIBS
    "-L\${libdir} -ldbus-cxx"
)
SET(PKG_CONFIG_CFLAGS
    "-I\${includedir}"
)
SET(PKG_CONFIG_REQUIRES
    "sigc++-3.0"
)

CONFIGURE_FILE(
    "${PROJECT_SOURCE_DIR}/dbus-cxx-2.0.pc.cmake"
    "${PROJECT_BINARY_DIR}/dbus-cxx-2.0.pc"
)
INSTALL( FILES "${PROJECT_BINARY_DIR}/dbus-cxx-2.0.pc"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

#
# Support for find_package
#
install(EXPORT dbus-cxxTargets
  FILE        dbus-cxxConfig.cmake
  NAMESPACE     dbus-cxx::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dbus-cxx
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/dbus-cxxConfigVersion.cmake"
  VERSION       ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)
install(FILES   "${CMAKE_CURRENT_BINARY_DIR}/dbus-cxxConfigVersion.cmake"
  DESTINATION   ${CMAKE_INSTALL_LIBDIR}/cmake/dbus-cxx
)

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
    file( COPY ${PROJECT_SOURCE_DIR}/doc/
          DESTINATION ${PROJECT_BINARY_DIR}/doc/ )
    configure_file( ${PROJECT_SOURCE_DIR}/doc/Doxyfile
                    ${PROJECT_BINARY_DIR}/doc @ONLY )

    add_custom_target( doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
        DEPENDS dbus-cxx
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/doc
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
    add_custom_target( tar_site ALL
        COMMAND tar -czf dbus-cxx-website-${dbus-cxx_VERSION}.tar.gz -C doc/reference/html .
        DEPENDS doc_doxygen
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
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
set(CPACK_SOURCE_PACKAGE_FILE_NAME "dbus-cxx-${dbus-cxx_VERSION}")
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
message(STATUS "  Build website ................... : ${BUILD_SITE}")
message(STATUS "  Enable code coverage report ..... : ${ENABLE_CODE_COVERAGE_REPORT}")
if( CMAKE_BUILD_TYPE MATCHES Debug )
message(STATUS "  libasan enabled ................. : ${ENABLE_ASAN}")
endif()
message(STATUS "  propagate_const ................. : ${DBUS_CXX_HAS_PROP_CONST}")
if( BUILD_TESTING )
message(STATUS "  Extended robustness tests ....... : ${ENABLE_ROBUSTNESS_TESTS}")
endif( BUILD_TESTING )

message(STATUS "Library Support:" )
message(STATUS "  Qt .............................. : ${ENABLE_QT_SUPPORT}")
message(STATUS "  GLib ............................ : ${ENABLE_GLIB_SUPPORT}")
message(STATUS "  libuv ........................... : ${ENABLE_UV_SUPPORT}")

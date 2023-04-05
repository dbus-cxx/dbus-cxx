# 
# This file is part of libcppgenerate
#
# Copyright 2018 Robert Middleton robert.middleton@rm5248.com
#
cmake_minimum_required( VERSION 3.1 )

project( libcppgenerate VERSION 0.2)

# We require at least C++11
set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

#
# Library ABI version
# Semantic versioning: https://semver.org/
#
set( CPPGENERATE_MAJOR 0 )
set( CPPGENERATE_MINOR 0 )
set( CPPGENERATE_MICRO 2 )
set( CPPGENERATE_VERSION ${CPPGENERATE_MAJOR}.${CPPGENERATE_MINOR}.${CPPGENERATE_MICRO} )

#
# All options that we have to compile
#
option( ENABLE_TESTS "Enable the unit tests" OFF )
option( ENABLE_EXAMPLES "Enable the examples" OFF )

#
# Sources
#
set( CPPGENERATE_SOURCES 
  cppgenerate/class.cpp
  cppgenerate/method.cpp
  cppgenerate/variable.cpp
  cppgenerate/membervariable.cpp
  cppgenerate/enum.cpp
  cppgenerate/codeblock.cpp
  cppgenerate/cppgenerateutils.cpp
  cppgenerate/printer.cpp
  cppgenerate/argument.cpp
  cppgenerate/constructor.cpp
)
set( CPPGENERATE_HEADERS
  cppgenerate/class.h
  cppgenerate/method.h
  cppgenerate/variable.h
  cppgenerate/membervariable.h
  cppgenerate/enum.h
  cppgenerate/codeblock.h
  cppgenerate/cppgenerateutils.h
  cppgenerate/printer.h
  cppgenerate/argument.h
  cppgenerate/constructor.h
  cppgenerate/accessmodifier.h
)

#
# Library information
#
add_library( objlib OBJECT ${CPPGENERATE_SOURCES} ${CPPGENERATE_HEADERS} )
set_property( TARGET objlib PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library( cppgenerate SHARED $<TARGET_OBJECTS:objlib>)
set_target_properties( cppgenerate PROPERTIES VERSION ${CPPGENERATE_VERSION} SOVERSION ${CPPGENERATE_MAJOR} )
target_include_directories( cppgenerate INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )

add_library( cppgenerate_static STATIC $<TARGET_OBJECTS:objlib>)
set_target_properties( cppgenerate_static PROPERTIES OUTPUT_NAME cppgenerate )
target_include_directories( cppgenerate_static INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )

#
# Install information
#
set( install_targets ${install_targets} cppgenerate cppgenerate_static )
IF( UNIX )
    include( GNUInstallDirs )
    install( TARGETS ${install_targets}
             RUNTIME DESTINATION bin
             LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
             ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" )
    install( FILES ${CPPGENERATE_HEADERS} DESTINATION include/cppgenerate )
ENDIF( UNIX )
IF( WIN32 )
    install( TARGETS ${install_targets}
             RUNTIME DESTINATION bin
             LIBRARY DESTINATION lib
             ARCHIVE DESTINATION lib )
    install( FILES ${CPPGENERATE_HEADERS} DESTINATION include/cppgenerate )
ENDIF( WIN32 )

IF( UNIX )
    #
    # pkg-config information
    #
    SET(PKG_CONFIG_LIBDIR
        "\${prefix}/lib"
    )
    SET(PKG_CONFIG_INCLUDEDIR
        "\${prefix}/include"
    )
    SET(PKG_CONFIG_LIBS
        "-L\${libdir} -lcppgenerate"
    )
    SET(PKG_CONFIG_CFLAGS
        "-I\${includedir}"
    )
    SET(PKG_CONFIG_STATIC_LIBS
        "-L\${libdir} -l:libcppgenerate.a"
    )

    CONFIGURE_FILE(
        "${CMAKE_CURRENT_SOURCE_DIR}/cppgenerate.pc.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/cppgenerate.pc"
    )
    CONFIGURE_FILE(
        "${CMAKE_CURRENT_SOURCE_DIR}/cppgenerate-static.pc.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/cppgenerate-static.pc"
    )
    INSTALL( FILES 
             "${CMAKE_CURRENT_BINARY_DIR}/cppgenerate.pc"
             "${CMAKE_CURRENT_BINARY_DIR}/cppgenerate-static.pc"
             DESTINATION lib/pkgconfig)
ENDIF( UNIX )

#
# Enable all warnings
#
if(MSVC)
  # Force to always compile with W4
  if(CMAKE_C_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")
  else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-long-long -pedantic")
endif()

#
# platform-specific defines
#
IF( WIN32 )
	ADD_DEFINITIONS( "-DCPPGENERATE_LIB" )
	target_compile_definitions( cppgenerate PUBLIC CPPGENERATE_STATIC )
ENDIF( WIN32 )

#
# Enable tests, if specified
#
if( ENABLE_TESTS )
    enable_testing()
    add_subdirectory( tests )
endif( ENABLE_TESTS )

#
# Enable examples, if specified
#
if( ENABLE_EXAMPLES )
    add_subdirectory( examples )
endif( ENABLE_EXAMPLES )

message( "CMake command ${CMAKE_COMMAND}" )
message( "CMake generator ${CMAKE_GENERATOR}" )

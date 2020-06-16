pkg_check_modules( dbus dbus-1 )

set( EXAMPLES_INCLUDES ${sigc_INCLUDE_DIRS} )
set( EXAMPLES_LINK dbus-cxx ${sigc_LDFLAGS} -lrt )
set( CMAKE_CXX_STANDARD 17 )

link_directories( ${CMAKE_BINARY_DIR} )

include_directories( ${CMAKE_SOURCE_DIR}/dbus-cxx
    ${CMAKE_BINARY_DIR}/dbus-cxx 
    ${dbus_INCLUDE_DIRS} 
    ${sigc_INCLUDE_DIRS} )

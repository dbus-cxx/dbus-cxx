set( EXAMPLES_INCLUDES ${dbus_INCLUDE_DIRS} ${sigc_INCLUDE_DIRS} )
set( EXAMPLES_LINK dbus-cxx ${dbus_LDFLAGS} ${sigc_LDFLAGS} -lrt )
set( CMAKE_CXX_STANDARD 11 )

link_directories( ${CMAKE_BINARY_DIR} )

include_directories( ${CMAKE_SOURCE_DIR}/dbus-cxx
    ${CMAKE_BINARY_DIR}/dbus-cxx 
    ${dbus_INCLUDE_DIRS} 
    ${sigc_INCLUDE_DIRS} )

pkg_check_modules( cppgenerate REQUIRED libcppgenerate )
pkg_check_modules( popt REQUIRED popt )
pkg_check_modules( expat REQUIRED expat )

set( TOOLS_INCLUDES ${dbus_INCLUDE_DIRS} ${sigc_INCLUDE_DIRS} )
set( TOOLS_LINK dbus-cxx ${dbus_LDFLAGS} ${sigc_LDFLAGS} ${popt_LDFLAGS} ${expat_LDFLAGS} -lrt )


link_directories( ${CMAKE_BINARY_DIR} )

include_directories( ${CMAKE_SOURCE_DIR}/dbus-cxx
    ${CMAKE_BINARY_DIR}/dbus-cxx 
    ${dbus_INCLUDE_DIRS} 
    ${sigc_INCLUDE_DIRS} )

add_executable( dbus-cxx-xml2cpp 
    generate_adapter.cpp
    generate_proxy.cpp
    parsed_object.cpp
    xml2cpp.cpp
    xml_parser.cpp
    arg.cpp
    method.cpp
    signal.cpp  )
target_link_libraries( dbus-cxx-xml2cpp ${TOOLS_LINK} )
install( TARGETS dbus-cxx-xml2cpp DESTINATION bin )

add_executable( dbus-cxx-introspect
    introspect.cpp )
target_link_libraries( dbus-cxx-introspect ${TOOLS_LINK} )
install( TARGETS dbus-cxx-introspect DESTINATION bin )

# Check to see if we want to use the bundled libcppgenerate or not
if( TOOLS_BUNDLED_CPPGENERATE )
    add_subdirectory( libcppgenerate )
else()
    pkg_check_modules( cppgenerate REQUIRED cppgenerate-static )
endif( TOOLS_BUNDLED_CPPGENERATE )

find_package(popt REQUIRED)

find_package(EXPAT REQUIRED)

add_executable( dbus-cxx-xml2cpp 
    generate_adapter.cpp
    generate_proxy.cpp
    parsed_object.cpp
    xml2cpp.cpp
    xml_parser.cpp
    arg.cpp
    method.cpp
    signal.cpp 
    code-generator.cpp 
)
target_link_libraries( dbus-cxx-xml2cpp dbus-cxx cppgenerate_static popt EXPAT::EXPAT rt )
install( TARGETS dbus-cxx-xml2cpp DESTINATION bin )

install( TARGETS dbus-cxx-xml2cpp
    COMPONENT dbus-cxx
    EXPORT dbus-cxxTargets
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" )

add_executable( dbus-cxx-introspect
    introspect.cpp )
target_link_libraries( dbus-cxx-introspect dbus-cxx popt rt )

install( TARGETS dbus-cxx-introspect
    COMPONENT dbus-cxx
    EXPORT dbus-cxxTargets
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" )

# Check to see if we want to use the bundled libcppgenerate or not
if( TOOLS_BUNDLED_CPPGENERATE )
    add_subdirectory(libcppgenerate EXCLUDE_FROM_ALL)
else()
    pkg_check_modules( cppgenerate REQUIRED IMPORTED_TARGET cppgenerate-static )
    add_library(cppgenerate_static ALIAS PkgConfig::cppgenerate)
endif( TOOLS_BUNDLED_CPPGENERATE )

pkg_check_modules( dbus REQUIRED IMPORTED_TARGET dbus-1 )
pkg_check_modules( popt REQUIRED IMPORTED_TARGET popt )

set( TOOLS_LINK dbus-cxx PkgConfig::dbus PkgConfig::sigc PkgConfig::popt PkgConfig::expat ${LIBRT} )

link_directories( ${CMAKE_BINARY_DIR} )

include_directories( ${CMAKE_SOURCE_DIR}/dbus-cxx
    ${CMAKE_BINARY_DIR}/dbus-cxx )

add_executable( dbus-cxx-xml2cpp
    xml2cpp.cpp
    code-generator.cpp
    signal-info.cpp
    method-info.cpp
    property-info.cpp
    interface-info.cpp
    node-info.cpp
)
target_link_libraries( dbus-cxx-xml2cpp ${TOOLS_LINK} cppgenerate_static )
install( TARGETS dbus-cxx-xml2cpp DESTINATION bin )
set_property( TARGET dbus-cxx-xml2cpp PROPERTY CXX_STANDARD 17 )

if( TOOLS_BUNDLED_CPPGENERATE )
    add_dependencies( dbus-cxx-xml2cpp cppgenerate )
endif( TOOLS_BUNDLED_CPPGENERATE )

add_executable( dbus-cxx-introspect
    introspect.cpp )
target_link_libraries( dbus-cxx-introspect ${TOOLS_LINK} )
install( TARGETS dbus-cxx-introspect DESTINATION bin )
set_property( TARGET dbus-cxx-introspect PROPERTY CXX_STANDARD 17 )


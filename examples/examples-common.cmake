pkg_check_modules( dbus IMPORTED_TARGET dbus-1 )

set( EXAMPLES_LINK dbus-cxx PkgConfig::sigc ${LIBRT} PkgConfig::dbus )
set( CMAKE_CXX_STANDARD 17 )

link_directories( ${CMAKE_BINARY_DIR} )

include_directories( ${CMAKE_SOURCE_DIR}/dbus-cxx
    ${CMAKE_BINARY_DIR}/dbus-cxx )

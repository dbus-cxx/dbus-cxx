# - Try to find libsigc++ include dirs and libraries
#
# Usage of this module as follows:
#
#     find_package(SIGCXX)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
#  SIGCXX_ROOT_DIR             Set this variable to the root installation of
#                              libsigc++ if the module has problems finding the
#                              proper installation path.
#
# Variables defined by this module:
#
#  SIGCXX_FOUND                System has libsigc++, include and library dirs found
#  SIGCXX_INCLUDE_DIR          The libsigc++ include directories
#  SIGCXX_CONFIG_DIR           The libsigc++ config directory
#  SIGCXX_LIBRARY              The libsigc++ library


find_path(SIGCXX_ROOT_DIR
    NAMES include/sigc++-2.0/sigc++/sigc++.h
          Include/sigc++-2.0/sigc++/sigc++.h
)

find_path(SIGCXX_CONFIG_DIR
    NAMES sigc++config.h
    HINTS ${SIGCXX_ROOT_DIR}/include/sigc++-2.0
    /usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}/sigc++-2.0/include
)

find_path(SIGCXX_INCLUDE_DIR
    NAMES sigc++/sigc++.h
    HINTS ${SIGCXX_ROOT_DIR}/include/sigc++-2.0
)

find_library(SIGCXX_LIBRARY
    NAMES sigc-2.0
    HINTS ${SIGCXX_ROOT_DIR}/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SIGCXX DEFAULT_MSG
    SIGCXX_INCLUDE_DIR
    SIGCXX_CONFIG_DIR
    SIGCXX_LIBRARY
)


mark_as_advanced(
    SIGCXX_ROOT_DIR
    SIGCXX_INCLUDE_DIR
    SIGCXX_CONFIG_DIR
    SIGCXX_LIBRARY
)

message(STATUS "SIGCXX_INCLUDE_DIR ${SIGCXX_INCLUDE_DIR}")
message(STATUS "SIGCXX_LIBRARY ${SIGCXX_LIBRARY}")

add_library(sigc++ SHARED IMPORTED)
add_library(sigc++::sigc++ ALIAS sigc++)
set_property(TARGET sigc++ PROPERTY
             IMPORTED_LOCATION "${SIGCXX_LIBRARY}")
target_include_directories(sigc++ INTERFACE ${SIGCXX_INCLUDE_DIR} ${SIGCXX_CONFIG_DIR})

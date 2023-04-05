# - Try to find the popt options processing library
# The module will set the following variables
#
#  POPT_FOUND - System has popt
#  POPT_INCLUDE_DIR - The popt include directory
#  POPT_LIBRARY - The libraries needed to use popt

# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls

find_package(PkgConfig QUIET)
if (PKG_CONFIG_FOUND)
  pkg_search_module( PC_DBUS QUIET dbus-1>=1.3 )
endif()


# Find the include directories
find_path(DBUS_INCLUDE_DIR
  NAMES dbus/dbus.h
  HINTS
    ${PC_DBUS_INCLUDEDIR}
    ${PC_DBUS_INCLUDE_DIRS}
  DOC "Path containing the dbus.h include file"
)


find_library(DBUS_LIBRARY
    NAMES dbus-1
    HINTS ${PC_DBUS_LIBDIR}
          ${PC_DBUS_LIBRARY_DIRS}
    DOC "dbus library path"
)

get_filename_component(_DBUS_LIBRARY_DIR ${DBUS_LIBRARY} PATH)
find_path(DBUS_ARCH_INCLUDE_DIR
    NAMES dbus/dbus-arch-deps.h
    HINTS ${PC_DBUS_INCLUDEDIR}
          ${PC_DBUS_INCLUDE_DIRS}
          ${_DBUS_LIBRARY_DIR}
          ${DBUS_INCLUDE_DIR}
    PATH_SUFFIXES include
)

set(DBUS_INCLUDE_DIRS ${DBUS_INCLUDE_DIR} ${DBUS_ARCH_INCLUDE_DIR})

message(STATUS "DBUS_ARCH_INCLUDE_DIR ${DBUS_ARCH_INCLUDE_DIR}")
message(STATUS "DBUS_INCLUDE_DIRS ${DBUS_INCLUDE_DIRS}")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(DBus
  REQUIRED_VARS DBUS_INCLUDE_DIRS DBUS_INCLUDE_DIR DBUS_LIBRARY
  VERSION_VAR PC_POPT_VERSION)

mark_as_advanced(DBUS_INCLUDE_DIR DBUS_LIBRARY)

if(DBUS_FOUND AND NOT TARGET dbus-1)
  add_library(dbus-1 UNKNOWN IMPORTED)
  set_target_properties(dbus-1 PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
    IMPORTED_LOCATION "${DBUS_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${DBUS_INCLUDE_DIRS}"
  )
endif()

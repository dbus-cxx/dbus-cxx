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

#
# libFuzzer based fuzz tests.  These are not run as part of the unit tests;
# see README.md in this directory for how to build and run them.
#

function( dbus_cxx_add_fuzzer NAME )
    add_executable( ${NAME} ${NAME}.cpp )
    target_link_libraries( ${NAME} dbus-cxx )
    target_link_options( ${NAME} PRIVATE -fsanitize=fuzzer )
    set_property( TARGET ${NAME} PROPERTY CXX_STANDARD 17 )
endfunction()

dbus_cxx_add_fuzzer( fuzz-message-create_from_data )
dbus_cxx_add_fuzzer( fuzz-message-body )
dbus_cxx_add_fuzzer( fuzz-signature )

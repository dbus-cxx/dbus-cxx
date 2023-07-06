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

set( dbus-cxx-compat-sources
        compat/fcntl.c
        compat/getdtablesize.c
        compat/pipe2.c
        compat/poll.c
        compat/posix_win.c
)

target_sources(dbus-cxx PRIVATE ${dbus-cxx-compat-sources})
target_include_directories (dbus-cxx PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

cmake_policy(SET CMP0079 NEW)  # allow linking libraries in dbus-cxx from this subdirectory
target_link_libraries( dbus-cxx "Ws2_32.lib" )

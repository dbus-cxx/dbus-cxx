#############################################################################
##   Copyright (C) 2007,2008,2009 by Rick L. Vinyard, Jr.                  ##
##   rvinyard@cs.nmsu.edu                                                  ##
##                                                                         ##
##   This file is part of the dbus-cxx library.                            ##
##                                                                         ##
##   The dbus-cxx library is free software; you can redistribute it and/or ##
##   modify it under the terms of the GNU General Public License           ##
##   version 3 as published by the Free Software Foundation.               ##
##                                                                         ##
##   The dbus-cxx library is distributed in the hope that it will be       ##
##   useful, but WITHOUT ANY WARRANTY; without even the implied warranty   ##
##   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   ##
##   General Public License for more details.                              ##
##                                                                         ##
##   You should have received a copy of the GNU General Public License     ##
##   along with this software. If not see <http://www.gnu.org/licenses/>.  ##
#############################################################################

INCLUDES = -I$(top_srcdir)/. $(PROJECT_CFLAGS)
METASOURCES = AUTO

if DBUS_CXX_TOOLS

bin_PROGRAMS = dbus-cxx-xml2cpp dbus-cxx-introspect


dbus_cxx_xml2cpp_SOURCES = generate_adapter.cpp generate_proxy.cpp \
	parsed_object.cpp xml2cpp.cpp xml_parser.cpp arg.cpp \
	method.cpp signal.cpp
dbus_cxx_xml2cpp_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la \
	$(PROJECT_LIBS)

dbus_cxx_introspect_SOURCES = introspect.cpp
dbus_cxx_introspect_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la \
	$(PROJECT_LIBS)

# Rule to generate hal_manager.h
hal_manager.h: hal_manager_introspection.xml dbus-cxx-xml2cpp
	        ./dbus-cxx-xml2cpp --file hal_manager_introspection.xml > $@


NOINST_HEADER = generate_adapter.h generate_proxy.h parsed_object.h xml_parser.h
EXTRA_DIST = generate_adapter.h generate_proxy.h parsed_object.h xml_parser.h


endif

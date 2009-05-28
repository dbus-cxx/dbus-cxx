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

noinst_PROGRAMS = calculator-client calculator-server calculator-watcher

BUILT_SOURCES = calculator_proxy.h calculator_adapter.h
CLEANFILES = calculator_proxy.h calculator_adapter.h

# Rule to generate proxy and adapter
calculator_proxy.h: calculator.xml $(top_builddir)/tools/dbus-cxx-xml2cpp
	        $(top_builddir)/tools/dbus-cxx-xml2cpp --xml calculator.xml --proxy -f
calculator_adapter.h: calculator.xml $(top_builddir)/tools/dbus-cxx-xml2cpp
	        $(top_builddir)/tools/dbus-cxx-xml2cpp --xml calculator.xml --adapter -f

gen: calculator_proxy.h calculator_adapter.h

calculator_client_SOURCES = calculator_proxy.h calculator_client.cpp
calculator_client_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la \
	$(PROJECT_LIBS)

calculator_server_SOURCES = calculator_adapter.h calculator_server.cpp calculator.cpp
calculator_server_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la \
	$(PROJECT_LIBS)

calculator_watcher_SOURCES = calculator_proxy.h calculator_watcher.cpp
calculator_watcher_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la \
	$(PROJECT_LIBS)

noinst_HEADERS = calculator.h client_popt.h

noinst_DATA = calculator.xml

endif

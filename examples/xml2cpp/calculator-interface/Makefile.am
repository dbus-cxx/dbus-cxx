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

PROXIES = calculator_proxy.h
ADAPTERS = calculator_adapter.h

BUILT_SOURCES = $(PROXIES) $(ADAPTERS)
CLEANFILES =  $(PROXIES) $(ADAPTERS)

gen:  $(PROXIES) $(ADAPTERS)

%_adapter.h : %.xml $(top_builddir)/tools/dbus-cxx-xml2cpp
	$(top_builddir)/tools/dbus-cxx-xml2cpp --xml $(top_srcdir)/examples/xml2cpp/calculator-interface/$*.xml --adapter -f

%_proxy.h : %.xml $(top_builddir)/tools/dbus-cxx-xml2cpp
	$(top_builddir)/tools/dbus-cxx-xml2cpp --xml $(top_srcdir)/examples/xml2cpp/calculator-interface/$*.xml --proxy -f

calculator_client_SOURCES = $(PROXIES) calculator_client.cpp
calculator_client_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la \
	$(PROJECT_LIBS)

calculator_server_SOURCES = $(ADAPTERS) calculator_server.cpp calculator.cpp
calculator_server_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la \
	$(PROJECT_LIBS)

calculator_watcher_SOURCES = $(PROXIES) calculator.cpp calculator_watcher.cpp
calculator_watcher_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la \
	$(PROJECT_LIBS)

noinst_HEADERS = calculator.h calculator_interface.h client_popt.h

noinst_DATA = calculator.xml

EXTRA_DIST = $(noinst_HEADERS) $(noinst_DATA) $(PROXIES) $(ADAPTERS)

endif

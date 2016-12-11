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

noinst_PROGRAMS = computer-client computer-server computer-watcher

PROXIES = calculator_proxy.h computer_proxy.h
ADAPTERS = calculator_adapter.h computer_adapter.h

BUILT_SOURCES = $(PROXIES) $(ADAPTERS)
CLEANFILES =  $(PROXIES) $(ADAPTERS)

gen:  $(PROXIES) $(ADAPTERS)

%_adapter.h : %.xml $(top_builddir)/tools/dbus-cxx-xml2cpp
	$(top_builddir)/tools/dbus-cxx-xml2cpp --xml $(top_srcdir)/examples/xml2cpp/computer-is-a/$*.xml --adapter -f

%_proxy.h : %.xml $(top_builddir)/tools/dbus-cxx-xml2cpp
	$(top_builddir)/tools/dbus-cxx-xml2cpp --xml $(top_srcdir)/examples/xml2cpp/computer-is-a/$*.xml --proxy -f

computer_client_SOURCES = $(PROXIES) computer_client.cpp
computer_client_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la \
	$(PROJECT_LIBS)

computer_server_SOURCES = $(ADAPTERS) computer_server.cpp computer.cpp calculator.cpp
computer_server_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la \
	$(PROJECT_LIBS)

computer_watcher_SOURCES = $(PROXIES) computer_watcher.cpp
computer_watcher_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la \
	$(PROJECT_LIBS)

noinst_HEADERS = computer.h calculator.h client_popt.h

noinst_DATA = computer.xml calculator.xml

EXTRA_DIST = $(noinst_HEADERS) $(noinst_DATA) $(PROXIES) $(ADAPTERS)

endif

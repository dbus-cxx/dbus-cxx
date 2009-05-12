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

noinst_PROGRAMS = hal-proxy

BUILT_SOURCES = hal_manager_proxy.h

CLEANFILES = hal_manager_proxy.h

# Rule to generate hal_manager_proxy.h
hal_manager_proxy.h: hal_manager_introspection.xml $(top_builddir)/tools/dbus-cxx-xml2cpp
	        $(top_builddir)/tools/dbus-cxx-xml2cpp --xml hal_manager_introspection.xml --proxy -f --prefix="hal_"

gen: hal_manager_proxy.h

hal_proxy_SOURCES = hal_manager_proxy.h hal_proxy.cpp
hal_proxy_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la \
	$(PROJECT_LIBS)

EXTRA_DIST = hal_manager_introspection.xml

endif

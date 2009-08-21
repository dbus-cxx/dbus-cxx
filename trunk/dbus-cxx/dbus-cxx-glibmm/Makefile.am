#############################################################################
##   Copyright (C) 2009 by Rick L. Vinyard, Jr.                            ##
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

INCLUDES = -I$(top_srcdir)/. $(PROJECT_CFLAGS) $(DBUS_CXX_GLIBMM_CFLAGS)
METASOURCES = AUTO

if DBUS_CXX_GLIBMM

lib_LTLIBRARIES = libdbus-cxx-glibmm.la
library_includedir = $(includedir)/dbus-cxx-@PACKAGE_RELEASE@/dbus-cxx-glibmm

libdbus_cxx_glibmm_la_LIBADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la \
	$(DBUS_CXX_GLIBMM_LIBS) $(PROJECT_LIBS)

libdbus_cxx_glibmm_la_LDFLAGS = -version-info @LIBTOOL_SO_VERSION@
library_include_HEADERS = dispatcher.h
libdbus_cxx_glibmm_la_SOURCES = dispatcher.cpp

endif

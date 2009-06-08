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

INCLUDES = $(PROJECT_CFLAGS) -I$(top_srcdir)/.
METASOURCES = AUTO
noinst_PROGRAMS = time-client time-server
time_client_SOURCES = time_client.cpp timeval_dbus.cpp
time_server_SOURCES = time_server.cpp timeval_dbus.cpp
time_client_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la $(PROJECT_LIBS)
time_server_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la $(PROJECT_LIBS)
noinst_HEADERS = timeval_dbus.h
EXTRA_DIST = $(noinst_HEADERS)

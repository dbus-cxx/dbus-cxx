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
noinst_PROGRAMS = callee callee-2 callee-c callee-dispatched \
	callee-object caller-object callee-object-inherited callee-raw caller caller-c caller-dispatched \
	caller-raw

caller_c_SOURCES = caller_c.cpp
caller_c_LDADD = $(PROJECT_LIBS)
callee_c_SOURCES = callee_c.cpp
callee_c_LDADD = $(PROJECT_LIBS)


caller_raw_SOURCES = caller_raw.cpp
caller_raw_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la $(PROJECT_LIBS)
callee_raw_SOURCES = callee_raw.cpp
callee_raw_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la $(PROJECT_LIBS)

caller_SOURCES = caller.cpp
caller_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la $(PROJECT_LIBS)
callee_SOURCES = callee.cpp
callee_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la $(PROJECT_LIBS)
callee_2_SOURCES = callee_2.cpp
callee_2_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la $(PROJECT_LIBS)
callee_object_SOURCES = callee_object.cpp
callee_object_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la $(PROJECT_LIBS)
caller_object_SOURCES = caller_object.cpp
caller_object_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la $(PROJECT_LIBS)
callee_object_inherited_SOURCES = callee_object_inherited.cpp
callee_object_inherited_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la $(PROJECT_LIBS)

callee_dispatched_SOURCES = callee_dispatched.cpp
callee_dispatched_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la
caller_dispatched_SOURCES = caller_dispatched.cpp
caller_dispatched_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la
noinst_HEADERS = caller_popt.h

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

INCLUDES = $(PROJECT_CFLAGS) $(DBUS_CXX_GLIBMM_CFLAGS) -I$(top_srcdir)/.
METASOURCES = AUTO

if DBUS_CXX_GLIBMM

noinst_PROGRAMS = glibmm-callee-dispatched glibmm-caller-dispatched
glibmm_callee_dispatched_SOURCES = glibmm_callee_dispatched.cpp
glibmm_callee_dispatched_LDADD = \
	$(top_builddir)/dbus-cxx-glibmm/libdbus-cxx-glibmm.la $(top_builddir)/dbus-cxx/libdbus-cxx.la $(DBUS_CXX_GLIBMM_LIBS) \
	$(PROJECT_LIBS)
glibmm_caller_dispatched_SOURCES = glibmm_caller_dispatched.cpp
glibmm_caller_dispatched_LDADD = \
	$(top_builddir)/dbus-cxx-glibmm/libdbus-cxx-glibmm.la $(top_builddir)/dbus-cxx/libdbus-cxx.la $(DBUS_CXX_GLIBMM_LIBS) \
	$(PROJECT_LIBS)

noinst_HEADERS = caller_popt.h
EXTRA_DIST = caller_popt.h
endif

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

INCLUDES = -I$(top_srcdir)/. $(PROJECT_CFLAGS) $(UNIT_TEST_CFLAGS)
METASOURCES = AUTO

if DBUS_CXX_UNIT_TESTS

noinst_PROGRAMS = dbus-cxx-unit-tests
dbus_cxx_unit_tests_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la \
	$(PROJECT_LIBS) $(UNIT_TEST_LIBS) -ldl
dbus_cxx_unit_tests_SOURCES = callmessagetests.cpp dbus_cxx_unit_tests.cpp \
	messageiteratortests.cpp pathclasstests.cpp
noinst_HEADERS = callmessagetests.h messageiteratortests.h pathclasstests.h

endif

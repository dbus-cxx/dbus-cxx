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
noinst_PROGRAMS = signal-emitter signal-emitter-2 signal-emitter-3 \
	signal-emitter-c signal-emitter-raw signal-receiver signal-receiver-2 signal-receiver-c \
	signal-receiver-raw signal-emitter-object signal-receiver-object signal-emitter-array signal-receiver-array

signal_emitter_c_SOURCES = signal_emitter_c.cpp
signal_emitter_c_LDADD = $(PROJECT_LIBS)
signal_receiver_c_SOURCES = signal_receiver_c.cpp
signal_receiver_c_LDADD = $(PROJECT_LIBS)

signal_emitter_raw_SOURCES = signal_emitter_raw.cpp
signal_emitter_raw_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la $(PROJECT_LIBS)
signal_receiver_raw_SOURCES = signal_receiver_raw.cpp
signal_receiver_raw_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la $(PROJECT_LIBS)

signal_emitter_SOURCES = signal_emitter.cpp
signal_emitter_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la $(PROJECT_LIBS)
signal_emitter_2_SOURCES = signal_emitter_2.cpp
signal_emitter_2_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la $(PROJECT_LIBS)
signal_emitter_3_SOURCES = signal_emitter_3.cpp
signal_emitter_3_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la $(PROJECT_LIBS)
signal_receiver_SOURCES = signal_receiver.cpp
signal_receiver_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la $(PROJECT_LIBS)
signal_receiver_2_SOURCES = signal_receiver_2.cpp
signal_receiver_2_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la $(PROJECT_LIBS)

signal_emitter_object_SOURCES = signal_emitter_object.cpp
signal_emitter_object_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la $(PROJECT_LIBS)
signal_receiver_object_SOURCES = signal_receiver_object.cpp
signal_receiver_object_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la $(PROJECT_LIBS)

signal_emitter_array_SOURCES = signal_emitter_array.cpp
signal_emitter_array_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la \
	$(PROJECT_LIBS)
signal_receiver_array_SOURCES = signal_receiver_array.cpp
signal_receiver_array_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la \
	$(PROJECT_LIBS)


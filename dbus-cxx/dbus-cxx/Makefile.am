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

INCLUDES = -I$(top_builddir)/. -I$(top_srcdir)/. $(PROJECT_CFLAGS)
METASOURCES = AUTO
lib_LTLIBRARIES = libdbus-cxx.la
library_includedir = $(includedir)/@PACKAGE_NAME@-@PACKAGE_RELEASE@/@PACKAGE_NAME@

dbus_cxx_m4 = dbus_signal.h.m4 connection.h.m4 interfaceproxy.h.m4 signal_proxy.h.m4 method.h.m4 method_impl.h.m4 interface.h.m4 object.h.m4 methodproxybase.h.m4 methodproxy.h.m4
EXTRA_DIST = dbus_signal.h.m4 connection.h.m4 interfaceproxy.h.m4 signal_proxy.h.m4 method.h.m4 method_impl.h.m4 interface.h.m4 object.h.m4 objectproxy.h.m4  methodproxybase.h.m4 methodproxy.h.m4 template.macros.m4

nodist_libdbus_cxx_la_SOURCES = dbus_signal.h connection.h interfaceproxy.h signal_proxy.h method.h method_impl.h interface.h object.h objectproxy.h methodproxybase.h methodproxy.h
BUILT_SOURCES = dbus_signal.h connection.h interfaceproxy.h signal_proxy.h method.h method_impl.h interface.h object.h objectproxy.h methodproxybase.h methodproxy.h
CLEANFILES = dbus_signal.h connection.h interfaceproxy.h signal_proxy.h method.h method_impl.h interface.h object.h objectproxy.h methodproxybase.h methodproxy.h

# Rules to generate .h and .cc from .h.m4 and .cpp.m4:
%.h: %.h.m4 template.macros.m4
	        $(M4) $<  > $@

%.cpp: %.cpp.m4 template.macros.m4
	        $(M4) $<  > $@

gen: dbus_signal.h connection.h signal_proxy.h method.h method_impl.h interface.h object.h objectproxy.h methodproxybase.h methodproxy.h

library_include_HEADERS = accumulators.h callmessage.h connection.h \
	dbus-cxx-config.h dbus_signal.h dispatcher.h enums.h error.h errormessage.h forward_decls.h interface.h \
	interfaceproxy.h message.h messageappenditerator.h messagefilter.h messagehandler.h \
	messageiterator.h method.h method_impl.h methodbase.h methodproxy.h methodproxybase.h \
	object.h objectpathhandler.h objectproxy.h path.h pendingcall.h pointer.h \
	returnmessage.h signal_base.h signal_proxy.h signal_proxy_base.h signalmessage.h \
	signalreceiver.h signature.h signatureiterator.h timeout.h types.h utility.h watch.h variant.h

libdbus_cxx_la_SOURCES = callmessage.cpp connection.cpp dispatcher.cpp \
	error.cpp errormessage.cpp interface.cpp interfaceproxy.cpp message.cpp \
	messageappenditerator.cpp messagefilter.cpp messagehandler.cpp messageiterator.cpp methodbase.cpp \
	methodproxybase.cpp object.cpp objectpathhandler.cpp objectproxy.cpp path.cpp \
	pendingcall.cpp returnmessage.cpp signal_base.cpp signal_proxy_base.cpp \
	signalmessage.cpp signalreceiver.cpp signature.cpp signatureiterator.cpp timeout.cpp \
	utility.cpp watch.cpp


libdbus_cxx_la_LDFLAGS = -version-info @LIBTOOL_SO_VERSION@

libdbus_cxx_la_LIBADD = $(PROJECT_LIBS)

#libdbus_c___1_la_LIBADD = $(dbus_LIBS) $(xml_LIBS)

#AM_CPPFLAGS = \
#	$(dbus_CFLAGS) \
#	$(xml_CFLAGS) \
#	-DDBUS_API_SUBJECT_TO_CHANGE

MAINTAINERCLEANFILES = \
	Makefile.in

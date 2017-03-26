INCLUDES = $(PROJECT_CFLAGS) -I$(top_srcdir)/.  $(PROJECT_CFLAGS)
METASOURCES = AUTO
noinst_PROGRAMS = \
	quickstart-client-0 quickstart-server-0
quickstart_client_0_SOURCES = quickstart_client_0.cpp
quickstart_client_0_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la $(PROJECT_LIBS)

quickstart_server_0_SOURCES = quickstart_server_0.cpp
quickstart_server_0_LDADD = $(top_builddir)/dbus-cxx/libdbus-cxx.la $(PROJECT_LIBS)
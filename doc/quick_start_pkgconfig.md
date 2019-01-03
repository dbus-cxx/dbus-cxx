Using pkgconfig {#quick_start_pkgconfig}
==

Although this is a bit more in-depth than I planned for a quick start tutorial 
I thought I'd include this little blurb on using pkgconfig to help with setting your application to use dbus-cxx.

 dbus-cxx is distributed with two (at the current time) pkgconfig .pc files named dbus-cxx-1.0.pc and dbus-cxx-glibmm-1.0.pc. 
 If you installed the Fedora dbus-cxx-devel or dbus-cxx-glibmm-devel packages you'll find these files installed in a directory 
 named /pkgconfig under in your architecture's library directory (probably /usr/lib/pkgconfig or /usr/lib64/pkgconfig ).

\section quick_start_pkgconfig_autotools Using pkg-config with autotools
If you are using autotools,
 modify configure.ac (or configure.in ) with the following lines:
\code  
PKG_CHECK_MODULES(PROJECT_DBUSCXX,[dbus-cxx-1.0 >= 0.7.0])
AC_SUBST(PROJECT_DBUSCXX_LIBS)
AC_SUBST(PROJECT_DBUSCXX_CFLAGS)
\endcode

You can then use the symbols PROJECT_DBUSCXX_LIBS and PROJECT_DBUSCXX_CFLAGS in your Makefile.am files.
For example, if you have a application named fooapp your Makefile.am might look like this:
\code  
INCLUDES = $(PROJECT_DBUSCXX_CFLAGS)
bin_PROGRAMS = fooapp
fooapp_SOURCES = fooapp.cpp
fooapp_LDADD = $(PROJECT_DBUSCXX_LIBS)
\endcode

\section quick_start_pkgconfig_qt Using pkg-config with QT
Because of QTs signal/slot mechanism, dbus-cxx will not work directly with QT.  There are a few things that need to be
modified in the .pro file of your project.  Add/change the following lines:
\code 
CONFIG += no_keywords
unix:CONFIG += link_pkgconfig
unix:PKGCONFIG += dbus-cxx-1.0
\endcode
At any point in your QT program, you need to use QT signals or slots, use the macros Q_SIGNALS or Q_SLOTS to define 
your signals/slots

<b>Continue On:</b> \ref quick_start_initial_concepts

<b>Go Back:</b> \ref quick_start


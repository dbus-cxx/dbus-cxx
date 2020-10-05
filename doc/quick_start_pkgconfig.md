Using pkgconfig {#quick_start_pkgconfig}
==

Although this is a bit more in-depth than I planned for a quick start tutorial 
I thought I'd include this little blurb on using pkgconfig to help with setting your application to use dbus-cxx.

dbus-cxx is distributed with several pkgconfig .pc files depending on your needs.

|.pc file|Used for|
|--------|--------|
|dbus-cxx-2.0.pc|Using the library directly.  If you're not using a framework/library, this is what you want to use|
|dbus-cxx-qt-2.0.pc|Integrating dbus-cxx with Qt.  Depends on dbus-cxx-2.0.pc|
|dbus-cxx-glib-2.0.pc|Integrating dbus-cxx with GLib.  Depends on dbus-cxx-2.0.pc|

## Using pkg-config with autotools
If you are using autotools, modify configure.ac (or configure.in ) with the following lines:
```
PKG_CHECK_MODULES(PROJECT_DBUSCXX,[dbus-cxx-2.0 >= 2.0.0])
AC_SUBST(PROJECT_DBUSCXX_LIBS)
AC_SUBST(PROJECT_DBUSCXX_CFLAGS)
```

You can then use the symbols `PROJECT_DBUSCXX_LIBS` and `PROJECT_DBUSCXX_CFLAGS` in your Makefile.am files.
For example, if you have a application named fooapp your Makefile.am might look like this:
```
INCLUDES = $(PROJECT_DBUSCXX_CFLAGS)
bin_PROGRAMS = fooapp
fooapp_SOURCES = fooapp.cpp
fooapp_LDADD = $(PROJECT_DBUSCXX_LIBS)
```

## Using pkg-config with Qt
Because of Qt's signal/slot mechanism, dbus-cxx will not work directly with Qt.  Assuming that you are using QMake,
you will need to add the following lines to your .pro file:
```
CONFIG += no_keywords
unix:CONFIG += link_pkgconfig
unix:PKGCONFIG += dbus-cxx-qt-2.0
```
Note that this will also allow you to use the Qt integration with dbus-cxx.

At any point in your Qt program, you need to use Qt signals or slots, use the macros Q_SIGNALS or Q_SLOTS to define 
your signals/slots.

## Using pkg-config with CMake
If your project uses CMake, something along the lines of the following should work:

```
pkg_check_modules( dbus-cxx-2.0 REQUIRED dbus-cxx )

target_include_directories( exe ${dbus-cxx_INCLUDE_DIRS} )
target_link_libraries( exe ${dbus-cxx_LDFLAGS} )
```

<b>Continue On:</b> \ref quick_start_initial_concepts

<b>Go Back:</b> \ref quick_start


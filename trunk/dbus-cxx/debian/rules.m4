#!/usr/bin/make -f

binary/dbus-cxx-tools:: binary/dbus-cxx0

install/dbus-cxx-doc::
	$(MAKE) docs
	mkdir -p $(DEB_DESTDIR)usr/share/gtk-doc/html/dbus-cxx-PACKAGE_RELEASE/reference/html/
	install -p --mode=0664 -t $(DEB_DESTDIR)usr/share/gtk-doc/html/dbus-cxx-PACKAGE_RELEASE/reference/html/ doc/reference/html/*
	install -p --mode=0664 -t $(DEB_DESTDIR)usr/share/gtk-doc/html/dbus-cxx-PACKAGE_RELEASE/ doc/dbus-cxx-PACKAGE_RELEASE.devhelp


include /usr/share/cdbs/1/rules/debhelper.mk
include /usr/share/cdbs/1/class/autotools.mk

DEB_DH_SHLIBDEPS_ARGS += -l$(DEB_DESTDIR)usr/lib -Ldbus-cxx --exclude=dbus-cxx-xml2cpp --exclude=dbus-cxx-introspect
DEB_CONFIGURE_EXTRA_FLAGS += --enable-tools --enable-glibmm --enable-doc

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

EXTRA_DIST = dbus-cxx.spec

SUBDIRS = dbus-cxx tools doc examples unit-tests

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = dbus-cxx-$(PKGCONFIG_VERSION).pc

INCLUDES = -I$(top_srcdir)/. $(PROJECT_CFLAGS)

toplevel_includedir = $(includedir)/dbus-cxx-$(PACKAGE_RELEASE)
toplevel_include_HEADERS = dbus-cxx.h

PACKAGE_RELEASEHOST="rvinyard@frs.sourceforge.net:uploads/"

docs:
	cd doc && make docs

upload-docs:
	cd doc && make upload-docs

upload-docs-only:
	cd doc && make upload-docs-only

upload-release: dist
	scp $(PACKAGE)-$(VERSION).tar.bz2 $(PACKAGE)-$(VERSION).tar.gz $(PACKAGE)-$(VERSION).zip $(PACKAGE_RELEASEHOST)

spec:
	m4 -D PACKAGE_VERSION=$(PACKAGE_VERSION) \
		-D PACKAGE_RELEASE=$(PACKAGE_RELEASE) \
		-D PKGCONFIG_VERSION=$(PKGCONFIG_VERSION) \
		-D API_VERSION=$(PACKAGE_RELEASE) \
		dbus-cxx.spec.m4 >dbus-cxx.spec

changelog:
	svn2cl --authors=svn2cl.authors -r HEAD:1 --group-by-day -a

rpm: spec dist-bzip2
	cp $(PACKAGE)-$(VERSION).tar.bz2 ~/rpmbuild/SOURCES/
	cp $(PACKAGE_NAME).spec ~/rpmbuild/SPECS/
	rpmbuild -ba $(PACKAGE_NAME).spec

srpm: spec dist-bzip2
	cp $(PACKAGE)-$(VERSION).tar.bz2 ~/rpmbuild/SOURCES/
	cp $(PACKAGE_NAME).spec ~/rpmbuild/SPECS/
	rpmbuild -bs $(PACKAGE_NAME).spec

mockbuild: srpm
	mock --rebuild ~/rpmbuild/SRPMS/$(PACKAGE)-$(VERSION)*.src.rpm


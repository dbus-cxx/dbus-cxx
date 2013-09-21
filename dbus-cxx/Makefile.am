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

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
	dbus-cxx.spec \
	m4/ax_cxx_compile_stdcxx_0x.m4 \
	m4/ax_prefix_config_h.m4 \
	ax_dbus_cxx_xml2cpp.m4 \
	debian/changelog                debian/dbus-cxx-glibmm0.dirs		\
	debian/compat                   debian/dbus-cxx-glibmm0.install		\
	debian/control                  debian/dbus-cxx-glibmm-dev.dirs		\
	debian/control.m4               debian/dbus-cxx-glibmm-dev.install	\
	debian/copyright                debian/dbus-cxx-glibmm-dev.install.m4	\
	debian/dbus-cxx0.dirs           debian/dbus-cxx-tools.dirs		\
	debian/dbus-cxx0.install        debian/dbus-cxx-tools.install		\
	debian/dbus-cxx-dev.dirs        debian/dirs				\
	debian/dbus-cxx-dev.install     debian/docs				\
	debian/dbus-cxx-dev.install.m4  debian/rules				\
	debian/dbus-cxx-doc.install     debian/rules.m4				\
	debian/dbus-cxx-doc.install.m4


SUBDIRS = dbus-cxx dbus-cxx-glibmm tools doc examples unit-tests

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = dbus-cxx-$(PKGCONFIG_VERSION).pc dbus-cxx-glibmm-$(PKGCONFIG_VERSION).pc

INCLUDES = -I$(top_srcdir)/. $(PROJECT_CFLAGS)

toplevel_includedir = $(includedir)/dbus-cxx-$(PACKAGE_RELEASE)
toplevel_include_HEADERS = dbus-cxx-glibmm.h dbus-cxx.h

PACKAGE_RELEASEHOST="${USER},dbus-cxx@frs.sourceforge.net:/home/frs/project/d/db/dbus-cxx/dbus-cxx"

docs:
	cd doc && make docs

upload-docs:
	cd doc && make upload-docs

upload-docs-only:
	cd doc && make upload-docs-only

TO_UPLOAD = $(PACKAGE)-$(VERSION).tar.bz2 $(PACKAGE)-$(VERSION).tar.gz $(PACKAGE)-$(VERSION).zip $(PACKAGE)-$(VERSION)-doc.tar.bz2 $(PACKAGE)-$(VERSION)-doc.tar.gz $(PACKAGE)-$(VERSION)-doc.zip
RELEASE_NOTES = $(PACKAGE)-$(VERSION)-release-notes.txt
TO_UPLOAD_RN = $(TO_UPLOAD) $(RELEASE_NOTES)

release-notes:
	echo "dbus-cxx is a C++ wrapper for the dbus library" > $(RELEASE_NOTES)
	echo "http://dbus-cxx.sourceforge.net" >> $(RELEASE_NOTES)
	echo >> $(RELEASE_NOTES)
	echo ===== $(VERSION) ===== >> $(RELEASE_NOTES)
	echo >> $(RELEASE_NOTES)
	echo >> $(RELEASE_NOTES)
	gedit $(RELEASE_NOTES) ChangeLog

upload-release-only:
	rm -rf $(VERSION)
	mkdir $(VERSION)
	if test -e $(RELEASE_NOTES); \
          then \
            cp -a $(TO_UPLOAD_RN) $(VERSION); \
          else \
            cp -a $(TO_UPLOAD) $(VERSION); \
        fi
	scp -Crp $(VERSION) $(PACKAGE_RELEASEHOST)
	rm -rf $(VERSION)

upload-release: changelog release-notes dist upload-release-only

dbus-cxx.spec:
	m4 -D PACKAGE_VERSION=$(PACKAGE_VERSION) \
		-D PACKAGE_RELEASE=$(PACKAGE_RELEASE) \
		-D PKGCONFIG_VERSION=$(PKGCONFIG_VERSION) \
		-D API_VERSION=$(PACKAGE_RELEASE) \
		dbus-cxx.spec.m4 >dbus-cxx.spec

changelog:
	svn2cl --authors=svn2cl.authors -r HEAD:1 --group-by-day -a

rpm: spec dist-bzip2
	cp $(PACKAGE)-$(VERSION).tar.bz2 $(PACKAGE)-$(VERSION)-doc.tar.bz2 ~/rpmbuild/SOURCES/
	cp $(PACKAGE_NAME).spec ~/rpmbuild/SPECS/
	rpmbuild -ba $(PACKAGE_NAME).spec

srpm: spec dist-bzip2
	cp $(PACKAGE)-$(VERSION).tar.bz2 ~/rpmbuild/SOURCES/
	cp $(PACKAGE)-$(VERSION)-doc.tar.bz2 ~/rpmbuild/SOURCES/
	cp $(PACKAGE_NAME).spec ~/rpmbuild/SPECS/
	rpmbuild -bs $(PACKAGE_NAME).spec

srpm-md5: spec dist-bzip2
	cp $(PACKAGE)-$(VERSION).tar.bz2 ~/rpmbuild/SOURCES/
	cp $(PACKAGE)-$(VERSION)-doc.tar.bz2 ~/rpmbuild/SOURCES/
	cp $(PACKAGE_NAME).spec ~/rpmbuild/SPECS/
	rpmbuild-md5 -bs $(PACKAGE_NAME).spec

mockbuild: srpm
	mock --rebuild ~/rpmbuild/SRPMS/$(PACKAGE)-$(VERSION)*.src.rpm

dist-hook:
	cd doc && make docsdist
	mv doc/$(PACKAGE)-$(VERSION)-doc.tar.gz .
	mv doc/$(PACKAGE)-$(VERSION)-doc.tar.bz2 .
	mv doc/$(PACKAGE)-$(VERSION)-doc.zip .

debfiles:
	m4 -D PACKAGE_VERSION=$(PACKAGE_VERSION) \
		-D PACKAGE_RELEASE=$(PACKAGE_RELEASE) \
		-D PKGCONFIG_VERSION=$(PKGCONFIG_VERSION) \
		-D API_VERSION=$(PACKAGE_RELEASE) \
		-D PACKAGE_MAJOR=$(PACKAGE_MAJOR_VERSION) \
		debian/control.m4 >debian/control
	m4 -D PACKAGE_VERSION=$(PACKAGE_VERSION) \
		-D PACKAGE_RELEASE=$(PACKAGE_RELEASE) \
		-D PKGCONFIG_VERSION=$(PKGCONFIG_VERSION) \
		-D API_VERSION=$(PACKAGE_RELEASE) \
		-D PACKAGE_MAJOR=$(PACKAGE_MAJOR_VERSION) \
		debian/dbus-cxx-dev.install.m4 >debian/dbus-cxx-dev.install
	m4 -D PACKAGE_VERSION=$(PACKAGE_VERSION) \
		-D PACKAGE_RELEASE=$(PACKAGE_RELEASE) \
		-D PKGCONFIG_VERSION=$(PKGCONFIG_VERSION) \
		-D API_VERSION=$(PACKAGE_RELEASE) \
		-D PACKAGE_MAJOR=$(PACKAGE_MAJOR_VERSION) \
		debian/dbus-cxx-doc.install.m4 >debian/dbus-cxx-doc.install
	m4 -D PACKAGE_VERSION=$(PACKAGE_VERSION) \
		-D PACKAGE_RELEASE=$(PACKAGE_RELEASE) \
		-D PKGCONFIG_VERSION=$(PKGCONFIG_VERSION) \
		-D API_VERSION=$(PACKAGE_RELEASE) \
		-D PACKAGE_MAJOR=$(PACKAGE_MAJOR_VERSION) \
		debian/dbus-cxx-glibmm-dev.install.m4 >debian/dbus-cxx-glibmm-dev.install
	m4 -D PACKAGE_VERSION=$(PACKAGE_VERSION) \
		-D PACKAGE_RELEASE=$(PACKAGE_RELEASE) \
		-D PKGCONFIG_VERSION=$(PKGCONFIG_VERSION) \
		-D API_VERSION=$(PACKAGE_RELEASE) \
		-D PACKAGE_MAJOR=$(PACKAGE_MAJOR_VERSION) \
		debian/rules.m4 >debian/rules

deb: debfiles
	debclean
	debuild -i -uc -us -b
	debclean

deb-src: debfiles
	debclean
	debuild -i -uc -us -S
	debclean

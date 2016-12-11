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

if DBUS_CXX_DOC

devhelp_file = dbus-cxx-@PACKAGE_RELEASE@.devhelp

EXTRA_DIST = Doxyfile \
	doxygen.css \
	doxygen_header.html \
	doxygen_footer.html \
	doxygen_to_devhelp.xsl \
	doxygen_to_devhelp.xsl.m4 \
	$(devhelp_file) \
	images/bug-small.png          images/documents-small.png  \
	images/bugs-small.png         images/download-small.png   \
	images/dbus-cxx-logo-med.png  images/fedora-logo-tiny.png \
	images/dbus-cxx-logo.png      images/gplv3.png            \
	images/dbus-cxx-logo.svg      images/sourcecode-small.png

mostlyclean-local:
	rm -rf reference

devhelp_stylesheet: doxygen_to_devhelp.xsl.m4
	m4 -D PACKAGE_VERSION=$(PACKAGE_VERSION) \
		-D PACKAGE_RELEASE=$(PACKAGE_RELEASE) \
		-D API_VERSION=$(PACKAGE_RELEASE) \
		doxygen_to_devhelp.xsl.m4 >doxygen_to_devhelp.xsl

doc-clean:
	rm -rf reference

refdocs: doc-clean
	doxygen $(srcdir)/Doxyfile \
	2> doxygen-warnings.txt | tee doxygen-output.txt && cat doxygen-warnings.txt
	cp -arv images/* reference/html/

# DevHelp reference
$(devhelp_file): refdocs devhelp_stylesheet
	xsltproc -o $@ doxygen_to_devhelp.xsl reference/xml/index.xml

docs: $(devhelp_file)

docdistdir="$(PACKAGE)-$(VERSION)"
doctgtdir="$(docdistdir)/doc/reference/"

docsdist: docs
	rm -rf $(docdistdir)
	mkdir $(docdistdir)
	mkdir -p $(doctgtdir)
	cp -arv reference/html/ $(doctgtdir)
	rm -rf `find $(doctgtdir) -name CVS`
	rm -rf `find $(doctgtdir) -name *.md5`
	rm -rf `find $(doctgtdir) -name *.map`
	rm -rf `find $(doctgtdir) -name *.dot`
	tar cvzf $(docdistdir)-doc.tar.gz $(docdistdir)/*
	tar cvjf $(docdistdir)-doc.tar.bz2 $(docdistdir)/*
	zip -r $(docdistdir)-doc.zip $(docdistdir)/*
	rm -rf $(docdistdir)

PACKAGE_WWWHOST="${USER},dbus-cxx@web.sourceforge.net:/home/project-web/dbus-cxx/htdocs/"

upload-docs: docs
	rsync -avu --delete reference/html/ $(PACKAGE_WWWHOST)

upload-docs-only:
	rsync -avu --delete reference/html/ $(PACKAGE_WWWHOST)

endif

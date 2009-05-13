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
	$(devhelp_file)

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

dist-hook: docs
	mkdir -p $(distdir)/reference/
	cp -arv reference/html/ $(distdir)/reference/
	rm -rf `find $(distdir)/reference -name CVS`
	rm -rf `find $(distdir)/reference -name *.md5`
	rm -rf `find $(distdir)/reference -name *.map`
	rm -rf `find $(distdir)/reference -name *.dot`

PACKAGE_WWWHOST="${USER},dbus-cxx@web.sourceforge.net:htdocs/"

upload-docs: docs
	rsync -avu --delete reference/html/ $(PACKAGE_WWWHOST)

upload-docs-only:
	rsync -avu --delete reference/html/ $(PACKAGE_WWWHOST)

endif

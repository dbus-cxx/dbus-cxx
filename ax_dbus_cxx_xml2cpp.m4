dnl #############################################################################
dnl ##   Copyright (C) 2009 by Rick L. Vinyard, Jr.                            ##
dnl ##   rvinyard@cs.nmsu.edu                                                  ##
dnl ##                                                                         ##
dnl ##   This file is part of the dbus-cxx library.                            ##
dnl ##                                                                         ##
dnl ##   The dbus-cxx library is free software; you can redistribute it and/or ##
dnl ##   modify it under the terms of the GNU General Public License           ##
dnl ##   version 3 as published by the Free Software Foundation.               ##
dnl ##                                                                         ##
dnl ##   The dbus-cxx library is distributed in the hope that it will be       ##
dnl ##   useful, but WITHOUT ANY WARRANTY; without even the implied warranty   ##
dnl ##   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   ##
dnl ##   General Public License for more details.                              ##
dnl ##                                                                         ##
dnl ##   You should have received a copy of the GNU General Public             ##
dnl ##   License along with this software. If not see                          ##
dnl ##   <http://www.gnu.org/licenses/>.                                       ##
dnl #############################################################################
dnl @synopsis AX_DBUS_CXX_XML2CPP []
dnl
dnl This is a utility function to check for the dbus-cxx-xml2cpp utility
dnl and will fail if the utility is not found.
dnl
dnl The application's location will be stored in the variable
dnl DBUS_CXX_XML2CPP.
dnl
dnl @category Misc
dnl @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
dnl @version 2009-05-28
dnl @license GPLv3

AC_DEFUN([AX_DBUS_CXX_XML2CPP],[
  AC_PATH_PROG([DBUS_CXX_XML2CPP],[dbus-cxx-xml2cpp],[nodbuscxxxml2cppfound])
  AS_IF([ test $DBUS_CXX_XML2CPP = nodbuscxxxml2cppfound ],[AC_MSG_ERROR("dbus-cxx-xml2cpp needed but not found")])
])

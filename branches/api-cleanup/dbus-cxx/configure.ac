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

#########################################################################
#  package specific values
#########################################################################
pushdef([PACKAGE_MAJOR_VERSION], [0])
pushdef([PACKAGE_MINOR_VERSION], [8])
pushdef([PACKAGE_MICRO_VERSION], [0])
pushdef([PKG_VERSION], PACKAGE_MAJOR_VERSION.PACKAGE_MINOR_VERSION.PACKAGE_MICRO_VERSION)

AC_INIT([dbus-cxx], PKG_VERSION, [rvinyard@cs.nmsu.edu])

[PACKAGE_MAJOR_VERSION]=PACKAGE_MAJOR_VERSION
[PACKAGE_MINOR_VERSION]=PACKAGE_MINOR_VERSION
[PACKAGE_MICRO_VERSION]=PACKAGE_MICRO_VERSION

popdef([PACKAGE_MAJOR_VERSION])
popdef([PACKAGE_MINOR_VERSION])
popdef([PACKAGE_MICRO_VERSION])
popdef([PKG_VERSION])

#
# +1 :  ? : +1 == new interface that does not break old one
# +1 :  ? :  0 == new interface that breaks old one
#  ? :  ? :  0 == no new interfaces, but breaks apps
#  ? : +1 :  ? == just some internal changes, nothing breaks but might work better
# CURRENT : REVISION : AGE
LIBTOOL_SO_VERSION=0:0:0
LIBTOOL_SO_VERSION_MAJOR=0
AC_SUBST(LIBTOOL_SO_VERSION)
AC_SUBST(LIBTOOL_SO_VERSION_MAJOR)

PKGCONFIG_REQUIRES="dbus-1 >= 1.0.0 sigc++-2.0 >= 2.0.0"
PKGCONFIG_VERSION="1.0"

GLIBMM_PKGCONFIG_REQUIRES="glibmm-2.4 >= 2.4.0"

#########################################################################
#  don't touch this
#########################################################################
PACKAGE_RELEASE=$PACKAGE_MAJOR_VERSION.$PACKAGE_MINOR_VERSION
AC_SUBST(PACKAGE_RELEASE)
AC_SUBST(PKGCONFIG_VERSION)

AC_SUBST(LIBTOOL_SO_VERSION)

AC_DEFINE_UNQUOTED(PACKAGE_MAJOR_VERSION, $PACKAGE_MAJOR_VERSION, [Major version])
AC_DEFINE_UNQUOTED(PACKAGE_MINOR_VERSION, $PACKAGE_MINOR_VERSION, [Minor version])
AC_DEFINE_UNQUOTED(PACKAGE_MICRO_VERSION, $PACKAGE_MICRO_VERSION, [Micro version])
AC_SUBST(PACKAGE_MAJOR_VERSION)
AC_SUBST(PACKAGE_MINOR_VERSION)
AC_SUBST(PACKAGE_MICRO_VERSION)

AM_INIT_AUTOMAKE([1.9 tar-ustar dist-bzip2 dist-zip -Wno-portability])
AM_CONFIG_HEADER([config.h])
AX_PREFIX_CONFIG_H([dbus-cxx/dbus-cxx-config.h],[DBUS_CXX],[config.h])

dnl this package needs m4
AC_CHECK_PROGS(M4, gm4 m4, m4)

AC_LANG_CPLUSPLUS
AC_PROG_CXX
AC_PROG_INSTALL
AX_CXX_COMPILE_STDCXX_0X

for top_builddir in . .. ../.. $ac_auxdir $ac_auxdir/..; do
test -f $top_builddir/configure && break
done
AC_PROG_LIBTOOL

AC_HEADER_STDC

AC_CONFIG_MACRO_DIR([m4])

#########################################################################
#  header check utility macros
#########################################################################
AC_DEFUN([AC_REQUIRE_HEADERS],[AC_CHECK_HEADERS($1,,AC_MSG_ERROR(Header $1 not found))])
AC_DEFUN([AC_REQUIRE_LIB],[AC_CHECK_LIB($1,$2,,AC_MSG_ERROR(Library $1 not found))])

AC_CHECK_HEADERS(popt.h)
AC_CHECK_LIB(popt, main)
AC_CHECK_LIB(rt, main)
AC_CHECK_LIB(thread, main)
AC_CHECK_LIB(pthread, main)

AC_CHECK_HEADERS(expat.h)
AC_CHECK_LIB(expat,main)

#########################################################################
#  header checks
#########################################################################
PKG_CHECK_MODULES(DBUS_VER,dbus-1 >= 1.2,AC_DEFINE(HAVE_DBUS_12,[],[If defined, dbus 1.2 or higher is present]),[AC_MSG_RESULT(dbus < 1.2)])

#########################################################################
#  Look for smart pointers in either tr1 or boost
#########################################################################
AC_DEFUN(
  [CHECK_FOR_BOOST_SMART_POINTER],
  [AC_MSG_CHECKING([for Boost smart pointers])
   AC_CHECK_HEADER( [boost/shared_ptr.hpp],
                     [AC_DEFINE(USE_BOOST_SMART_POINTER,[],[If defined, boost library smart pointers will be used])
                     ],
                     [AC_MSG_ERROR([No usable smart pointer library found])
                     ]
                   )
  ]
)

AC_DEFUN(
  [CHECK_FOR_TR1_SMART_POINTER],
  [AC_MSG_CHECKING([for TR1 smart pointers])
   AC_CHECK_HEADER( [tr1/boost_shared_ptr.h],
                     [AC_DEFINE(USE_TR1_SMART_POINTER,[],[If defined TR1 smart pointers will be used])
                     ],
                     [CHECK_FOR_BOOST_SMART_POINTER
                     ],
                     [
                       #include <typeinfo>
                       #include <memory>
                       #include <functional>
                       #ifdef __GNUC__
                         #if __GNUC_ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 1))
                           #include <ext/concurrence.h>
                           #include <tr1/type_traits>
                         #else
                           #include <bits/concurrence.h>
                         #endif
                       #else
                         #include <bits/concurrence.h>
                       #endif
                       #include <ext/mt_allocator.h>
                     ]
                   )
  ]
)

AC_MSG_CHECKING([for c++0x smart pointers])
AS_IF([test x"$with_cpp" != xno && 
       ( test $ax_cv_cxx_compile_cxx0x_native = yes || 
       test $ax_cv_cxx_compile_cxx0x_cxx = yes ) ],
      [
        AS_IF([test $ax_cv_cxx_compile_cxx0x_cxx = yes],
              [ CXXFLAGS="$CXXFLAGS -std=c++0x"
                CXX_STD="-std=c++0x"
                AC_SUBST(CXX_STD)
                AC_SUBST(CXXFLAGS)
               ])
        AC_DEFINE(USE_CXX0X_SMART_POINTER,[],[If defined c++0x smart pointers will be used])
        AC_MSG_RESULT(yes)
      ],
      [ AC_MSG_RESULT(no)
        CPP0XFLAGS=""
        AC_SUBST(CPP0XFLAGS)
        CHECK_FOR_TR1_SMART_POINTER
      ]
     )

AC_TYPE_LONG_LONG_INT
AC_TYPE_UNSIGNED_LONG_LONG_INT
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long int])
AC_CHECK_SIZEOF([long long int])

#########################################################################
#  pkgconfig package requirements
#########################################################################
PKG_CHECK_MODULES([PROJECT],[$PKGCONFIG_REQUIRES])
AC_SUBST(PKGCONFIG_REQUIRES)
AC_SUBST(PROJECT_LIBS)
AC_SUBST(PROJECT_CFLAGS)

AC_ARG_ENABLE([doc],[AS_HELP_STRING([--enable-doc],[Build documentation [default=no]])],[enable_doc=$enableval],[enable_doc=no])
AM_CONDITIONAL(DBUS_CXX_DOC, [test "x${enable_doc}" == xyes])

AC_ARG_ENABLE([glibmm],[AS_HELP_STRING([--enable-glibmm],[Build glibmm support [default=no]])],[enable_glibmm=$enableval],[enable_glibmm=no])
AS_IF([test "x${enable_glibmm}" == xyes],
  [ PKG_CHECK_MODULES(DBUS_CXX_GLIBMM,[$GLIBMM_PKGCONFIG_REQUIRES])
  ],
  [])
AM_CONDITIONAL(DBUS_CXX_GLIBMM, [test "x${enable_glibmm}" == xyes])
AC_SUBST(GLIBMM_PKGCONFIG_REQUIRES)
AC_SUBST(DBUS_CXX_GLIBMM_LIBS)
AC_SUBST(DBUS_CXX_GLIBMM_CFLAGS)

AC_ARG_ENABLE([unit-tests],[AS_HELP_STRING([--enable-unit-tests],[Build dbus-cxx unit test applications [default=no]])],[enable_unit_tests=$enableval],[enable_unit_tests=no])
AS_IF([test "x${enable_unit_tests}" == xyes],
  [ PKG_CHECK_MODULES([UNIT_TEST],[cppunit])
  ],
  [])
AM_CONDITIONAL(DBUS_CXX_UNIT_TESTS, [test "x${enable_unit_tests}" == xyes])
AC_SUBST(UNIT_TEST_LIBS)
AC_SUBST(UNIT_TEST_CFLAGS)

AC_ARG_ENABLE([hal],[AS_HELP_STRING([--enable-hal],[Build dbus-cxx-hal [default=no]])],[enable_hal=$enableval],[enable_hal=no])
AM_CONDITIONAL(DBUS_CXX_HAL, [test "x${enable_hal}" == xyes])

AC_ARG_ENABLE([examples],[AS_HELP_STRING([--enable-examples],[Build dbus-cxx examples [default=no]])],[enable_examples=$enableval],[enable_examples=no])
AM_CONDITIONAL(DBUS_CXX_EXAMPLES, [test "x${enable_examples}" == xyes])

AC_ARG_ENABLE([tools],[AS_HELP_STRING([--enable-tools],[Build dbus-cxx tools [default=no]])],[enable_tools=$enableval],[enable_tools=no])
AM_CONDITIONAL(DBUS_CXX_TOOLS, [test "x${enable_tools}" == xyes])

AC_ARG_ENABLE([debug-output],[AS_HELP_STRING([--enable-debug-output],[Enable debugging output [default=no]])],[AC_DEFINE([DEBUG_ENABLED],[],[Enable debugging output])])

AC_ARG_WITH([cpp],[AS_HELP_STRING([--without-cpp],[Dont use C++0x Smart Pointers [default=no]])],[],[with_cpp=yes])

#debug_default="no"
#AC_ARG_ENABLE(debug,[AS_HELP_STRING([--enable-debug=[no/full]],[Turn on debugging #[default=${debug_default}]])],,[enable_debug=${debug_default}])
#AS_IF([test "x${enable_debug}" == xfull],[ PROJECT_CFLAGS="$PROJECT_CFLAGS -O0 -g3 -DDEBUG" ])

AC_OUTPUT(Makefile dbus-cxx-1.0.pc dbus-cxx-glibmm-1.0.pc dbus-cxx-glibmm/Makefile \
	dbus-cxx/Makefile doc/Makefile examples/Makefile examples/basics/Makefile \
	examples/basics/methods/Makefile examples/basics/signals/Makefile examples/basics/types/Makefile \
	examples/glibmm/Makefile examples/quick-start/Makefile examples/xml2cpp/Makefile \
	examples/xml2cpp/calculator-interface/Makefile examples/xml2cpp/calculator/Makefile \
	examples/xml2cpp/computer-has-a/Makefile examples/xml2cpp/computer-is-a/Makefile \
	tools/Makefile unit-tests/Makefile)


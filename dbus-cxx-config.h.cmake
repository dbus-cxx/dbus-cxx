/***************************************************************************
 *   Copyright (C) 2020 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 *                                                                         *
 *   The dbus-cxx library is free software; you can redistribute it and/or *
 *   modify it under the terms of the GNU General Public License           *
 *   version 3 as published by the Free Software Foundation.               *
 *                                                                         *
 *   The dbus-cxx library is distributed in the hope that it will be       *
 *   useful, but WITHOUT ANY WARRANTY; without even the implied warranty   *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   *
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this software. If not see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#ifndef DBUSCXX_CONFIG_H
#define DBUSCXX_CONFIG_H

#cmakedefine DBUS_CXX_HAVE_DBUS_12
#cmakedefine DBUS_CXX_SIZEOF_LONG_INT @DBUS_CXX_SIZEOF_LONG_INT@
#cmakedefine DBUS_CXX_HAS_CXXABI_H @DBUS_CXX_HAS_CXXABI_H@
#cmakedefine DBUS_CXX_HAS_CXA_DEMANGLE @DBUS_CXX_HAS_CXA_DEMANGLE@

#define DBUS_CXX_PACKAGE_MAJOR_VERSION ${DBUS_CXX_PACKAGE_MAJOR_VERSION}
#define DBUS_CXX_PACKAGE_MINOR_VERSION ${DBUS_CXX_PACKAGE_MINOR_VERSION}
#define DBUS_CXX_PACKAGE_MICRO_VERSION ${DBUS_CXX_PACKAGE_MICRO_VERSION}

#endif DBUSCXX_CONFIG_H

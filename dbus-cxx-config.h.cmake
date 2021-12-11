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

#cmakedefine DBUS_CXX_HAS_CXXABI_H @DBUS_CXX_HAS_CXXABI_H@
#cmakedefine DBUS_CXX_HAS_CXA_DEMANGLE @DBUS_CXX_HAS_CXA_DEMANGLE@

#define DBUS_CXX_PACKAGE_MAJOR_VERSION ${dbus-cxx_VERSION_MAJOR}
#define DBUS_CXX_PACKAGE_MINOR_VERSION ${dbus-cxx_VERSION_MINOR}
#define DBUS_CXX_PACKAGE_MICRO_VERSION ${dbus-cxx_VERSION_PATCH}

#cmakedefine01 DBUS_CXX_HAS_PROP_CONST

#if DBUS_CXX_HAS_PROP_CONST
#include <experimental/propagate_const>
#define DBUS_CXX_PROPAGATE_CONST(T) std::experimental::propagate_const<T>
#else
#define DBUS_CXX_PROPAGATE_CONST(T) T
#endif

#endif /* DBUSCXX_CONFIG_H */

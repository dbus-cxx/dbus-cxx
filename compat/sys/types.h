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
/*
 * sys/types.h compatibility shim
 */
#ifdef _MSC_VER
#if _MSC_VER >= 1900
#include <../ucrt/sys/types.h>
#else
#include <../include/sys/types.h>
#endif
#else
#include_next <sys/types.h>
#endif

#ifndef DBUSCXX_SYS_TYPES_H
#define DBUSCXX_SYS_TYPES_H

#include <stdint.h>

#ifdef _MSC_VER
typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;
typedef uint32_t        in_addr_t;
typedef uint32_t        mode_t;
typedef uint32_t        __uid_t;
typedef __uid_t         uid_t;

#include <basetsd.h>
typedef SSIZE_T ssize_t;

#endif /* _MSC_VER */

#endif /* DBUSCXX_SYS_TYPES_H */

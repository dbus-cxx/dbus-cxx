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
 * fcntl.h compatibility shim
 */
#ifndef _WIN32
#include_next <fcntl.h>
#else

#ifdef _MSC_VER
#if _MSC_VER >= 1900
#include <../ucrt/fcntl.h>
#else
#include <../include/fcntl.h>
#endif
#else
#include_next <fcntl.h>
#endif /* _MSC_VER */

/* fcntl(2) requests */
#define F_DUPFD         0 /* Duplicate fildes */
#define F_GETFD         1 /* Get fildes flags (close on exec) */
#define F_SETFD         2 /* Set fildes flags (close on exec) */
#define F_GETFL         3 /* Get file flags */
#define F_SETFL         4 /* Set file flags */
#define F_DUPFD_CLOEXEC 1030

#ifdef __cplusplus
extern "C" {
#endif

int fcntl( int fd, int action, /* arg */... );

#ifdef __cplusplus
}
#endif

#endif /* _WIN32 */

#ifndef O_NONBLOCK
#define O_NONBLOCK      0x100000
#endif

#ifndef O_CLOEXEC
#define O_CLOEXEC       0x200000
#endif

#ifndef FD_CLOEXEC
#define FD_CLOEXEC      1
#endif

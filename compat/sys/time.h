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
 * sys/time.h compatibility shim
 */
#ifndef DBUSCXX_SYS_TIME_H
#define DBUSCXX_SYS_TIME_H

#ifdef _MSC_VER
#include <winsock2.h>
#ifdef __cplusplus
extern "C" {
#endif
int gettimeofday(struct timeval *tp, void *tzp);
#ifdef __cplusplus
}
#endif
#else
#include_next <sys/time.h>
#endif

#endif /* DBUSCXX_SYS_TIME_H */

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
 * time.h compatibility shim
 */

#ifdef _MSC_VER
#if _MSC_VER >= 1900
#include <../ucrt/time.h>
#else
#include <../include/time.h>
#endif
#else
#include_next <time.h>
#endif

#ifndef DBUSCXX_TIME_H
#define DBUSCXX_TIME_H

#if defined(_WIN32) && !defined(HAVE_LOCALTIME_R)
#define localtime_r(_clock, _result) _localtime64_s(_result, _clock)
#endif

#endif /* DBUSCXX_TIME_H */

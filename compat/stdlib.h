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
 * stdlib.h compatibility shim
 */
#ifdef _MSC_VER
#if _MSC_VER >= 1900
#include <../ucrt/stdlib.h>
#else
#include <../include/stdlib.h>
#endif
#else
#include_next <stdlib.h>
#endif

#ifndef DBUSCXX_STDLIB_H
#define DBUSCXX_STDLIB_H

#include <sys/types.h>
#include <stdint.h>

#endif /* DBUSCXX_STDLIB_H */

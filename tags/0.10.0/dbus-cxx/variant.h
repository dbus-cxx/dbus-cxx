/***************************************************************************
 *   Copyright (C) 2009,2010 by Rick L. Vinyard, Jr.                       *
 *   rvinyard@cs.nmsu.edu                                                  *
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

#ifndef DBUS_CXX_VARIANT
#define DBUS_CXX_VARIANT

namespace DBus{

/**
 * A Variant is a type of DBus object that can be one of several
 * different types of normal DBus object.
 *
 * In other words, a variant can hold different values, but only
 * one value at a time.
 */
template <class T>
class Variant{
	public:

	T data;
	Variant(){
		data = 0;
	}

	Variant( T& idata ){
		data = idata;
	}

};

}

#endif

// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
/***************************************************************************
 *   Copyright (C) 2009 by Rick L. Vinyard, Jr.                            *
 *   rvinyard@cs.nmsu.edu                                                  *
 *   Copyright (C) 2019 by Robert Middleton                                *
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
#include "custom-type.h"
#include <iostream>

DBus::MessageIterator& operator>>( DBus::MessageIterator& i, struct custom& c ) {
    std::cout << "extracting custom" << std::endl;
    c.first = i.get_int32();
    i.next();
    c.second = i.get_int32();
    i.next();
    return i;
}

DBus::MessageAppendIterator& operator<<( DBus::MessageAppendIterator& i, const struct custom& c ) {
    std::cout << "appending to iterator" << std::endl;
    i << ( int32_t )c.first;
    i << ( int32_t )c.second;
    return i;
}

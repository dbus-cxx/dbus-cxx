// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
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
#include <dbus-cxx.h>
#include <dbus-cxx/validator.h>
#include <unistd.h>
#include <iostream>

#include "test_macros.h"

bool validate_one_section_bus_name() {
    return !DBus::Validator::validate_bus_name( "dbuscxx" );
}

bool validate_two_section_bus_name() {
    return DBus::Validator::validate_bus_name( "dbuscxx.test" );
}

bool validate_three_section_bus_name() {
    return DBus::Validator::validate_bus_name( "dbuscxx.test.foo" );
}

#define ADD_TEST(name) do{ if( test_name == STRINGIFY(name) ){ \
            ret = validate_##name();\
        } \
    } while( 0 )

int main( int argc, char** argv ) {
    if( argc < 1 ) {
        return 1;
    }

    std::string test_name = argv[1];
    bool ret = false;

    ADD_TEST( one_section_bus_name );
    ADD_TEST( two_section_bus_name );
    ADD_TEST( three_section_bus_name );

    return !ret;
}

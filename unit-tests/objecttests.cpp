// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
/***************************************************************************
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
#include <dbus-cxx.h>

#include "test_macros.h"

std::shared_ptr<DBus::Dispatcher> dispatch;

double example_method( double, double ) {
    return 0.0;
}

bool object_proxy_create() {
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    std::shared_ptr<DBus::ObjectProxy> object = conn->create_object_proxy( "test", "/test/path" );

    TEST_ASSERT_RET_FAIL( object );
    return true;
}

bool object_proxy_create_method1() {
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    std::shared_ptr<DBus::ObjectProxy> object = conn->create_object_proxy( "test", "/test/path" );
    std::shared_ptr<DBus::MethodProxy<double( double, double )>> proxy = object->create_method<double( double, double )>( "Example.path", "method_name" );
    TEST_ASSERT_RET_FAIL( proxy );
    return true;
}

bool object_export_method() {
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    std::shared_ptr<DBus::Object> object = conn->create_object( "/another/path", DBus::ThreadForCalling::DispatcherThread );
    std::shared_ptr<DBus::Method<double( double, double )>> method = object->create_method<double( double, double )>( "method_name", sigc::ptr_fun( example_method ) );
    TEST_ASSERT_RET_FAIL( method );
    return true;
}

#define ADD_TEST(name) do{ if( test_name == STRINGIFY(name) ){ \
            ret = object_##name();\
        } \
    } while( 0 )

int main( int argc, char** argv ) {
    if( argc < 1 ) {
        return 1;
    }

    std::string test_name = argv[1];
    bool ret = false;

    dispatch = DBus::StandaloneDispatcher::create();

    ADD_TEST( proxy_create );
    ADD_TEST( proxy_create_method1 );
    ADD_TEST( export_method );

    return !ret;
}

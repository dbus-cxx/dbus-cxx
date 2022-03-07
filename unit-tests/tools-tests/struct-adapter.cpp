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
#include "structtypesAdapter.h"

class MyStructType : public test_struct_interface {
public:
    void structMethod( std::tuple<int, int> struct1, std::tuple<int, std::tuple<int, int>> struct2 ) {
    }

    void mapMethod( std::map<std::string, DBus::Variant> map ) {}

    void arrayMethod( std::vector<double> vector ) {}

    void mapMethod2( std::map<std::string, std::string> map ) {}
};

int main() {
    std::shared_ptr<DBus::Dispatcher> dispatch = DBus::StandaloneDispatcher::create();
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    MyStructType myStruct;
    std::shared_ptr<test_struct_interfaceInterface> interfaceptr = test_struct_interfaceInterface::create( &myStruct );
    std::shared_ptr<structtypesAdapter> ptr = structtypesAdapter::create( conn, interfaceptr, "/path" );
}

// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
#include "SimpleTypesAdapter.h"

class MyType : public test_simpletype_interface {
public:
    void typesMethod( int first, double second ) {
    }
};

int main() {
    std::shared_ptr<DBus::Dispatcher> dispatch = DBus::StandaloneDispatcher::create();
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    MyType myType;
    std::shared_ptr<test_simpletype_interfaceInterface> simpleInterface =
            test_simpletype_interfaceInterface::create( &myType );
    std::shared_ptr<SimpleTypesAdapter> ptr = SimpleTypesAdapter::create( conn, simpleInterface, "/path" );
}

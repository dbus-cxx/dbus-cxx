// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
#include "DbusTacoAdapter.h"

class TacoAdaptee : public com_amazon_josharenson_TacoOrdererInterface {
    std::string OrderTaco( std::string meatType ) {
        return "ordering " + meatType;
    }
};

int main() {
    std::shared_ptr<DBus::Dispatcher> dispatch = DBus::StandaloneDispatcher::create();
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    TacoAdaptee taco;
    std::shared_ptr<com_amazon_josharenson_TacoOrdererInterfaceInterface> taco_interface =
            com_amazon_josharenson_TacoOrdererInterfaceInterface::create( &taco );
    std::shared_ptr<DbusTacoAdapter> ptr = DbusTacoAdapter::create( conn, taco_interface );
}

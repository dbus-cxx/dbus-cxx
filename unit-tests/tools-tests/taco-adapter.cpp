#include "DbusTacoAdapter.h"

class TacoAdaptee : public DbusTacoAdaptee {
    std::string OrderTaco( std::string meatType ) {
        return "ordering " + meatType;
    }
};

int main() {
    std::shared_ptr<DBus::Dispatcher> dispatch = DBus::StandaloneDispatcher::create();
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    TacoAdaptee taco;
    std::shared_ptr<DbusTacoAdapter> ptr = DbusTacoAdapter::create( &taco );
}

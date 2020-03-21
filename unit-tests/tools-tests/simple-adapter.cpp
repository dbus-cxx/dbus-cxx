#include "SimpleTypesAdapter.h"

class MyType : public SimpleTypesAdaptee {
public:
	void typesMethod( int first, double second ){
	}
};

int main(){
    DBus::init();

    std::shared_ptr<DBus::Dispatcher> dispatch = DBus::Dispatcher::create();
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    MyType myType;
    std::shared_ptr<SimpleTypesAdapter> ptr = SimpleTypesAdapter::create( &myType, "/path" );
}

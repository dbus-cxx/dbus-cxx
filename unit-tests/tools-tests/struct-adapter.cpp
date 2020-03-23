#include "structtypesAdapter.h"

class MyStructType : public structtypesAdaptee {
public:
	void structMethod( std::tuple<int,int> struct1, std::tuple<int,std::tuple<int,int>> struct2 ){
	}

	void mapMethod( std::map<std::string,DBus::Variant> map ){}

	void arrayMethod( std::vector<double> vector ){}

	void mapMethod2( std::map<std::string,std::string> map ){}
};

int main(){
    DBus::init();

    std::shared_ptr<DBus::Dispatcher> dispatch = DBus::Dispatcher::create();
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    MyStructType myStruct;
    std::shared_ptr<structtypesAdapter> ptr = structtypesAdapter::create( &myStruct, "/path" );
}

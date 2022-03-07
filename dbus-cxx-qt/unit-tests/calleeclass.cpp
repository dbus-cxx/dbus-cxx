// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
#include "calleeclass.h"

CalleeClass::CalleeClass( std::string path ) :
    DBus::Object ( path )
{
    create_method<int(int,int)>( "test.foo", "add", sigc::mem_fun( *this, &CalleeClass::add ) );
}

std::shared_ptr<CalleeClass> CalleeClass::create( std::string path ){
    return std::shared_ptr<CalleeClass>( new CalleeClass( path ) );
}

int CalleeClass::add(int a, int b){
    return a + b;
}

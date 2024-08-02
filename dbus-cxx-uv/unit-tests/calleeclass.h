// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
#ifndef CALLEECLASS_H
#define CALLEECLASS_H

#include <dbus-cxx.h>

class CalleeClass : public DBus::Object
{
private:
    CalleeClass( std::string path );

public:
    static std::shared_ptr<CalleeClass> create( std::string path );

    int add( int a, int b );
};

#endif // CALLEECLASS_H


// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause

#include <uv.h>

#include <dbus-cxx-uv.h>

#include "calleeclass.h"

int value = 0;

static void timer_cb(uv_timer_t* h)
{
    auto methodref = *reinterpret_cast<std::shared_ptr<DBus::MethodProxy<int(int,int)>>*>(h->data);
    value = (*methodref)( 5, 10 ); 
    uv_stop(uv_default_loop());
}

int main(int argc, char** argv){
    std::shared_ptr<DBus::Dispatcher> disp = DBus::Uv::UvDispatcher::create();
    std::shared_ptr<DBus::Connection> conn = disp->create_connection( DBus::BusType::SESSION );
    std::shared_ptr<DBus::ObjectProxy> proxy = conn->create_object_proxy( "dbuscxx.test", "/test" );
    std::shared_ptr<DBus::MethodProxy<int(int,int)>> methodref = proxy->create_method<int(int,int)>( "test.foo", "add" );

    uv_timer_t uv_timer;

    if (int r = uv_timer_init(uv_default_loop(), &uv_timer); r < 0) {
        fprintf(stderr, "libuv failure: %s\n", uv_strerror(r));
        return -1;
    }

    uv_timer.data = &methodref;

    if (int r = uv_timer_start(&uv_timer, timer_cb, 100, 0); r < 0) {
        fprintf(stderr, "libuv failure: %s\n", uv_strerror(r));
        return -1;
    }

    if (int r = uv_run(uv_default_loop(), UV_RUN_DEFAULT); r < 0) {
        fprintf(stderr, "libuv failure: %s\n", uv_strerror(r));
        return -1;
    }
    
    fprintf(stderr, "*** %s got %d\n", argv[0], value);

    return value != 15;
}

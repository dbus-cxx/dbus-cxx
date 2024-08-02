// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause

#include <uv.h>

#include <dbus-cxx-uv.h>

#include "calleeclass.h"

static void timer_cb(uv_timer_t* h)
{
    uv_stop(uv_default_loop());
}

int main(int argc, char** argv){
    DBus::set_logging_function( DBus::log_std_err );
    DBus::set_log_level( SL_TRACE );

    std::shared_ptr<DBus::Dispatcher> disp = DBus::Uv::UvDispatcher::create();

    std::shared_ptr<DBus::Connection> conn = disp->create_connection( DBus::BusType::SESSION );
    conn->request_name( "dbuscxx.test" );
    std::shared_ptr<CalleeClass> callee = CalleeClass::create( "/test" );

    conn->register_object( callee, DBus::ThreadForCalling::CurrentThread );

    uv_timer_t uv_timer;

    if (int r = uv_timer_init(uv_default_loop(), &uv_timer); r < 0) {
        fprintf(stderr, "libuv failure: %s\n", uv_strerror(r));
        return -1;
    }

    if (int r = uv_timer_start(&uv_timer, timer_cb, 1000, 0); r < 0) {
        fprintf(stderr, "libuv failure: %s\n", uv_strerror(r));
        return -1;
    }

    fprintf(stderr, "*** Before libuv loop\n");

    if (int r = uv_run(uv_default_loop(), UV_RUN_DEFAULT); r < 0) {
        fprintf(stderr, "libuv failure: %s\n", uv_strerror(r));
        return -1;
    }

    fprintf(stderr, "*** After libuv loop\n");
            
    return 0;
}

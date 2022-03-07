// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
#include <QCoreApplication>
#include <QTimer>

#include <dbus-cxx-qt.h>

#include "calleeclass.h"

int main(int argc, char** argv){
    QCoreApplication a(argc, argv);
    std::shared_ptr<DBus::Dispatcher> disp;
    std::shared_ptr<DBus::Qt::QtThreadDispatcher> thrDisp;
    bool use_qt_dispatcher = true;
    std::string dispatch_type( argv[ 1 ] );

    DBus::set_logging_function( DBus::log_std_err );
    DBus::set_log_level( SL_TRACE );

    if( dispatch_type == "standalone" ){
        use_qt_dispatcher = false;
    }

    if( use_qt_dispatcher ){
        disp = DBus::Qt::QtDispatcher::create();
    }else{
        disp = DBus::StandaloneDispatcher::create();
    }

    std::shared_ptr<DBus::Connection> conn = disp->create_connection( DBus::BusType::SESSION );
    conn->request_name( "dbuscxx.test" );
    std::shared_ptr<CalleeClass> callee = CalleeClass::create( "/test" );

    conn->register_object( callee, DBus::ThreadForCalling::CurrentThread );

    if( !use_qt_dispatcher ){
        thrDisp = DBus::Qt::QtThreadDispatcher::create();
        conn->add_thread_dispatcher( thrDisp );
    }

    QTimer timer;
    QObject::connect( &timer, &QTimer::timeout,
             []{
        QCoreApplication* app = QCoreApplication::instance();
        app->exit();
    });

    timer.start( 1000 );

    return a.exec();
}

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
    bool use_dispatcher = true;
    std::string dispatch_type( argv[ 1 ] );

    if( dispatch_type == "standalone" ){
        use_dispatcher = false;
    }

    if( use_dispatcher ){
        disp = DBus::Qt::QtDispatcher::create();
    }else{
        disp = DBus::StandaloneDispatcher::create();
    }

    std::shared_ptr<DBus::Connection> conn = disp->create_connection( DBus::BusType::SESSION );
    std::shared_ptr<DBus::ObjectProxy> proxy = conn->create_object_proxy( "dbuscxx.test", "/test" );
    std::shared_ptr<DBus::MethodProxy<int(int,int)>> methodref = proxy->create_method<int(int,int)>( "test.foo", "add" );

    QTimer timer;
    QObject::connect( &timer, &QTimer::timeout,
             [methodref]{
        int value = (*methodref)( 5, 10 );
        int retval = (value != 15);

        QCoreApplication* app = QCoreApplication::instance();
        app->exit( retval );
    });

    timer.start( 100 );

    return a.exec();
}

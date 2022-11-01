// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
/***************************************************************************
 *   Copyright (C) 2020 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 *                                                                         *
 *   The dbus-cxx library is free software; you can redistribute it and/or *
 *   modify it under the terms of the GNU General Public License           *
 *   version 3 as published by the Free Software Foundation.               *
 *                                                                         *
 *   The dbus-cxx library is distributed in the hope that it will be       *
 *   useful, but WITHOUT ANY WARRANTY; without even the implied warranty   *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   *
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this software. If not see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#include "signalNameProxy.h"
#include <iostream>
#include <thread>

static std::string value;

static void dosomething( std::string in ) {
    value = in;
}

int main( int argc, char** argv ) {
    DBus::set_logging_function( DBus::log_std_err );
    DBus::set_log_level( SL_TRACE );

    std::shared_ptr<DBus::Dispatcher> dispatch = DBus::StandaloneDispatcher::create();
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    std::shared_ptr<signalNameProxy> proxy = signalNameProxy::create( conn );
    std::shared_ptr<DBus::SignalProxy<void(std::string)>> signalProxy = proxy->getsignal_name_interfaceInterface()->signal_exampleSignal();
    signalProxy->connect( sigc::ptr_fun( dosomething ) );

    if( argc > 1 ){
        std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

        if( value == "hi there" ){
            return 0;
        }

        return 1;
    }

    return 0;
}

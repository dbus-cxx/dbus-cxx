/***************************************************************************
 *   Copyright (C) 2019 by Robert Middleton                                *
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
#include <dbus-cxx.h>
#include <unistd.h>

#include "test_macros.h"

DBus::Dispatcher::pointer dispatch;
std::string signal_value;

void sigHandle( std::string value ){
    signal_value = value;
}

bool signal_create(){
    DBus::Connection::pointer conn = dispatch->create_connection(DBus::BUS_SESSION);

    DBus::signal<void,std::string>::pointer signal = conn->create_signal<void,std::string>( "/test/signal", "test.signal.type" );

    TEST_ASSERT_RET_FAIL( signal );
    return true;
}

bool signal_tx_rx(){
    DBus::Connection::pointer conn = dispatch->create_connection(DBus::BUS_SESSION);

    DBus::signal<void,std::string>::pointer signal = conn->create_signal<void,std::string>( "/test/signal", "test.signal.type", "Path" );
    DBus::signal_proxy<void,std::string>::pointer proxy = conn->create_signal_proxy<void,std::string>( "/test/signal", "test.signal.type", "Path" );

    proxy->connect( sigc::ptr_fun( sigHandle ) );

    signal->emit( "TestSignal" );
    sleep( 1 );

    TEST_ASSERT_RET_FAIL( signal_value.compare( "TestSignal" ) == 0 );
    return true;
}

#define ADD_TEST(name) do{ if( test_name == STRINGIFY(name) ){ \
  ret = signal_##name();\
} \
} while( 0 )

int main(int argc, char** argv){
  if(argc < 1)
    return 1;

  std::string test_name = argv[1];
  bool ret = false;

  DBus::init();
  dispatch = DBus::Dispatcher::create();

  ADD_TEST(create);
  ADD_TEST(tx_rx);

  return !ret;
}

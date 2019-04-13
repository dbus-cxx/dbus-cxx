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

#include "test_macros.h"

DBus::Dispatcher::pointer dispatch;

bool connection_create_signal_proxy(){
    DBus::Connection::pointer conn = dispatch->create_connection(DBus::BUS_SESSION);

    DBus::signal_proxy_simple::pointer proxy = conn->create_signal_proxy("interface.name", "myname");

    DBus::Connection::InterfaceToNameProxySignalMap map = conn->get_signal_proxies();
    TEST_ASSERT_RET_FAIL( map.size() == 1 );

    DBus::Connection::InterfaceToNameProxySignalMap::iterator name_map_location = map.find("interface.name");
    TEST_ASSERT_RET_FAIL( name_map_location != map.end() );

    DBus::Connection::NameToProxySignalMap name_map = name_map_location->second;
    TEST_ASSERT_RET_FAIL( name_map.size() == 1 );

    return true;
}

bool connection_get_signal_proxy_by_iface(){
    DBus::Connection::pointer conn = dispatch->create_connection(DBus::BUS_SESSION);

    DBus::signal_proxy_simple::pointer proxy = conn->create_signal_proxy("interface.name", "myname");

    DBus::Connection::NameToProxySignalMap signals = conn->get_signal_proxies("interface.name");
    TEST_ASSERT_RET_FAIL( signals.size() == 1 );

    return true;
}

bool connection_get_signal_proxy_by_iface_and_name(){
    DBus::Connection::pointer conn = dispatch->create_connection(DBus::BUS_SESSION);

    DBus::signal_proxy_simple::pointer proxy = conn->create_signal_proxy("interface.name", "myname");

    DBus::Connection::ProxySignals signals = conn->get_signal_proxies("interface.name", "myname");
    TEST_ASSERT_RET_FAIL( signals.size() == 1 );
    TEST_ASSERT_RET_FAIL( signals.front() == proxy );

    return true;
}

#define ADD_TEST(name) do{ if( test_name == STRINGIFY(name) ){ \
  ret = connection_##name();\
} \
} while( 0 )

int main(int argc, char** argv){
  if(argc < 1)
    return 1;

  std::string test_name = argv[1];
  bool ret = false;

  DBus::init();
  dispatch = DBus::Dispatcher::create();

  ADD_TEST(create_signal_proxy);
  ADD_TEST(get_signal_proxy_by_iface);
  ADD_TEST(get_signal_proxy_by_iface_and_name);

  return !ret;
}

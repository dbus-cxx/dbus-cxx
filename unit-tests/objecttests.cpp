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

double example_method( double, double ){
    return 0.0;
}

bool object_proxy_create(){
    DBus::Connection::pointer conn = dispatch->create_connection(DBus::BUS_SESSION);

    DBus::ObjectProxy::pointer object = conn->create_object_proxy("test", "/test/path");

    TEST_ASSERT_RET_FAIL( object );
    return true;
}

bool object_proxy_create_method1(){
    DBus::Connection::pointer conn = dispatch->create_connection(DBus::BUS_SESSION);

    DBus::ObjectProxy::pointer object = conn->create_object_proxy("test", "/test/path");
    DBus::MethodProxy<double,double,double>::pointer proxy = object->create_method<double,double,double>( "Example.path", "method_name" );
    TEST_ASSERT_RET_FAIL( proxy );
    return true;
}

bool object_export_method(){
    DBus::Connection::pointer conn = dispatch->create_connection(DBus::BUS_SESSION);

    DBus::Object::pointer object = conn->create_object( "/another/path" );
    DBus::Method<double,double,double>::pointer method = object->create_method<double,double,double>( "method_name", sigc::ptr_fun( example_method ) );
    TEST_ASSERT_RET_FAIL( method );
    return true;
}

#define ADD_TEST(name) do{ if( test_name == STRINGIFY(name) ){ \
  ret = object_##name();\
} \
} while( 0 )

int main(int argc, char** argv){
  if(argc < 1)
    return 1;

  std::string test_name = argv[1];
  bool ret = false;

  DBus::init();
  dispatch = DBus::Dispatcher::create();

  ADD_TEST(proxy_create);
  ADD_TEST(proxy_create_method1);
  ADD_TEST(export_method);

  return !ret;
}

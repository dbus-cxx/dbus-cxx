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
//#include <dbus-cxx.h>
#include <unistd.h>

#include "test_macros.h"
#include "custom-type.h"

std::shared_ptr<DBus::Dispatcher> dispatch;
std::shared_ptr<DBus::Connection> conn;

std::shared_ptr<DBus::ObjectProxy> proxy;
std::shared_ptr<DBus::MethodProxy<int(int,int)>> int_method_proxy;
std::shared_ptr<DBus::MethodProxy<void()>> void_method_proxy;
std::shared_ptr<DBus::MethodProxy<void(struct custom)>> void_custom_method_proxy;
std::shared_ptr<DBus::MethodProxy<int(int, struct custom)>> int_custom_method_proxy2;
std::shared_ptr<DBus::MethodProxy<int(int, std::vector<struct custom>)>> int_vec_custom_method_proxy2;

std::shared_ptr<DBus::Object> object;
std::shared_ptr<DBus::Method<int(int,int)>> int_method;
std::shared_ptr<DBus::Method<void()>> void_method;
std::shared_ptr<DBus::Method<void(struct custom)>> void_custom_method;
std::shared_ptr<DBus::Method<int(int,struct custom)>> int_custom_method2;
std::shared_ptr<DBus::Method<int(int,std::vector<struct custom>)>> int_vec_custom_method2;

int add(int a, int b){
    return a + b;
}

void void_method_symbol(){}

void void_custom_method_symbol(struct custom c){
}

int int_intcustom_symbol(int i, struct custom c){
  return i + c.first + c.second;
}

int int_int_vec_custom_symbol(int i, std::vector<struct custom> vec){
  int retval = 0;

  for( struct custom cust : vec ){
    retval += cust.first;
    retval += cust.second;
  }

  return retval;
}

void client_setup(){
    proxy = conn->create_object_proxy( "dbuscxx.test", "/test" );

    int_method_proxy = proxy->create_method<int(int,int)>( "foo.what", "add" );
    void_method_proxy = proxy->create_method<void()>( "foo.what", "void" );
    void_custom_method_proxy = proxy->create_method<void(struct custom)>( "foo.what", "void_custom" );
    int_custom_method_proxy2 = proxy->create_method<int(int,struct custom)>( "foo.what", "int_intcustom" );
    int_vec_custom_method_proxy2 = proxy->create_method<int(int,std::vector<struct custom>)>( "foo.what", "int_int_veccustom" );
}

void server_setup(){
    int ret = conn->request_name( "dbuscxx.test", DBUS_NAME_FLAG_REPLACE_EXISTING );
    if( ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER ) exit(1);

    object = conn->create_object("/test", DBus::ThreadForCalling::DispatcherThread);
    int_method = object->create_method<int(int,int)>("foo.what", "add", sigc::ptr_fun( add ) );
    void_method = object->create_method<void()>("foo.what", "void", sigc::ptr_fun( void_method_symbol ) );
    void_custom_method = object->create_method<void(struct custom)>("foo.what", "void_custom", sigc::ptr_fun( void_custom_method_symbol ) );
    int_custom_method2 = object->create_method<int(int,struct custom)>("foo.what", "int_intcustom", sigc::ptr_fun( int_intcustom_symbol ) );
    int_vec_custom_method2 = object->create_method<int(int,std::vector<struct custom>)>("foo.what", "int_int_veccustom", sigc::ptr_fun( int_int_vec_custom_symbol ) );
}

bool data_send_integers(){
    int val = (*int_method_proxy)( 2, 3 );
    
    return TEST_EQUALS( val, 5 );
}

bool data_void_method(){
    (*void_method_proxy)();

    return true;
}

bool data_void_custom(){
    struct custom c;
    c.first = 5;
    c.second = 10;
    (*void_custom_method_proxy)( c );

    return true;
}

bool data_send_intcustom(){
    struct custom c;
    c.first = 5;
    c.second = 10;
    int val = (*int_custom_method_proxy2)( 10, c );
 
    return TEST_EQUALS( val, 25 );
}

bool data_send_intveccustom(){
    struct custom c;
    c.first = 50;
    c.second = 2;

    struct custom c1;
    c1.first = 3;
    c1.second = 9;

    std::vector<struct custom> callparam;
    callparam.push_back( c );
    callparam.push_back( c1 );

    int val = (*int_vec_custom_method_proxy2)( 22, callparam );

    return TEST_EQUALS( val, 64 );
}

#define ADD_TEST(name) do{ if( test_name == STRINGIFY(name) ){ \
  ret = data_##name();\
} \
} while( 0 )

int main(int argc, char** argv){
  if(argc < 1)
    return 1;

  std::string test_name = argv[2];
  bool ret = false;
  bool is_client = std::string( argv[1] ) == "client";

  dispatch = DBus::Dispatcher::create();
  conn = dispatch->create_connection(DBus::BusType::SESSION);

  if( is_client ){
    client_setup();
    ADD_TEST(send_integers);
    ADD_TEST(void_method);
    ADD_TEST(void_custom);
    ADD_TEST(send_intcustom);
    ADD_TEST(send_intveccustom);
  }else{
    server_setup();
    ret = true;
    sleep( 1000 );
  }


  return !ret;
}

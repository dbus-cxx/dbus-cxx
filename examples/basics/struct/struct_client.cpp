/***************************************************************************
 *   Copyright (C) 2019 by Robert Middleton.                               *
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
#include <iostream>

int main(int argc, const char** argv)
{
  std::shared_ptr<DBus::Dispatcher> dispatcher = DBus::StandaloneDispatcher::create();

  std::shared_ptr<DBus::Connection> connection = dispatcher->create_connection( DBus::BusType::SESSION );

  std::shared_ptr<DBus::ObjectProxy> object = connection->create_object_proxy("dbuscxx.example.struct.server", "/dbuscxx/example/Struct");

  DBus::MethodProxy<std::tuple<int,int>()>& methodref = *(object->create_method<std::tuple<int,int>()>("Struct.Basic", "now"));

  std::tuple<int,int> value;
  value = methodref( );
  
  char format_string[200];
  
  snprintf(format_string, 200, "%d %d", std::get<0>(value), std::get<1>(value) );
  
  std::cout << "The values are: " << format_string << std::endl;

  return 0;
}

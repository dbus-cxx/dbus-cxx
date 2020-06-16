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
#include <sys/time.h>
#include <unistd.h>


std::tuple<int,int> now() {
  std::tuple<int,int> value = std::make_tuple( 5, 88 );
  return value;
}

int main()
{
  std::shared_ptr<DBus::Dispatcher> dispatcher = DBus::StandaloneDispatcher::create();

  std::shared_ptr<DBus::Connection> conn = dispatcher->create_connection(DBus::BusType::SESSION);
  
  // request a name on the bus
  if( conn->request_name( "dbuscxx.example.struct.server", DBUSCXX_NAME_FLAG_REPLACE_EXISTING ) !=
      DBus::RequestNameResponse::PrimaryOwner )
      return 1;

  std::shared_ptr<DBus::Object> object = conn->create_object("/dbuscxx/example/Struct", DBus::ThreadForCalling::DispatcherThread );

  object->create_method<std::tuple<int,int>()>("now", sigc::ptr_fun(now) );

  std::cout << "Running" << std::flush;
  
  for (int i=0; i < 20; i++)
  {
    std::cout << "." << std::flush;
    sleep(1);
  }
  
  std::cout << std::endl;

  return 0;

}

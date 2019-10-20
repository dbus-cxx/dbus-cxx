/***************************************************************************
 *   Copyright (C) 2007,2010 by Rick L. Vinyard, Jr.                       *
 *   rvinyard@cs.nmsu.edu                                                  *
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

double add( double param1, double param2 )      { return param1 + param2; }

int main()
{
  int ret;

  DBus::init();
  
  std::shared_ptr<DBus::Dispatcher> dispatcher = DBus::Dispatcher::create();

  std::shared_ptr<DBus::Connection> conn = dispatcher->create_connection(DBus::BusType::SESSION);
  
  ret = conn->request_name( "dbuscxx.quickstart_0.server", DBUS_NAME_FLAG_REPLACE_EXISTING );
  if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) return 1;

  //create an object on us
  std::shared_ptr<DBus::Object> object = conn->create_object("/dbuscxx/quickstart_0");

  //add a method that can be called over the dbus
  object->create_method<double(double,double)>("dbuscxx.Quickstart", "add", sigc::ptr_fun(add) );

  sleep(10);
  
  return 0;
}

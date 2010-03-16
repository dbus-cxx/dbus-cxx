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
#include <iostream>

int main()
{
  DBus::init();

  DBus::Dispatcher::pointer dispatcher;
  dispatcher = DBus::Dispatcher::create();

  DBus::Connection::pointer connection;
  connection = dispatcher->create_connection( DBus::BUS_SESSION );

  DBus::ObjectProxy::pointer object;
  object = connection->create_object_proxy("dbuscxx.quickstart_0.server", "/dbuscxx/quickstart_0");

  DBus::MethodProxy<double,double,double>& add_proxy
    = *(object->create_method<double,double,double>("dbuscxx.Quickstart","add"));

  double answer;
  answer = add_proxy( 1.1, 2.2 );

  std::cout << "1.1 + 2.2 = " << answer << std::endl;

  return 0;
}

// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
/***************************************************************************
 *   Copyright (C) 2008,2010 by Rick L. Vinyard, Jr.                       *
 *   rvinyard@cs.nmsu.edu                                                  *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 ***************************************************************************/
#include <dbus-cxx.h>
#include <iostream>

int main()
{
  std::shared_ptr<DBus::Dispatcher> dispatcher = DBus::StandaloneDispatcher::create();

  std::shared_ptr<DBus::Connection> connection = dispatcher->create_connection( DBus::BusType::SESSION );

  //create an object proxy, which stands in for a real object.
  //a proxy exists over the dbus
  std::shared_ptr<DBus::ObjectProxy> object = connection->create_object_proxy("dbuscxx.quickstart_0.server", "/dbuscxx/quickstart_0");

  //a method proxy acts like a real method, but will go over the dbus
  //to do its work.
  DBus::MethodProxy<double(double,double)>& add_proxy
    = *(object->create_method<double(double,double)>("dbuscxx.Quickstart","add"));

  double answer;
  // Call the method over the bus.  This blocks by default.
  answer = add_proxy( 1.1, 2.2 );

  std::cout << "1.1 + 2.2 = " << answer << std::endl;

  return 0;
}

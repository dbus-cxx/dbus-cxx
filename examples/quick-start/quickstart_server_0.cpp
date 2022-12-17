// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
/***************************************************************************
 *   Copyright (C) 2008,2010 by Rick L. Vinyard, Jr.                       *
 *   rvinyard@cs.nmsu.edu                                                  *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 ***************************************************************************/
#include <dbus-cxx.h>
#include <thread>
#include <chrono>

double add( double param1, double param2 )      { return param1 + param2; }

int main()
{
  std::shared_ptr<DBus::Dispatcher> dispatcher = DBus::StandaloneDispatcher::create();

  std::shared_ptr<DBus::Connection> conn = dispatcher->create_connection(DBus::BusType::SESSION);
  
  if( conn->request_name( "dbuscxx.quickstart_0.server", DBUSCXX_NAME_FLAG_REPLACE_EXISTING ) != DBus::RequestNameResponse::PrimaryOwner )
      return 1;

  //create an object that will contain methods that can be called
  std::shared_ptr<DBus::Object> object = conn->create_object("/dbuscxx/quickstart_0", DBus::ThreadForCalling::DispatcherThread);

  //add a method that can be called over the dbus
  object->create_method<double(double,double)>("dbuscxx.Quickstart", "add", sigc::ptr_fun(add) );

  std::this_thread::sleep_for(std::chrono::seconds(10));

  return 0;
}

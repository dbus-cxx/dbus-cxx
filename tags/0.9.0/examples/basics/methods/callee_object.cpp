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
#include <unistd.h>

/**
 * This example exports an already-existing class out onto the bus.
 * This class does not have to implement any interface in order to
 * do its work.
 */

class Test {
  public:
    Test(double d): x(d) { }
    
    double add( double param1, double param2 )      { return param1 + param2 + x; }
    double subtract( double param1, double param2 ) { return param1 - param2 - x; }
    double multiply( double param1, double param2 ) { return param1 * param2 * x; }
    double divide( double param1, double param2 )   { return param1 / param2 / x; }

    double x;
};

int main()
{
  DBus::init();
  
  int ret;

  Test test(2.0);
  
  DBus::Dispatcher::pointer dispatcher = DBus::Dispatcher::create();

  DBus::Connection::pointer conn = dispatcher->create_connection(DBus::BUS_SESSION);
  
  // request a name on the bus
  ret = conn->request_name( "dbuscxx.example.calculator.server", DBUS_NAME_FLAG_REPLACE_EXISTING );
  if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) return 1;

  DBus::Object::pointer object = conn->create_object("/dbuscxx/example/Calculator");

  //use sigc::mem_fun as this is a member function of a class
  object->create_method<double,double,double>("add", sigc::mem_fun(test, &Test::add) );
  object->create_method<double,double,double>("sub", sigc::mem_fun(test, &Test::subtract) );
  object->create_method<double,double,double>("mul", sigc::mem_fun(test, &Test::multiply) );
  object->create_method<double,double,double>("div", sigc::mem_fun(test, &Test::divide) );

  std::cout << "Running" << std::flush;
  
  for (int i=0; i < 10; i++)
  {
    std::cout << "." << std::flush;
    sleep(1);
  }
  
  std::cout << std::endl;

  return 0;

}


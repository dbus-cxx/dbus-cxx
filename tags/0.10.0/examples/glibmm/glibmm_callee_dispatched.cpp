/***************************************************************************
 *   Copyright (C) 2007 by Rick L. Vinyard, Jr.                            *
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
#include <glibmm.h>
#include <dbus-cxx-glibmm.h>

#include <iostream>
#include <cmath>

Glib::RefPtr<Glib::MainLoop> main_loop;

bool on_timeout();

class Test {
  public:
    double add( double param1, double param2 )      { return param1 + param2; }
    double subtract( double param1, double param2 ) { return param1 - param2; }
    double multiply( double param1, double param2 ) { return param1 * param2; }
    double divide( double param1, double param2 )   { return param1 / param2; }
};


DBus::HandlerResult reply_to_method_call( DBus::Connection::pointer conn, DBus::Message::const_pointer msg );

int main()
{
  Test test;
  
  DBus::init();
  
  main_loop = Glib::MainLoop::create();

  int ret;
  
  DBus::Glib::Dispatcher dispatcher;

  DBus::Connection::pointer conn = dispatcher.create_connection(DBus::BUS_SESSION);
  
  // request a name on the bus
  ret = conn->request_name( "dbuscxx.example.calculator.server", DBUS_NAME_FLAG_REPLACE_EXISTING );
  if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) return 1;
  
  DBus::Object::pointer object = conn->create_object("/dbuscxx/example/Calculator");
  object->create_method<double,double,double>("add", sigc::mem_fun(test, &Test::add) );
  object->create_method<double,double,double>("sub", sigc::mem_fun(test, &Test::subtract) );
  object->create_method<double,double,double>("mul", sigc::mem_fun(test, &Test::multiply) );
  object->create_method<double,double,double>("div", sigc::mem_fun(test, &Test::divide) );

  Glib::signal_timeout().connect( sigc::ptr_fun(&on_timeout), 1000 );
  main_loop->run();
  
  std::cout << std::endl;

  return 0;
}

bool on_timeout()
{
  static int elapsed = 0;
  elapsed++;
    
  if ( elapsed % 5 == 0 )
    std::cout << " " << elapsed << std::endl;
  else
    std::cout << "." << std::flush;
      
  if ( elapsed >= 20 and main_loop ) main_loop->quit();

  return true;
}

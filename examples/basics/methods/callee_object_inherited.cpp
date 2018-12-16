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
 * This example shows how to export a class onto the bus that extends
 * DBus::Object.  
 *
 * The main difference between this and callee_object.cpp is where the
 * calls to create_method go, and where the path is set.
 */

class Inherited: public DBus::Object
{
  protected:
    
    Inherited(double d):
      DBus::Object("/dbuscxx/example/Calculator"),
      x(d)
    {
      this->create_method<double,double,double>("add", sigc::mem_fun(*this, &Inherited::add) );
      this->create_method<double,double,double>("sub", sigc::mem_fun(*this, &Inherited::subtract) );
      this->create_method<double,double,double>("mul", sigc::mem_fun(*this, &Inherited::multiply) );
      this->create_method<double,double,double>("div", sigc::mem_fun(*this, &Inherited::divide) );
    }

  public:
    typedef DBusCxxPointer<Inherited> pointer;

    static pointer create(unsigned int x) { return pointer(new Inherited(x)); }
    
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

  DBus::Dispatcher::pointer dispatcher = DBus::Dispatcher::create();

  DBus::Connection::pointer conn = dispatcher->create_connection(DBus::BUS_SESSION);
  
  // request a name on the bus
  ret = conn->request_name( "dbuscxx.example.calculator.server", DBUS_NAME_FLAG_REPLACE_EXISTING );
  if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) return 1;

  Inherited::pointer inherited = Inherited::create(2.0);

  conn->register_object( inherited );

  std::cout << "Running" << std::flush;
  
  for (int i=0; i < 10; i++)
  {
    std::cout << "." << std::flush;
    sleep(1);
  }
  
  std::cout << std::endl;

  return 0;

}

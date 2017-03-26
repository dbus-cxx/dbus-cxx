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

#include "caller_popt.h"

/**
 * Calculator class.  As this is an ObjectProxy, we are able to
 * call methods normally, as if they were methods on a real object.
 * As with most dbus-cxx objects, we have a public typedef for a 
 * smart pointer type.  There is also a create() method which will
 * create a new proxy for us.
 */
class Calculator: public DBus::ObjectProxy
{
  protected:

    Calculator(DBus::Connection::pointer conn):
      DBus::ObjectProxy(conn, "dbuscxx.example.calculator.server", "/dbuscxx/example/Calculator")
    {
      m_method_add = this->create_method<double,double,double>("Calculator.Basic", "add");
      m_method_sub = this->create_method<double,double,double>("Calculator.Basic", "sub");
      m_method_mul = this->create_method<double,double,double>("Calculator.Basic", "mul");
      m_method_div = this->create_method<double,double,double>("Calculator.Basic", "div");
    }

  public:

    typedef DBusCxxPointer<Calculator> pointer;

    static pointer create(DBus::Connection::pointer conn)
    {
      return pointer(new Calculator(conn));
    }
    
    double add(double param1, double param2) { return (*m_method_add)(param1,param2); }

    double sub(double param1, double param2) { return (*m_method_sub)(param1,param2); }

    double mul(double param1, double param2) { return (*m_method_mul)(param1,param2); }

    double div(double param1, double param2) { return (*m_method_div)(param1,param2); }

  protected:
    
    DBus::MethodProxy<double,double,double>::pointer m_method_add;
    DBus::MethodProxy<double,double,double>::pointer m_method_sub;
    DBus::MethodProxy<double,double,double>::pointer m_method_mul;
    DBus::MethodProxy<double,double,double>::pointer m_method_div;
};

int main(int argc, const char** argv)
{
  // Useful symbols declared:
  //   op: C string containing the op to perform
  //   opsym: C char containing the mathmatical symbol of the op
  //   param1: double containing parameter 1
  //   param2: double containing parameter 2
  CALLER_PARSE_ARGS(argc, argv);

  DBus::init();

  DBus::Dispatcher::pointer dispatcher = DBus::Dispatcher::create();

  DBus::Connection::pointer connection = dispatcher->create_connection( DBus::BUS_SESSION );

  //Create the calculator from the connection
  Calculator::pointer calculator = Calculator::create(connection);

  double answer=NAN;

  //We may now call methods on this proxy as if it was a real object,
  //and not a remote object over the bus
  if      ( strcmp(op,"add") == 0 ) { answer = calculator->add(param1,param2); }
  else if ( strcmp(op,"sub") == 0 ) { answer = calculator->sub(param1,param2); }
  else if ( strcmp(op,"mul") == 0 ) { answer = calculator->mul(param1,param2); }
  else if ( strcmp(op,"div") == 0 ) { answer = calculator->div(param1,param2); }

  std::cout << param1 << " " << opsym << " " << param2 << " = " << answer << std::endl;

  return 0;
}

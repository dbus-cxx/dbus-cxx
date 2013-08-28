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

  //We need to create an object proxy in order to say that we have methods
  //on it.  An object proxy consists of at minimum a dbus name(in this case
  //dbuscxx.example.calculator.server), and a path on that
  //(/dbuscxx/example/calculator)
  DBus::ObjectProxy::pointer object = connection->create_object_proxy("dbuscxx.example.calculator.server", "/dbuscxx/example/Calculator");

  //Now, create a method on that object.  This will be the same as calling:
  //dbuscxx.example.calculator.server /dbuscxx/example/Calculator
  //     Calculator.Basic.[add|sub|mul|div] <arg1> <arg2>
  DBus::MethodProxy<double,double,double>& methodref = *(object->create_method<double,double,double>("Calculator.Basic", op));

  double answer;
  answer = methodref( param1, param2 );

  std::cout << param1 << " " << opsym << " " << param2 << " = " << answer << std::endl;

  DBus::MethodProxy<void>& methodref2 = *(object->create_method<void>("Calculator.Basic", "print"));
  methodref2();

  return 0;
}

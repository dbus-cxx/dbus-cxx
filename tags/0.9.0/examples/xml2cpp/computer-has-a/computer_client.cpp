/***************************************************************************
 *   Copyright (C) 2009,2010 by Rick L. Vinyard, Jr.                       *
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

#include "computer_proxy.h"
#include "calculator_proxy.h"

#include "client_popt.h"

int main(int argc, const char** argv)
{
  // Useful symbols declared:
  //   op: C string containing the op to perform
  //   opsym: C char containing the mathmatical symbol of the op
  //   param1: double containing parameter 1
  //   param2: double containing parameter 2
  CLIENT_PARSE_ARGS(argc, argv);

  DBus::init();

  DBus::Dispatcher::pointer dispatcher = DBus::Dispatcher::create();

  DBus::Connection::pointer connection = dispatcher->create_connection( DBus::BUS_SESSION );

  DBus::Example::ComputerProxy::pointer computer = DBus::Example::ComputerProxy::create(connection);

  DBus::Example::CalculatorProxy::pointer calculator = computer->calculator();

  double answer=NAN;
  uint64_t result;

  if      ( strcmp(op,"add")         == 0 ) { answer = calculator->add(param1,param2); std::cout << param1 << " + " << param2 << " = " << answer << std::endl; }
  else if ( strcmp(op,"sub")         == 0 ) { answer = calculator->subtract(param1,param2); std::cout << param1 << " - " << param2 << " = " << answer << std::endl; }
  else if ( strcmp(op,"mul")         == 0 ) { answer = calculator->multiply(param1,param2); std::cout << param1 << " * " << param2 << " = " << answer << std::endl; }
  else if ( strcmp(op,"div")         == 0 ) { answer = calculator->divide(param1,param2); std::cout << param1 << " / " << param2 << " = " << answer << std::endl; }
  else if ( strcmp(op,"pi")          == 0 ) { answer = calculator->pi();               std::cout << "pi = " << answer << std::endl; }
  else if ( strcmp(op,"print-pi")    == 0 ) { calculator->print_pi();                  std::cout << "printed pi on server" << std::endl; }
  else if ( strcmp(op,"factorial")   == 0 ) { result = computer->factorial(param1);  std::cout << (uint64_t)param1 << "! = " << result << std::endl; }
  else if ( strcmp(op,"fibonacci")   == 0 ) { result = computer->fibonacci(param1);  std::cout << "Fibonacci_" << (uint64_t)(param1) << " = " << result << std::endl; }
  else if ( strcmp(op,"thue-morse")  == 0 )
  {
    if ( param1 > 6.0 )
    {
      std::cout << "Thue-Morse limit is 6" << std::endl;
    }
    else
    {
      result = computer->thue_morse(param1);
      std::cout << "Thue-Morse_" << (uint64_t)(param1) << " = " << result << std::endl;
      std::cout << "Thue-Morse_" << (uint64_t)(param1) << " = ";
      uint64_t mask = 0x0001;
      std::string binary;
      for (int i = 0; i < pow(2,param1); i++)
      {
        binary += (result&mask)?'1':'0';
        result = result >> 1;
      }
      for ( std::string::reverse_iterator i = binary.rbegin(); i != binary.rend(); i++ )
        std::cout << *i;
      std::cout << std::endl;
    }
  }

  return 0;
}

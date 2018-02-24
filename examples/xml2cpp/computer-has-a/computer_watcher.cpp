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

#include <unistd.h>

void print_calculation(std::string op, std::string opsym, double result, double param1, double param2);

void print_computation(std::string op, uint64_t result, uint8_t n);

int main()
{
  DBus::init();

  DBus::Dispatcher::pointer dispatcher = DBus::Dispatcher::create();

  DBus::Connection::pointer connection = dispatcher->create_connection( DBus::BUS_SESSION );

  DBus::Example::ComputerProxy::pointer computer = DBus::Example::ComputerProxy::create(connection);

  computer->signal_computation().connect( sigc::ptr_fun(print_computation) );
  computer->calculator()->signal_calculation().connect( sigc::ptr_fun(print_calculation) );

  std::cout << "Running" << std::flush;
  
  for (int i=0; i < 30; i++)
  {
    std::cout << "." << std::flush;
    sleep(1);
  }
  
  std::cout << std::endl;

  return 0;
}

void print_calculation(std::string op, std::string opsym, double result, double param1, double param2)
{
  std::cout << param1 << " " << opsym << " " << param2 << " = " << result << std::endl;
}

void print_computation(std::string op, uint64_t result, uint8_t n)
{
  std::cout << op << "(" << (int)n << ") = " << result << std::endl;
}

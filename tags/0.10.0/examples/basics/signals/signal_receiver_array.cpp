/***************************************************************************
 *   Copyright (C) 2007,2008,2009,2010 by Rick L. Vinyard, Jr.             *
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
 * This example demonstrates a dispatched signal receiver of an array.
 */

void print( std::vector<double> array );

int main()
{
  DBus::init();

  DBus::Dispatcher::pointer dispatcher = DBus::Dispatcher::create();

  DBus::Connection::pointer connection = dispatcher->create_connection( DBus::BUS_SESSION );

  DBus::signal_proxy<void,std::vector<double> >::pointer signal = connection->create_signal_proxy<void,std::vector<double> >("/test/signal/Object", "test.signal.Type", "Test");

  signal->connect( sigc::ptr_fun(print) );

  std::cout << "Running" << std::flush;
  
  for (int i=0; i < 10; i++)
  {
    std::cout << "." << std::flush;
    sleep(1);
  }
  
  std::cout << std::endl;

  return 0;
}

void print(std::vector<double> array)
{
  std::cout << "signal-emitter-array callback: " << std::endl;
  std::cout << "\tArray size:" << array.size() << std::endl;
  for (unsigned i=0; i < array.size(); i++)
    std::cout << "\t\tElement " << i << " = " << array[i] << std::endl;
}

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
#include <unistd.h>
#include <cmath>

/**
 * This example extends the signal-emitter example emits an array
 */

void callback(std::vector<double> array);

int main()
{
  DBus::init();

  DBus::Dispatcher::pointer dispatcher = DBus::Dispatcher::create();

  DBus::Connection::pointer connection = dispatcher->create_connection( DBus::BUS_SESSION );

  DBus::signal<void,std::vector<double> >::pointer signal = connection->create_signal<void,std::vector<double> >("/test/signal/Object", "test.signal.Type", "Test");

  std::vector<double> array;
  array.push_back(M_PI);
  array.push_back(M_E);
  array.push_back(M_LN2);
  array.push_back(M_LN10);

  signal->connect( sigc::ptr_fun(callback) );

  signal->emit( array );

  usleep(250000);

  return 0;
}

void callback(std::vector<double> array)
{
  std::cout << "signal-emitter-array callback: " << std::endl;
  std::cout << "\tArray size:" << array.size() << std::endl;
  for (unsigned i=0; i < array.size(); i++)
    std::cout << "\t\tElement " << i << " = " << array[i] << std::endl;
}

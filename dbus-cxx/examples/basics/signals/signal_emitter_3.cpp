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
#include <stdio.h>
#include <unistd.h>

/**
 * This example extends the signal-emitter example and eliminates the
 * need for the connection flush() calls by using a dispatcher.
 */

void callback(std::string);

int main()
{
  DBus::init();

  DBus::Dispatcher::pointer dispatcher = DBus::Dispatcher::create();

  DBus::Connection::pointer connection = dispatcher->create_connection( DBus::BUS_SESSION );

  DBus::signal<void,std::string>::pointer signal = connection->create_signal<void,std::string>("/test/signal/Object", "test.signal.Type", "Test");

  const char* sigvalue1 = "Hello";
  std::string sigvalue2("World");

  signal->connect( sigc::ptr_fun(callback) );

  signal->emit( sigvalue1 );

  signal->emit( sigvalue2 );

  usleep(250000);

  return 0;
}

void callback(std::string s)
{
  std::cout << "signal-emitter-2 callback: " << s << std::endl;
}

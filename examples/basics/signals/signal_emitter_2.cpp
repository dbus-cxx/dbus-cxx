/***************************************************************************
 *   Copyright (C) 2007,2008,2009 by Rick L. Vinyard, Jr.                  *
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
 * This example extends the signal-emitter example and demonstrates
 * the ability to use dbus-cxx signals for local callbacks as well.
 *
 * When the signal is emitted a dbus signal message is transmitted
 * on the connection as well as calling the connected callback()
 * function.
 */

void callback(std::string);

int main()
{
  DBus::init();
  
  DBus::Connection::pointer connection = DBus::Connection::create( DBus::BUS_SESSION );

  DBus::signal<void,std::string>::pointer signal = connection->create_signal<void,std::string>("/test/signal/Object", "test.signal.Type", "Test");

  const char* sigvalue1 = "Hello";
  std::string sigvalue2("World");

  signal->connect( sigc::ptr_fun(callback) );

  signal->emit( sigvalue1 );
  connection->flush();

  signal->emit( sigvalue2 );
  connection->flush();

  usleep(250000);
}

void callback(std::string s)
{
  std::cout << "signal-emitter-2 callback: " << s << std::endl;
}

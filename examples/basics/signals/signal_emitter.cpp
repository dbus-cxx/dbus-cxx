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
 * This example demonstrates a signal emitter using a dbus-cxx signal
 * derived from sigc::signal.
 *
 * The message of the signal is created when the signal is emitted from
 * the emission parameters.
 *
 * However, the connection must still be flushed since a dispatcher is
 * not being used.
 */

int main()
{
  std::shared_ptr<DBus::Connection> connection = DBus::Connection::create( DBus::BusType::SESSION );

  std::shared_ptr<DBus::Signal<void(std::string)>> signal = connection->create_free_signal<void(std::string)>("/test/signal/Object", "test.signal.Type", "Test");

  const char* sigvalue1 = "Hello";
  std::string sigvalue2("World");

  signal->emit( sigvalue1 );
  connection->flush();

  signal->emit( sigvalue2 );
  connection->flush();

  usleep(250000);
}

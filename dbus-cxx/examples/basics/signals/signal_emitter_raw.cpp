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
 * This example demonstrates a signal emitter that uses raw dbus-cxx bus
 * calls without a dispatcher. This is a similar approach to the
 * signal-emitter-c example.
 */

using namespace DBus;
    
int main()
{
  DBus::init();

  Connection::pointer connection = DBus::Connection::create( DBus::BUS_SESSION );

  SignalMessage::pointer message;
  message = SignalMessage::create( "/test/signal/Object",  // object name of the signal
                                   "test.signal.Type",     // interface name of the signal
                                   "Test" );               // name of the signal

  const char* sigvalue1 = "Hello World";

  // append arguments onto signal
  message << sigvalue1;

  // send the message and flush the connection
  connection << message;
  connection->flush();

  usleep(250000);
}

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
 * This example demonstrates a dispatched signal receiver. Once started
 * the main loop spends most of it's time sleeping and dispatching occurs
 * in the dispatch threads.
 *
 * This example connects the print function to the connection's incoming
 * dbus signal and extracts the string from the message.
 */

DBus::HandlerResult print( DBus::SignalMessage::const_pointer );

int main()
{
  DBus::init();

  DBus::Dispatcher::pointer dispatcher = DBus::Dispatcher::create();

  DBus::Connection::pointer connection = dispatcher->create_connection( DBus::BUS_SESSION );

  DBus::signal_proxy_simple::pointer signal = connection->create_signal_proxy("/test/signal/Object", "test.signal.Type", "Test");

  signal->signal_dbus_incoming().connect( sigc::ptr_fun(print) );

  std::cout << "Running" << std::flush;
  
  for (int i=0; i < 10; i++)
  {
    std::cout << "." << std::flush;
    sleep(1);
  }
  
  std::cout << std::endl;

  return 0;
}

DBus::HandlerResult print( DBus::SignalMessage::const_pointer msg)
{
  std::string val;
  msg >> val;
  std::cout << "Got Signal with value " << val << std::endl;
  return DBus::HANDLED;
}

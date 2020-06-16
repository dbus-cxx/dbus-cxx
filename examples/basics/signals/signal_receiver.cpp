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

void print( std::string str );

int main()
{
  std::shared_ptr<DBus::Dispatcher> dispatcher = DBus::StandaloneDispatcher::create();

  std::shared_ptr<DBus::Connection> connection = dispatcher->create_connection( DBus::BusType::SESSION );

  std::shared_ptr<DBus::signal_proxy<std::string>> signal = 
      connection->create_signal_proxy<std::string>(
          DBus::SignalMatchRule::create()
              .setPath("/test/signal/Object")
              .setInterface("test.signal.Type")
              .setMember("Test"),
      DBus::ThreadForCalling::DispatcherThread );

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

void print( std::string str )
{
  std::cout << "Got Signal with value " << str << std::endl;
}

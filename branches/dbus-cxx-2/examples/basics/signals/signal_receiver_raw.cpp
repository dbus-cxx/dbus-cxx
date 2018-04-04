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
#include <iostream>
#include <unistd.h>

/**
 * This example demonstrates a signal receiver that uses raw dbus-cxx bus
 * calls without a dispatcher. This is a similar approach to the
 * signal-receiver-c example.
 */

int main()
{
  DBus::init();
  
  DBus::Connection::pointer connection = DBus::Connection::create( DBus::BUS_SESSION );
  DBus::Message::pointer msg;
  std::string val;

  // add a rule for which messages we want to see
  connection->add_match( "type='signal',interface='test.signal.Type'" ); // see signals from the given interface
  connection->flush();

  // loop listening for signals being emmitted
  while ( true ) {

    // non blocking read of the next available message
    connection->read_write( 0 );
    msg = connection->pop_message();

    // loop again if we haven't read a message
    if ( not msg or not msg->is_valid() ) {
      sleep( 1 );
      continue;
    }

    // check if the message is a signal from the correct interface
    // and with the correct name
    if ( msg->is_signal( "test.signal.Type", "Test" ) ) {
      // read the parameters
      msg >> val;
      std::cout << "Got Signal with value " << val << std::endl;
    }
  }

  return 0;
}

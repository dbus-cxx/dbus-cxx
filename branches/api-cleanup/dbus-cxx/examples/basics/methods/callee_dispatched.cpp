/***************************************************************************
 *   Copyright (C) 2007,2010 by Rick L. Vinyard, Jr.                       *
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
#include <cmath>
#include <unistd.h>

/**
 * This example uses the low-level dbus-cxx methods(but not the low-level
 * C bindings).  This is normally not something that you want to do.
 */

DBus::HandlerResult reply_to_method_call( DBus::Connection::pointer conn, DBus::Message::const_pointer msg );

int main()
{
  DBus::init();
  
  int ret;
  
  DBus::Dispatcher::pointer dispatcher = DBus::Dispatcher::create();

  DBus::Connection::pointer conn = dispatcher->create_connection(DBus::BUS_SESSION);
  
  // request a name on the bus
  ret = conn->request_name( "dbuscxx.example.calculator.server", DBUS_NAME_FLAG_REPLACE_EXISTING );
  if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) return 1;
  
  conn->create_object("/dbuscxx/example/Calculator", reply_to_method_call);

  std::cout << "Running" << std::flush;
  
  for (int i=0; i < 10; i++)
  {
    std::cout << "." << std::flush;
    sleep(1);
  }
  
  std::cout << std::endl;

  return 0;
}

DBus::HandlerResult reply_to_method_call( DBus::Connection::pointer conn, DBus::Message::const_pointer msg )
{
  DBus::ReturnMessage::pointer reply;
  double param1, param2, answer=NAN;

  // read the arguments
  msg >> param1 >> param2;
  std::cout << "Method called with " << param1 << ", " << param2 << std::endl;

  // figure out what the actual method called was
  if      ( msg->is_call( "Calculator.Basic", "add" ) ) answer = param1 + param2;
  else if ( msg->is_call( "Calculator.Basic", "sub" ) ) answer = param1 - param2;
  else if ( msg->is_call( "Calculator.Basic", "mul" ) ) answer = param1 * param2;
  else if ( msg->is_call( "Calculator.Basic", "div" ) ) answer = param1 / param2;

  if ( not isnan(answer) )
  {
    // create a reply from the message
    reply = msg->create_reply( );

    // Was the reply created successfully
    if ( not reply ) return DBus::NOT_HANDLED;
    
    // add the arguments to the reply
    reply << answer;

    // send the reply
    conn << reply;
  }

  return DBus::HANDLED;
}

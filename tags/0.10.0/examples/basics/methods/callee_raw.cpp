/***************************************************************************
 *   Copyright (C) 2007 by Rick L. Vinyard, Jr.                            *
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

void reply_to_method_call( DBus::CallMessage::pointer msg, DBus::Connection::pointer conn );

int main()
{
  DBus::init();
  
  DBus::Message::pointer msg;
  int ret;
  DBus::Connection::pointer conn = DBus::Connection::create(DBus::BUS_SESSION);

  // request a name on the bus
  ret = conn->request_name( "dbuscxx.example.calculator.server", DBUS_NAME_FLAG_REPLACE_EXISTING );
  if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) return 1;

  // loop, testing for new messages
  while ( true ) {
    // non blocking read of the next available message
    conn->read_write( 0 );
    msg = conn->pop_message( );

    // loop again if we haven't got a message
    if ( not msg ) {
      sleep( 1 );
      continue;
    }

    // check this is a method call for the right interface and method
    if ( msg->type() == DBus::CALL_MESSAGE ) reply_to_method_call( DBus::CallMessage::create(msg), conn );
  }
}

void reply_to_method_call( DBus::CallMessage::pointer msg, DBus::Connection::pointer conn )
{
  DBus::ReturnMessage::pointer reply;
  double param1, param2, answer=NAN;

  if ( not msg->has_interface("Calculator.Basic") ) return;

  // read the arguments
  if ( msg->begin() == msg->end() )
    std::cerr << "Message has no arguments!" << std::endl;
  else if ( msg->begin().arg_type() != DBus::TYPE_DOUBLE )
    std::cerr << "Argument is not double!" << std::endl;
  else
    msg >> param1 >> param2;
  std::cout << "Method called with parameters " << param1 << ", " << param2 << std::endl;

  if      ( msg->is_call("Calculator.Basic", "add") ) answer = param1 + param2;
  else if ( msg->is_call("Calculator.Basic", "sub") ) answer = param1 - param2;
  else if ( msg->is_call("Calculator.Basic", "mul") ) answer = param1 * param2;
  else if ( msg->is_call("Calculator.Basic", "div") ) answer = param1 / param2;

  if ( not isnan(answer) )
  {
    // create a reply from the message
    reply = msg->create_reply( );

    // Was the reply successfully created
    if ( not reply ) return;
    
    // add the arguments to the reply
    reply << answer;

    // send the reply && flush the connection
    conn << reply;
    conn->flush();
  }
}

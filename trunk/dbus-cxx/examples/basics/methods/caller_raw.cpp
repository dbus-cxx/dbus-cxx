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

#include <string.h>
#include <math.h>
#include <popt.h>

int main(int argc, const char** argv)
{
  // Parse command line options
  // This is boilerplate for all the caller-* examples
  poptContext opt_context;
  char* op = NULL;
  double param1=NAN, param2=NAN;

  struct poptOption option_table[] = {
    { "param1", '1', POPT_ARG_DOUBLE,   &param1, '1', "First parameter [default=3.3]" },
    { "param2", '2', POPT_ARG_DOUBLE,   &param2, '2', "Second parameter [default=4.4]" },
    { "op",    'o', POPT_ARG_STRING, &op,        'o', "The operation to execute (add, sub, mul, div) [default=add] (case sensitive)" },
    POPT_AUTOHELP
    { NULL, 0, 0, NULL, 0 }
  };

  opt_context = poptGetContext(NULL, argc, argv, option_table, 0 );

  while ( (poptGetNextOpt(opt_context)) >= 0) { }

  if ( op == NULL ) op = strdup("add");

  if ( isnan(param1) ) param1 = 3.3;

  if ( isnan(param2) ) param2 = 4.4;

  if ( not (strcmp(op,"add")==0 or strcmp(op,"sub")==0 or strcmp(op,"mul")==0 or strcmp(op,"div")==0) ) {
    fprintf( stderr, "ERROR: Operation '%s' is invalid.\n\nOperation is case sensitive and must be one of:\n\tadd\n\tsub\n\tmul\n\tdiv\n", op );
    return 1;
  }

  char opsym;

  switch(op[0])
  {
    case 'a': opsym = '+'; break;
    case 's': opsym = '-'; break;
    case 'm': opsym = '*'; break;
    case 'd': opsym = '/'; break;
  }

  // Resume example code

  DBus::init();
  
  DBus::PendingCall::pointer pending;
  DBus::Message::pointer retmsg;

  DBus::Connection::pointer connection = DBus::Connection::create( DBus::BUS_SESSION );
  
  DBus::CallMessage::pointer msg = DBus::CallMessage::create("dbuscxx.example.calculator.server", // target for the method call
                                                             "/dbuscxx/example/Calculator",       // object to call on
                                                             "Calculator.Basic",                  // interface to call on
                                                             op );                                // method name

  msg << param1 << param2;
  
  pending = connection->send_with_reply_async(msg);

  connection->flush();

  pending->block();

  DBus::Message::iterator iter;
  retmsg = pending->steal_reply();

  if ( retmsg->type() == DBus::ERROR_MESSAGE )
  {
    DBus::ErrorMessage::pointer errormsg = DBus::ErrorMessage::create(retmsg);
    std::cout << "Oops... got an error: " << errormsg->name() << std::endl;
    return 1;
  }

  double answer;
  retmsg >> answer;
  std::cout << param1 << " " << opsym << " " << param2 << " = " << answer << std::endl;

  return 0;
}

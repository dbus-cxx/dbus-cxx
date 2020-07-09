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
#include <cmath>
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

  if ( std::isnan(param1) ) param1 = 3.3;

  if ( std::isnan(param2) ) param2 = 4.4;

  if ( ! (strcmp(op,"add")==0 || strcmp(op,"sub")==0 || strcmp(op,"mul")==0 || strcmp(op,"div")==0) ) {
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

  std::shared_ptr<DBus::Message> retmsg;

  std::shared_ptr<DBus::Connection> connection = DBus::Connection::create( DBus::BusType::SESSION );

  std::shared_ptr<DBus::ObjectProxy> object = connection->create_object_proxy("dbuscxx.example.calculator.server", "/dbuscxx/example/Calculator");

  /* 
   * Create a non-templated method proxy to represent a remote method.  Since this is not templated,
   * we can't use operator() to call it.
   */
  std::shared_ptr<DBus::MethodProxyBase> method = DBus::MethodProxyBase::create(op);

  object->add_method( "Calculator.Basic", method );

  /*
   * Create a CallMessage that we will use to send a remote method call
   */
  std::shared_ptr<DBus::CallMessage> msg = method->create_call_message();
  
  /*
   * Append the parameters to the CallMessage.
   */
  msg << param1 << param2;

  try{
      /*
       * Call the remote method.  If a return comes back, the response will be in the ReturnMessage.
       * If there is an error, an exception will be thrown.
       */
      std::shared_ptr<const DBus::ReturnMessage> retmsg = method->call(msg);

      double answer;
      retmsg >> answer;

      std::cout << param1 << " " << opsym << " " << param2 << " = " << answer << std::endl;
  }catch( DBus::Error err ){
      std::cout << "Oops... got an error: " << err.name() << std::endl;
      return 1;
  }

  return 0;
}

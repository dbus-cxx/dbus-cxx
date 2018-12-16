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
#include <dbus/dbus.h>
#include <stdio.h>
#include <unistd.h>

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

  DBusError err;
  DBusConnection* conn;
  int ret;
  // initialise the errors
  dbus_error_init( &err );

  // connect to the bus
  conn = dbus_bus_get( DBUS_BUS_SESSION, &err );
  if ( dbus_error_is_set( &err ) ) {
    fprintf( stderr, "Connection Error (%s)\n", err.message );
    dbus_error_free( &err );
  }
  if ( NULL == conn ) {
    return( 1 );
  }

  DBusMessage * msg;
  DBusMessageIter args;
  DBusPendingCall* pending;

  msg = dbus_message_new_method_call( "dbuscxx.example.calculator.server", // target for the method call
                                      "/dbuscxx/example/Calculator",       // object to call on
                                      "Calculator.Basic",                  // interface to call on
                                      op ); // method name
  if ( NULL == msg ) {
    fprintf( stderr, "Message Null\n" );
    return ( 1 );
  }

  // append arguments
  dbus_message_iter_init_append( msg, &args );
  if ( !dbus_message_iter_append_basic( &args, DBUS_TYPE_DOUBLE, &param1 ) ) {
    fprintf( stderr, "Out Of Memory!\n" );
    return ( 1 );
  }
  if ( !dbus_message_iter_append_basic( &args, DBUS_TYPE_DOUBLE, &param2 ) ) {
    fprintf( stderr, "Out Of Memory!\n" );
    return ( 1 );
  }

  // send message and get a handle for a reply
  if ( !dbus_connection_send_with_reply ( conn, msg, &pending, -1 ) ) {
    // -1 is default timeout
    fprintf( stderr, "Out Of Memory!\n" );
    return ( 1 );
  }
  if ( NULL == pending ) {
    fprintf( stderr, "Pending Call Null\n" );
    return ( 1 );
  }
  dbus_connection_flush( conn );

  // free message
  dbus_message_unref( msg );

  // block until we receive a reply
  dbus_pending_call_block( pending );

  // get the reply message
  msg = dbus_pending_call_steal_reply( pending );
  if ( NULL == msg ) {
    fprintf( stderr, "Reply Null\n" );
    return ( 1 );
  }
  // free the pending message handle
  dbus_pending_call_unref( pending );

  double answer;

  // read the parameters
  if ( !dbus_message_iter_init( msg, &args ) )
    fprintf( stderr, "Message has no arguments!\n" );
  else if ( DBUS_TYPE_DOUBLE != dbus_message_iter_get_arg_type( &args ) )
    fprintf( stderr, "Argument is not double!\n" );
  else
    dbus_message_iter_get_basic( &args, &answer );

  printf("%f %c %f = %f\n", param1, opsym, param2, answer);

  // free reply and close connection
  dbus_message_unref( msg );

}

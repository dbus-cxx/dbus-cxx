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
#include <dbus/dbus.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

void reply_to_method_call( DBusMessage* msg, DBusConnection* conn );

int main()
{
  DBusError err;
  DBusConnection* conn;
  DBusMessage* msg;
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
    _exit( 1 );
  }

  // request a name on the bus
  ret = dbus_bus_request_name(conn, "dbuscxx.example.calculator.server",
                              DBUS_NAME_FLAG_REPLACE_EXISTING
                                  , &err);
  if (dbus_error_is_set(&err)) {
    fprintf(stderr, "Name Error (%s)\n", err.message);
    dbus_error_free(&err);
  }
  if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) {
    _exit(1);
  }
  
  // loop, testing for new messages
  while ( true ) {
    // non blocking read of the next available message
    dbus_connection_read_write( conn, 0 );
    msg = dbus_connection_pop_message( conn );

    // loop again if we haven't got a message
    if ( NULL == msg ) {
      sleep( 1 );
      continue;
    }

    if ( dbus_message_has_interface(msg, "Calculator.Basic") )
    reply_to_method_call( msg, conn );

    // free the message
    dbus_message_unref( msg );
  }

}

void reply_to_method_call( DBusMessage* msg, DBusConnection* conn )
{
  DBusMessage * reply;
  DBusMessageIter args;
  double param1=NAN, param2=NAN, answer=NAN;
  dbus_uint32_t serial = 0;

  // read the arguments
  if ( !dbus_message_iter_init( msg, &args ) )
    fprintf( stderr, "Message has no arguments!\n" );
  if ( DBUS_TYPE_DOUBLE != dbus_message_iter_get_arg_type( &args ) )
    fprintf( stderr, "First argument is not double!\n" );
  else
    dbus_message_iter_get_basic( &args, &param1 );
  dbus_message_iter_next(&args);
  if ( DBUS_TYPE_DOUBLE != dbus_message_iter_get_arg_type( &args ) )
    fprintf( stderr, "Second argument is not double!\n" );
  else
    dbus_message_iter_get_basic( &args, &param2 );
  printf( "Method called with %f and %f\n", param1, param2 );

  if      ( dbus_message_is_method_call( msg, "Calculator.Basic", "add" ) ) answer = param1 + param2;
  else if ( dbus_message_is_method_call( msg, "Calculator.Basic", "sub" ) ) answer = param1 - param2;
  else if ( dbus_message_is_method_call( msg, "Calculator.Basic", "mul" ) ) answer = param1 * param2;
  else if ( dbus_message_is_method_call( msg, "Calculator.Basic", "div" ) ) answer = param1 / param2;

  if ( not isnan(answer) )
  {

    // create a reply from the message
    reply = dbus_message_new_method_return( msg );

    // add the arguments to the reply
    dbus_message_iter_init_append( reply, &args );
    if ( !dbus_message_iter_append_basic( &args, DBUS_TYPE_DOUBLE, &answer ) ) {
      fprintf( stderr, "Out Of Memory!\n" );
      _exit( 1 );
    }

    // send the reply && flush the connection
    if ( !dbus_connection_send( conn, reply, &serial ) ) {
      fprintf( stderr, "Out Of Memory!\n" );
      _exit( 1 );
    }
    dbus_connection_flush( conn );

    // free the reply
    dbus_message_unref( reply );
  }

}

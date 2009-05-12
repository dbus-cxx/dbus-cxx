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

/**
 * This is an example from the dbus library and demonstrates a C
 * based signal receiver.
 */

int main()
{
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

  char* sigvalue;
  DBusMessage * msg;
  DBusMessageIter args;
  
  // add a rule for which messages we want to see
  dbus_bus_add_match( conn,
                      "type='signal',interface='test.signal.Type'",
                      &err ); // see signals from the given interface
  dbus_connection_flush( conn );
  if ( dbus_error_is_set( &err ) ) {
    fprintf( stderr, "Match Error (%s)\n", err.message );
    return( 1 );
  }
  // loop listening for signals being emmitted
  while ( true ) {

    // non blocking read of the next available message
    dbus_connection_read_write( conn, 0 );
    msg = dbus_connection_pop_message( conn );

    // loop again if we haven't read a message
    if ( NULL == msg ) {
      sleep( 1 );
      continue;
    }

    // check if the message is a signal from the correct interface
    // and with the correct name
    if ( dbus_message_is_signal( msg, "test.signal.Type", "Test" ) ) {
      // read the parameters
      if ( !dbus_message_iter_init( msg, &args ) )
        fprintf( stderr, "Message has no arguments!\n" );
      else if ( DBUS_TYPE_STRING != dbus_message_iter_get_arg_type( &args ) )
        fprintf( stderr, "Argument is not string!\n" );
      else {
        dbus_message_iter_get_basic( &args, &sigvalue );
        printf( "Got Signal with value %s\n", sigvalue );
      }
    }

    // free the message
    dbus_message_unref( msg );
  }
}


/*
 * Example low-level D-Bus code.
 * Written by Matthew Johnson <dbus@matthew.ath.cx>
 *
 * This code has been released into the Public Domain.
 * You may do whatever you like with it.
 */
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
    return ( 1 );
  }

  dbus_uint32_t serial = 0; // unique number to associate replies with requests
  DBusMessage* msg;
  DBusMessageIter args;

  // create a signal and check for errors
  msg = dbus_message_new_signal( "/test/signal/Object",   // object name of the signal
                                 "test.signal.Type",   // interface name of the signal
                                 "Test" ); // name of the signal
  if ( NULL == msg ) {
    fprintf( stderr, "Message Null\n" );
    return ( 1 );
  }

  const char* sigvalue = "Hello World";

  // append arguments onto signal
  dbus_message_iter_init_append( msg, &args );
  if ( !dbus_message_iter_append_basic( &args, DBUS_TYPE_STRING, &sigvalue ) ) {
    fprintf( stderr, "Out Of Memory!\n" );
    return ( 1 );
  }

  // send the message and flush the connection
  if ( !dbus_connection_send( conn, msg, &serial ) ) {
    fprintf( stderr, "Out Of Memory!\n" );
    return ( 1 );
  }
  dbus_connection_flush( conn );

  // free the message
  dbus_message_unref( msg );

  usleep(250000);

}

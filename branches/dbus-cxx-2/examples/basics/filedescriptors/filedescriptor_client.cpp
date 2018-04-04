/***************************************************************************
 *   Copyright (C) 2007,2010 by Rick L. Vinyard, Jr.                       *
 *   rvinyard@cs.nmsu.edu                                                  *
 *   Copyright (C) 2017 by Robert Middleton                                *
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
#include <errno.h>
#include <string.h>
#include <time.h>

static char datebuffer[ 128 ];

void mylog( const char* logger_name, const struct SL_LogLocation* location,
      const enum SL_LogLevel level,
      const char* log_string ){
    time_t now = time( NULL );
    struct tm* timeinfo;
 
    timeinfo = localtime( &now );
    strftime( datebuffer, 128, "%H:%M:%S", timeinfo );
    std::cerr << "[" << datebuffer << "] " 
        <<  logger_name << " - " << log_string << std::endl;
}

int main( int argc, char** argv ){
  /* This program gets a file descriptor from the server, and writes a 10 
   * byte string to it.
   */

  //uncomment the following line to enable logging from the library.
  //DBus::setLoggingFunction( mylog );

  DBus::init();
  DBus::Dispatcher::pointer dispatcher = DBus::Dispatcher::create();
  DBus::Connection::pointer conn = dispatcher->create_connection(DBus::BUS_SESSION);
  DBus::ObjectProxy::pointer object = conn->create_object_proxy("dbuscxx.example.filedescriptor.server", "/dbuscxx/example/FileDescriptor");

  DBus::MethodProxy<DBus::FileDescriptor::pointer>& methodref = *(object->create_method<DBus::FileDescriptor::pointer>("Filedescriptor.basic", "getFiledescriptor"));

  std::cout << "Running..." << std::flush;

  DBus::FileDescriptor::pointer fd;
  fd = methodref();
  
  if( write( fd->getDescriptor(), "A String 0", 10 ) < 0 ){
    std::cerr << "Error writing to file descriptor: " << strerror( errno ) << std::endl;
    return 1;
  }
  
  std::cout << "Done!" << std::endl;
 
  return 0;
}

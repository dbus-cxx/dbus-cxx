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
#include <poll.h>

static int pipes[2];
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

std::shared_ptr<DBus::FileDescriptor> getFiledescriptor(){
  return DBus::FileDescriptor::create( pipes[1] );
}

int main( int argc, char** argv ){
  /* This program creates a pipe, and sends one end of the pipe to 
   * the client that requests it.  It then reads 10 bytes and exits
   */
  if( pipe( pipes ) < 0 ){
      std::cerr << "Unable to create pipes: " << strerror( errno ) << std::endl;
  }

  //uncomment the following line to enable logging from the library.
  //DBus::setLoggingFunction( mylog );

  std::shared_ptr<DBus::Dispatcher> dispatcher = DBus::StandaloneDispatcher::create();
  std::shared_ptr<DBus::Connection> conn = dispatcher->create_connection(DBus::BusType::SESSION);

  // request a name on the bus
  if( conn->request_name( "dbuscxx.example.filedescriptor.server", DBUSCXX_NAME_FLAG_REPLACE_EXISTING ) !=
      DBus::RequestNameResponse::PrimaryOwner )
      return 1;

  std::shared_ptr<DBus::Object> object = conn->create_object("/dbuscxx/example/FileDescriptor", DBus::ThreadForCalling::DispatcherThread);

  object->create_method<std::shared_ptr<DBus::FileDescriptor>()>("Filedescriptor.basic", "getFiledescriptor", sigc::ptr_fun(getFiledescriptor) );

  std::cout << "Running..." << std::flush;
  
  {
    struct pollfd poll_fd;
    poll_fd.fd = pipes[0];
    poll_fd.events = POLLIN;

    if( poll( &poll_fd, 1, 1000 * 20 ) < 0 ){
      std::cerr << "Unable to poll: " << strerror( errno ) << std::endl;
    }else{
        char buffer[ 11 ];
        buffer[ 10 ] = 0;
        int got = read( pipes[ 0 ], buffer, 10 );
        std::cout << "Got " << got << " bytes.  As string: " << buffer << std::endl;
    }
  }
  
  std::cout << std::endl;


}

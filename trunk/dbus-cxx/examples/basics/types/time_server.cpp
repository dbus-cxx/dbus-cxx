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
#include <sys/time.h>

#include "timeval_dbus.h"

struct timeval now() {
  struct timeval current;
  if ( gettimeofday(&current, NULL) != 0 )
  {
    current.tv_sec = 0;
    current.tv_usec = 0;
  }
  return current;
}

int main()
{
  DBus::init();
  
  int ret;
  
  DBus::Dispatcher dispatcher;

  DBus::Connection::pointer conn = dispatcher.create_connection(DBus::BUS_SESSION);
  
  // request a name on the bus
  ret = conn->request_name( "dbuscxx.example.time.server", DBUS_NAME_FLAG_REPLACE_EXISTING );
  if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) return 1;

  DBus::Object::pointer object = conn->create_object("/dbuscxx/example/Time");

  object->create_method<struct timeval>("now", sigc::ptr_fun(now) );

  dispatcher.start();
  
  std::cout << "Running" << std::flush;
  
  for (int i=0; i < 20; i++)
  {
    std::cout << "." << std::flush;
    sleep(1);
  }
  
  dispatcher.stop();

  std::cout << std::endl;

  return 0;

}

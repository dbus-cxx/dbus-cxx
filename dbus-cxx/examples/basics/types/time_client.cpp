/***************************************************************************
 *   Copyright (C) 2007,2010 by Rick L. Vinyard, Jr.                       *
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

#include "timeval_dbus.h"


int main(int argc, const char** argv)
{

  DBus::init();

  DBus::Dispatcher::pointer dispatcher = DBus::Dispatcher::create();

  DBus::Connection::pointer connection = dispatcher->create_connection( DBus::BUS_SESSION );

  DBus::ObjectProxy::pointer object = connection->create_object_proxy("dbuscxx.example.time.server", "/dbuscxx/example/Time");

  DBus::MethodProxy<struct timeval>& methodref = *(object->create_method<struct timeval>("Time.Basic", "now"));

  struct timeval current;
  current = methodref( );
  
  struct tm tm_current;
  time_t tcurrent = current.tv_sec;
  localtime_r( &tcurrent, &tm_current );
  char format_string[200];
  char formatted_time[200];
  
  snprintf(format_string, 200, "%%a, %%d %%b %%Y %%H:%%M:%%S.%09d %%z", (int)(current.tv_usec));
  strftime( formatted_time, 200, format_string, &tm_current );
  
  std::cout << "The current time is: " << formatted_time << std::endl;

  return 0;
}

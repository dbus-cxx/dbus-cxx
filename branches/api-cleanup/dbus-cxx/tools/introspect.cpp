/***************************************************************************
 *   Copyright (C) 2007 by Rick L. Vinyard, Jr.                            *
 *   rvinyard@cs.nmsu.edu                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation version 2.1.                *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

#include <iostream>

#include <popt.h>

#include <dbus-cxx.h>

using namespace DBus;

int main( int argc, const char** argv )
{
  poptContext opt_context;
  int use_system=0;
  int use_session=0;
  const char* dest=NULL;
  const char* path=NULL;
  char c;

  struct poptOption option_table[] = {
    { "system",  0, POPT_ARG_NONE,   &use_system,  'y', "Use the system bus for introspection" },
    { "session", 0, POPT_ARG_NONE,   &use_session, 's', "Use the session bus for introspection [default]" },
    { "dest",    0, POPT_ARG_STRING, &dest,        'd', "The name of the connection to perform the introspection on" },
    POPT_AUTOHELP
    { NULL, 0, 0, NULL, 0 }
  };

  opt_context = poptGetContext(NULL, argc, argv, option_table, 0 );
  poptSetOtherOptionHelp( opt_context, "<object path to introspect>");

  if ( argc < 2) {
    poptPrintUsage(opt_context, stderr, 0);
    return 1;
  }

  while ( (c = poptGetNextOpt(opt_context)) >= 0) { }

  path = poptGetArg( opt_context );

  if ( (path == NULL) || !(poptPeekArg(opt_context) == NULL))
  {
    poptPrintUsage(opt_context, stderr, 0);
    std::cerr << "You must specify a DBus object path: e.g., /org/freedesktop/Hal/Manager" << std::endl;
    return 1;
  }

  DBus::init();
  
  Connection::pointer conn;

  if ( use_system )
    conn = Connection::create(BUS_SYSTEM);
  else
    conn = Connection::create(BUS_SESSION);

  std::string introspection = conn->introspect( dest, path );

  std::cout << introspection << std::endl;

  return 0;
}

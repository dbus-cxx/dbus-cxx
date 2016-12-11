/***************************************************************************
 *   Copyright (C) 2009 by Rick L. Vinyard, Jr.                            *
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
#ifndef DBUS_CXX_SIGNAL_H
#define DBUS_CXX_SIGNAL_H

#include "arg.h"

class Interface;

class DBusSignal {
public:
  DBusSignal():interface(NULL), ignored(0) { }

  std::string strfmt(int depth=0);

  std::string varname();
  std::string adapter_name();
  std::string adapter_conn_name(); 
  std::string cpp_adapter_create();
  std::vector<std::string> proxy_arg_names();
  std::vector<std::string> adapter_arg_names();
  std::string cpp_proxy_accessor();
  std::string cpp_declare_proxy();
  std::string adapter_signal_create();
  std::string adapter_signal_declare();
  std::string adapter_signal_conn_declare();
  std::string adapter_signal_connect();
  std::string adapter_signal_disconnect(); 
  
  bool args_valid();

  std::map<std::string,std::string> iterator_support();

  void set_interface( Interface* );

  int get_ignored();
  void set_name( std::string );
  void set_ignored( int ignore );
  void set_accessor( std::string );
  void push_arg( Arg );

private:
  Interface* interface;
  std::string name;
  std::vector<Arg> args;
  std::string accessor;
  int ignored;
};

#endif

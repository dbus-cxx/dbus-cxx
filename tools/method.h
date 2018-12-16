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

#ifndef METHOD_H
#define METHOD_H

#include <string>
#include <map>

#include "arg.h"

class Interface;

/**
 * Represents a DBus method.  Contains arguments.
 */
class Method {

public:
  Method():interface(NULL), mis_const(false), mis_virtual(false) { }

  std::string strfmt(int depth=0);

  std::vector<std::string> adapter_arg_names();

  std::string get_name();

  std::string stubsignature();
  std::string stubname() { return name + "_adapter_stub_" + stubsignature(); }
  std::string varname()  { return "m_method_" + name + "_" + stubsignature(); }
  std::string cpp_adapter_create();
  std::string cpp_proxy_method();
  std::string cpp_declare_proxy();
  std::string cpp_adapter_creation();
  std::string cpp_adapter_stub();
  bool args_valid();

  std::map<std::string,std::string> iterator_support();

  void set_interface( Interface* interface );
  void set_name( std::string );
  void set_cppname( std::string );
  void set_const( bool );
  void set_virtual( bool );

  bool is_const();
  bool is_virtual();

  void push_in_arg( Arg );
  void push_out_arg( Arg );

private:
  Interface* interface;
  std::string name;
  std::string cppname;
  bool mis_const;
  bool mis_virtual;

  std::vector<Arg> in_args;
  std::vector<Arg> out_args;
};

#endif

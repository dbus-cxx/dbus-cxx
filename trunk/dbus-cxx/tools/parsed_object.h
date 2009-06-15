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
#ifndef PARSED_OBJECT_H
#define PARSED_OBJECT_H

#include <cstring>
#include <string>
#include <vector>
#include <sstream>

#include <dbus-cxx.h>

typedef enum Direction {
  DIRECTION_NONE,
  DIRECTION_IN,
  DIRECTION_OUT
} Direction;

inline void out_tabs( std::ostream& out, int depth )
{
  for ( int i=0; i < depth; i++ ) out << "  ";
}

struct Arg {
  Arg() { }
  
  std::string dbus_name;
  std::string cxx_name;
  std::string direction_string;
  std::string cpp_type_override;
  DBus::Signature signature;

  std::string name() { if ( cxx_name.empty() ) return dbus_name; return cxx_name; }
  Direction direction();
  std::string cpp_type();
  std::string cpp_cast(const std::string& var);
  std::string dbus_cast(const std::string& var);
  std::string cpp_dbus_type();
  std::string stubsignature();
  DBus::Type type();
  std::string strfmt(int depth=0);

};

struct Interface;

struct Method {
  Method():interface(NULL) { }
  Interface* interface;
  std::string name;
  std::string cppname;

  std::vector<Arg> in_args;
  std::vector<Arg> out_args;

  std::string strfmt(int depth=0);

  std::vector<std::string> adapter_arg_names();

  std::string stubsignature();
  std::string stubname() { return name + "_adapter_stub_" + stubsignature(); }
  std::string varname()  { return "m_method_" + name + "_" + stubsignature(); }
  std::string cpp_creation();
  std::string cpp_proto();
  std::string cpp_decl();
  std::string cpp_adapter_creation();
  std::string cpp_adapter_stub();
  bool args_valid();
};

struct Signal {
  Signal():interface(NULL), ignored(0) { }
  Interface* interface;
  std::string name;
  std::vector<Arg> args;
  std::string accessor;
  int ignored;

  std::string strfmt(int depth=0);

  std::string varname() { return "m_signal_" + name; }
  std::string adapter_name() { return "m_signal_adapter_" + name; }
  std::string adapter_conn_name() { return "m_signal_adapter_connection_" + name; }
  std::string cpp_creation();
  std::vector<std::string> proxy_arg_names();
  std::vector<std::string> adapter_arg_names();
  std::string cpp_proto();
  std::string cpp_decl();
  std::string adapter_signal_create();
  std::string adapter_signal_declare();
  std::string adapter_signal_conn_declare() { if (args_valid()) return "sigc::connection " + adapter_conn_name() + ";"; return ""; }
  std::string adapter_signal_connect();
  std::string adapter_signal_disconnect() { return adapter_conn_name() + ".disconnect();"; }
  
  bool args_valid();
};

struct Node;

struct Interface {
  Interface(): node(NULL), ignored(false) { }
  Node* node;
  std::string cxx_name;
  std::string cppprefix;
  std::string dbus_name;
  bool ignored;
  std::vector<Method> methods;
  std::vector<Signal> signals;
  
  std::string strfmt(int depth=0);

  std::string name() { if ( not cxx_name.empty() ) return cxx_name; return dbus_name; }

  std::vector<std::string> cpp_method_creation();
  std::vector<std::string> cpp_method_proto();
  std::vector<std::string> cpp_method_decl();
  std::vector<std::string> cpp_adapter_creation();
  std::vector<std::string> cpp_adapter_stubs();
  std::vector<std::string> cpp_adapter_signal_connection();
  std::vector<std::string> cpp_adapter_signal_disconnection();
};

struct Node {
  Node(): ignored(false) { }
  
  std::string dbus_name;
  std::string cppname;
  std::string dbus_destination;
  std::string dbus_path;
  bool ignored;

  std::string gen_namespace;
  std::string orig_namespace;
  std::string cppinclude;
  std::string file_prefix;

  std::string adapter_parent;
  std::string adapter_parent_include;
  std::string proxy_parent;
  std::string proxy_parent_include;

  std::string adapter_include;
  std::string adapter;
  std::string proxy_include;
  std::string proxy;
  std::string accessor;
  std::string adaptee_accessor;

  std::vector<Interface> interfaces;
  std::vector<Node> children;
  std::vector<Method> methods;
  std::vector<Signal> signals;
  
  std::string strfmt(int depth=0);

  std::vector<std::string> namespaces();
  std::string cppname_upper();
  std::vector<std::string> namespaces_upper();
  std::string name()         { if ( cppname.empty() ) return dbus_name; return cppname; }
  std::string name_lower();
  std::string adapter_name() { return name() + "Adapter"; }
  std::string proxy_name()   { return name() + "Proxy"; }

  std::string adaptee_fqn();
  
  std::string cpp_namespace_begin(const std::string& tab="  ");
  std::string cpp_namespace_end(const std::string& tab="  ");
};

typedef std::vector<Node> Nodes;



#endif

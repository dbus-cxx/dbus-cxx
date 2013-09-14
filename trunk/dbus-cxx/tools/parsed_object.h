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


#include <dbus-cxx.h>

#include <cstring>
#include <string>
#include <vector>
#include <sstream>


#include "method.h"
#include "dbus_signal.h"


inline void out_tabs( std::ostream& out, int depth )
{
  for ( int i=0; i < depth; i++ ) out << "  ";
}

void merge( std::map<std::string,std::string>& tgt, const std::map<std::string,std::string>& src );

struct Interface;

struct Node;

struct DBusSignal;

struct Interface {
  Interface(): node(NULL), ignored(false) { }
  Node* node;
  std::string cxx_name;
  std::string cppprefix;
  std::string dbus_name;
  bool ignored;
  std::vector<Method> methods;
  std::vector<DBusSignal> signals;
  
  std::string strfmt(int depth=0);

  std::string name() { if ( not cxx_name.empty() ) return cxx_name; return dbus_name; }

  std::vector<std::string> cpp_adapter_methods_signals_create();
  std::vector<std::string> cpp_proxy_methods_signals();
  std::vector<std::string> cpp_declare_proxy_objects();
  std::vector<std::string> cpp_adapter_creation();
  std::vector<std::string> cpp_adapter_stubs();
  std::vector<std::string> cpp_adapter_signal_connection();
  std::vector<std::string> cpp_adapter_signal_disconnection();

  std::map<std::string,std::string> iterator_support();
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

  std::string other_proxy_parents_str;
  std::string other_proxy_parent_includes_str;

  std::vector<std::string> other_proxy_parents();
  std::vector<std::string> other_proxy_parent_includes();

  std::string adapter_include;
  std::string adapter;
  std::string proxy_include;
  std::string proxy;
  std::string accessor;
  std::string adaptee_accessor;

  std::vector<Interface> interfaces;
  std::vector<Node> children;
  std::vector<Method> methods;
  std::vector<DBusSignal> signals;
  
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

  std::map<std::string,std::string> iterator_support();
};

typedef std::vector<Node> Nodes;



#endif

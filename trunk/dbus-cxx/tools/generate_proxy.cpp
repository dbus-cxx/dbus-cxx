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
#include "generate_proxy.h"

#include <sstream>

std::string generate_proxy_h(Node n)
{
  std::ostringstream sout;
  
  std::string tab = "  ";

  std::string class_indent;

  std::string decl_indent;
  
  std::vector<std::string> nsu = n.namespaces_upper();

  for ( int i = 0; i < nsu.size(); i++ ) class_indent += tab;

  decl_indent = class_indent + tab + tab;

  std::string definestr = "__DBUS_PROXY_";
  for ( int i = 0; i < nsu.size(); i++ ) definestr += nsu[i] + "_";
  definestr += n.cppname_upper() + "_H";

  sout << "#ifndef " << definestr << "\n"
       << "#define " << definestr << "\n\n"
       << "#include <dbus-cxx.h>\n\n";
  
  sout << n.cpp_namespace_begin(tab) + "\n";

  sout << class_indent << "class " << n.proxy_name() << " : public ::DBus::ObjectProxy\n"
       << class_indent << "{\n"
       << class_indent << tab << "protected:\n"
       << decl_indent << n.proxy_name() << "( ::DBus::Connection::pointer conn, const std::string& dest=\""
       << n.dbus_destination << "\", const std::string& path=\"" << n.dbus_path << "\"):\n"
       << decl_indent << tab << "::DBus::ObjectProxy(conn, dest, path)\n"
       << decl_indent << "{\n";

  for ( int i=0; i < n.interfaces.size(); i++ )
  {
    if ( n.interfaces[i].ignored ) continue;
    std::vector<std::string> decls = n.interfaces[i].cpp_method_creation();
    for ( int k=0; k < decls.size(); k++ )
      sout << decl_indent << tab << decls[k] << "\n";
  }

  sout << decl_indent << "}\n";

  sout << "\n" << class_indent << tab << "public:\n\n"
       << decl_indent << "typedef DBusCxxPointer<" << n.proxy_name() << "> pointer;\n\n"
       << decl_indent << "static pointer create( ::DBus::Connection::pointer conn,\n"
       << decl_indent << "                       const std::string& dest=\"" << n.dbus_destination << "\",\n"
       << decl_indent << "                       const std::string& path=\"" << n.dbus_path << "\"\n"
       << decl_indent << "                     )\n"
       << decl_indent << "{ return pointer( new " << n.proxy_name() << "(conn, dest, path)); }\n\n";

  for ( int i=0; i < n.interfaces.size(); i++ )
  {
    if ( n.interfaces[i].ignored ) continue;
    std::vector<std::string> decls = n.interfaces[i].cpp_method_proto();
    for ( int k=0; k < decls.size(); k++ )
      sout << decl_indent << decls[k] << "\n\n";
  }

  sout << "\n" << class_indent << tab << "protected:\n\n";

  for ( int i=0; i < n.interfaces.size(); i++ )
  {
    if ( n.interfaces[i].ignored ) continue;
    std::vector<std::string> decls = n.interfaces[i].cpp_method_decl();
    for ( int k=0; k < decls.size(); k++ )
      sout << decl_indent << decls[k] << "\n";
    sout << "\n";
  }

  sout << "\n" << class_indent << "};\n\n"
       << n.cpp_namespace_end(tab) << "\n";

  sout << "#endif\n";

  return sout.str();
}

std::string generate_proxy_cpp(Node n)
{
  std::string proxy;

  return proxy;
}

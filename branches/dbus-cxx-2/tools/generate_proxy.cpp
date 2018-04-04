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

std::string make_decl( const std::string & basis )
{
  std::string decl = basis;
  const char* acceptable = "_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  size_t pos;
  while ( (pos = decl.find_first_not_of(acceptable)) != std::string::npos )
    decl[pos] = '_';
  for ( size_t i = 0; i < decl.size(); i++ )
    decl[i] = toupper(decl[i]);
  return decl;
}

std::string generate_proxy_h(Node n)
{
  std::ostringstream sout;
  
  std::string tab = "  ";

  std::string class_indent;

  std::string decl_indent;
  
  std::vector<std::string> nsu = n.namespaces_upper();

  bool inherit_from_dbus_objectproxy = n.proxy_parent.empty();

  for ( unsigned i = 0; i < nsu.size(); i++ ) class_indent += tab;

  decl_indent = class_indent + tab + tab;

  std::string definestr = "__DBUS_PROXY_";
  for ( unsigned i = 0; i < nsu.size(); i++ ) definestr += nsu[i] + "_";
  definestr += n.cppname_upper() + "_H";

  sout << "#ifndef " << definestr << "\n"
       << "#define " << definestr << "\n\n"
       << "#include <dbus-cxx.h>\n\n";

  if ( not n.cppinclude.empty() )
    sout << "#include " << n.cppinclude << "\n\n";

  if ( not n.proxy_parent_include.empty() )
    sout << "#include " << n.proxy_parent_include << "\n\n";

  std::vector<std::string> oppi = n.other_proxy_parent_includes();
  for ( std::vector<std::string>::iterator i = oppi.begin(); i != oppi.end(); i++ )
    sout << "#include " << *i << "\n";
  
  for ( std::vector<Node>::iterator i = n.children.begin(); i != n.children.end(); i++ )
  {
    if ( not i->proxy_include.empty() )
      sout << "#include " << i->proxy_include << "\n";
  }

  std::map<std::string,std::string> iterator_support = n.iterator_support();

  if ( iterator_support.size() > 0 )
  {
    std::map<std::string,std::string>::iterator i;
    for ( i = iterator_support.begin(); i != iterator_support.end(); i++ )
    {
      std::string definestr = "__DBUS_" + make_decl(i->first) + "_ITERATOR_SUPPORT";
      sout << "#ifndef " << definestr << "\n"
           << "  #define " << definestr << "\n"
           << "  DBUS_CXX_ITERATOR_SUPPORT( " << i->first << " , " << i->second << " )\n"
           << "#endif\n\n";
    }
  }
  
  sout << "\n" <<  n.cpp_namespace_begin( tab ) << "\n";

  // Class declaration
  if ( inherit_from_dbus_objectproxy )
    sout << class_indent << "class " << n.proxy_name() << " : public ::DBus::ObjectProxy";
  else
    sout << class_indent << "class " << n.proxy_name() << " : public " << n.proxy_parent;

  std::vector<std::string> opp = n.other_proxy_parents();
  for ( std::vector<std::string>::iterator i = opp.begin(); i != opp.end(); i++ )
    sout << ", public " << *i;

  sout << "\n";
  
  sout << class_indent << "{\n"
       << class_indent << tab << "protected:\n"
       << decl_indent << n.proxy_name() << "( ::DBus::Connection::pointer conn, const std::string& dest=\""
       << n.dbus_destination << "\", const std::string& path=\"" << n.dbus_path << "\"):\n";

  // Parent instantiation
  if ( inherit_from_dbus_objectproxy )
    sout << decl_indent << tab << "::DBus::ObjectProxy(conn, dest, path)\n";
  else
    sout << decl_indent << tab << n.proxy_parent << "(conn, dest, path)\n";

  sout << decl_indent << "{\n";

  sout << decl_indent << tab << "::DBus::Path child_path;\n";
  for ( std::vector<Node>::iterator i = n.children.begin(); i != n.children.end(); i++ )
  {
    sout << decl_indent << tab << "child_path = path; \n"
         << decl_indent << tab << "child_path.append_element(\"" << i->name() << "\");\n"
         << decl_indent << tab << "m_" << i->name() << " = " << i->proxy << "::create(conn, dest, child_path);\n";
  }

  sout << "\n";

  for ( unsigned i=0; i < n.interfaces.size(); i++ )
  {
    if ( n.interfaces[i].ignored ) continue;
    std::vector<std::string> decls = n.interfaces[i].cpp_adapter_methods_signals_create();
    for ( unsigned k=0; k < decls.size(); k++ )
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

  for ( std::vector<Node>::iterator i = n.children.begin(); i != n.children.end(); i++ )
    sout << decl_indent << i->proxy << "::pointer " << i->accessor << "() { return m_" << i->name() << "; }\n";

  sout << "\n";
  
  for ( unsigned i=0; i < n.interfaces.size(); i++ )
  {
    if ( n.interfaces[i].ignored ) continue;
    std::vector<std::string> decls = n.interfaces[i].cpp_proxy_methods_signals();
    for ( unsigned k=0; k < decls.size(); k++ )
      sout << decl_indent << decls[k] << "\n\n";
  }

  sout << "\n" << class_indent << tab << "protected:\n\n";

  for ( std::vector<Node>::iterator i=n.children.begin(); i != n.children.end(); i++ )
    sout << decl_indent << i->proxy << "::pointer m_" << i->name() << ";\n";

  sout << "\n";

  for ( size_t i=0; i < n.interfaces.size(); i++ )
  {
    if ( n.interfaces[i].ignored ) continue;
    std::vector<std::string> decls = n.interfaces[i].cpp_declare_proxy_objects();
    for ( unsigned k=0; k < decls.size(); k++ )
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

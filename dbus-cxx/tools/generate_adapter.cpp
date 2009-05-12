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
#include "generate_adapter.h"

#include <sstream>

std::string generate_adapter_h(Node n)
{
  std::ostringstream sout;
  
  std::string tab = "  ";

  std::string class_indent;

  std::string decl_indent;
  
  std::vector<std::string> nsu = n.namespaces_upper();

  for ( int i=0; i < n.interfaces.size(); i++ ) n.interfaces[i].node = &n;

  for ( int i = 0; i < nsu.size(); i++ ) class_indent += tab;

  decl_indent = class_indent + tab + tab;

  std::string definestr = "__DBUS_ADAPTER_";
  for ( int i = 0; i < nsu.size(); i++ ) definestr += nsu[i] + "_";
  definestr += n.cppname_upper() + "_H";

  sout << "#ifndef " << definestr << "\n"
      << "#define " << definestr << "\n\n"
      << "#include <dbus-cxx.h>\n\n";

  if ( not n.cppinclude.empty() )
    sout << "#include " << n.cppinclude << "\n\n";
  
  sout << n.cpp_namespace_begin(tab) + "\n";

  sout << class_indent << "class " << n.adapter_name() << " : public DBus::Object\n"
       << class_indent << "{\n"
       << class_indent << tab << "protected:\n"
       << decl_indent << n.adapter_name() << "( " << n.adaptee_fqn() << "* adaptee=NULL, const std::string& path=\"" << n.dbus_path << "\"):\n"
       << decl_indent << tab << "DBus::Object(path),\n"
       << decl_indent << tab << "m_adaptee(adaptee)\n"
       << decl_indent << "{\n";

  for ( int i=0; i < n.interfaces.size(); i++ )
  {
    if ( n.interfaces[i].ignored ) continue;
    std::vector<std::string> decls = n.interfaces[i].cpp_adapter_creation();
    for ( int k=0; k < decls.size(); k++ )
      sout << decl_indent << tab << decls[k] << "\n";
  }

  sout << decl_indent << "}\n";

  sout << "\n" << class_indent << tab << "public:\n\n"
      << decl_indent << "typedef DBusCxxPointer<" << n.adapter_name() << "> pointer;\n\n"
      << decl_indent << "static pointer create( " << n.adaptee_fqn() << "* adaptee=NULL, const std::string& path=\"" << n.dbus_path << "\")\n"
      << decl_indent << tab << "{ return pointer( new " << n.adapter_name() << "(adaptee, path)); }\n\n";

  sout << decl_indent << n.adaptee_fqn() << "* adaptee() { return m_adaptee; }\n\n"
       << decl_indent << "void set_adaptee( " << n.adaptee_fqn() << "* adaptee ) { m_adaptee = adaptee; }\n\n";

  sout << "\n" << class_indent << tab << "protected:\n\n"
       << decl_indent << n.adaptee_fqn() << "* m_adaptee;\n\n"
       << decl_indent << "void check_adaptee() { if ( not m_adaptee) throw DBus::ErrorInvalidAdaptee(); }\n\n";

  for ( int i=0; i < n.interfaces.size(); i++ )
  {
    if ( n.interfaces[i].ignored ) continue;
    std::vector<std::string> decls = n.interfaces[i].cpp_adapter_stubs();
    for ( int k=0; k < decls.size(); k++ )
      sout << decl_indent << decls[k] << "\n";
    sout << "\n";
  }

  sout << "\n" << class_indent << "};\n\n"
      << n.cpp_namespace_end(tab) << "\n";

  sout << "#endif\n";

  return sout.str();
}

std::string generate_adapter_cpp(Node n)
{
  return std::string();
}


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

  bool inherit_from_dbus_object = n.adapter_parent.empty();

  for ( unsigned i=0; i < n.interfaces.size(); i++ ) n.interfaces[i].node = &n;

  for ( unsigned i = 0; i < nsu.size(); i++ ) class_indent += tab;

  decl_indent = class_indent + tab + tab;

  std::string definestr = "__DBUS_ADAPTER_";
  for ( unsigned i = 0; i < nsu.size(); i++ ) definestr += nsu[i] + "_";
  definestr += n.cppname_upper() + "_H";

  sout << "#ifndef " << definestr << "\n"
      << "#define " << definestr << "\n\n"
      << "#include <dbus-cxx.h>\n\n";

  if ( not n.cppinclude.empty() )
    sout << "#include " << n.cppinclude << "\n\n";

  if ( not n.adapter_parent_include.empty() )
    sout << "#include " << n.adapter_parent_include << "\n\n";

  for ( std::vector<Node>::iterator i = n.children.begin(); i != n.children.end(); i++ )
  {
    if ( not i->adapter_include.empty() )
      sout << "#include " << i->adapter_include << "\n";
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
  
  
  sout << "\n";
  
  sout << n.cpp_namespace_begin(tab) + "\n";

  // Class declaration
  // if there is no specific parent we'll inherit from DBus::Object
  if ( inherit_from_dbus_object )
    sout << class_indent << "class " << n.adapter_name() << " : public ::DBus::Object\n";
  // Otherwise we'll inherit from the explicit parent
  else
    sout << class_indent << "class " << n.adapter_name() << " : public " << n.adapter_parent << "\n";

  sout << class_indent << "{\n"
       << class_indent << tab << "protected:\n"
       << decl_indent << n.adapter_name() << "( " << n.adaptee_fqn() << "* adaptee=NULL, const std::string& path=\"" << n.dbus_path << "\"):\n";

  // We need to instantiate DBus::Object unless we have a parent
  if ( inherit_from_dbus_object )
    sout << decl_indent << tab << "::DBus::Object(path),\n";
  // Otherwise we need to instantiate the parent with the path and adaptee
  else
    sout << decl_indent << tab << n.adapter_parent << "(adaptee, path),\n";
  
  sout << decl_indent << tab << "m_adaptee(adaptee)\n"
       << decl_indent << "{\n\n"
       << decl_indent << tab << "::DBus::MethodBase::pointer temp_method;\n\n";

  for ( unsigned i=0; i < n.interfaces.size(); i++ )
  {
    if ( n.interfaces[i].ignored ) continue;
    std::vector<std::string> decls = n.interfaces[i].cpp_adapter_creation();
    for ( unsigned k=0; k < decls.size(); k++ )
      sout << decl_indent << tab << decls[k] << "\n";
  }

  if ( n.children.size() > 0 )
    sout << "\n" << decl_indent << tab << "::DBus::Path child_path;\n";

  for ( std::vector<Node>::iterator i=n.children.begin(); i != n.children.end(); i++ )
  {
    sout << decl_indent << tab << "\n"
         << decl_indent << tab << "child_path = path;\n"
         << decl_indent << tab << "child_path.append_element(\"" << i->name() << "\");\n"
         << decl_indent << tab << "DBUS_CXX_DEBUG( \"" << i->name() << ":constructor: child path = \" << child_path);\n"
         << decl_indent << tab << "m_" << i->name() << " = " << i->adapter << "::create(NULL, child_path);"
         << decl_indent << tab << "this->add_child(\"" << i->name() << "\", m_" << i->name() << ");\n"
         << decl_indent << tab << "if ( adaptee ) m_" << i->name() << "->set_adaptee(&(adaptee->" << i->adaptee_accessor << "));\n";
  }

  sout << decl_indent << "}\n";

  sout << "\n" << class_indent << tab << "public:\n\n"
      << decl_indent << "typedef DBusCxxPointer<" << n.adapter_name() << "> pointer;\n\n"
      << decl_indent << "static pointer create( " << n.adaptee_fqn() << "* adaptee=NULL, const std::string& path=\"" << n.dbus_path << "\")\n"
      << decl_indent << tab << "{ return pointer( new " << n.adapter_name() << "(adaptee, path)); }\n\n";

  sout << decl_indent << n.adaptee_fqn() << "* adaptee() { return m_adaptee; }\n\n"
       << decl_indent << "void set_adaptee( " << n.adaptee_fqn() << "* adaptee ) {\n"
       << decl_indent << tab << "m_adaptee = adaptee;\n";

  for ( size_t i=0; i < n.interfaces.size(); i++ )
  {
    if ( n.interfaces[i].ignored ) continue;
    std::vector<std::string> decls;
    decls = n.interfaces[i].cpp_adapter_signal_disconnection();
    for ( size_t k=0; k < decls.size(); k++ )
      sout << decl_indent << tab << decls[k] << "\n";
    decls = n.interfaces[i].cpp_adapter_signal_connection();
    for ( size_t k=0; k < decls.size(); k++ )
      sout << decl_indent << tab << decls[k] << "\n";
  }

  if ( n.children.size() > 0 )
  {
    sout << decl_indent << tab << "if ( m_adaptee ) {\n";
    for ( std::vector<Node>::iterator i=n.children.begin(); i != n.children.end(); i++ )
      sout << decl_indent << tab << tab << "m_" << i->name() << "->set_adaptee( m_adaptee->" << i->adaptee_accessor << " );\n";
    sout << decl_indent << tab << "}\n"
         << decl_indent << tab << "else {\n";
    for ( std::vector<Node>::iterator i=n.children.begin(); i != n.children.end(); i++ )
      sout << decl_indent << tab << tab << "m_" << i->name() << "->set_adaptee( NULL );\n";
    sout << decl_indent << tab << "}\n";
    sout << decl_indent << "}\n\n";

    for ( std::vector<Node>::iterator i=n.children.begin(); i != n.children.end(); i++ )
      sout << decl_indent << i->adapter << "::pointer " << i->accessor << "() { return m_" << i->name() << "; }\n\n";
  }
  else
  {
    sout << decl_indent << "}\n\n";
  }

  sout << decl_indent << "void set_adaptee( " << n.adaptee_fqn() << "& adaptee ) {\n"
       << decl_indent << tab << "this->set_adaptee(&adaptee);\n"
       << decl_indent << "}\n\n";

  // Start of private section
  // These need to be private so children can use the same names without conflict
  sout << "\n" << class_indent << tab << "private:\n\n";

  sout << decl_indent << n.adaptee_fqn() << "* m_adaptee;\n\n";

  for ( std::vector<Node>::iterator i = n.children.begin(); i != n.children.end(); i++ )
    sout << decl_indent << i->adapter << "::pointer m_" << i->name() << ";\n";

  sout << "\n" << decl_indent << "void check_adaptee() { if ( not m_adaptee) throw ::DBus::ErrorInvalidAdaptee::create(); }\n\n";

  for ( unsigned i=0; i < n.interfaces.size(); i++ )
  {
    if ( n.interfaces[i].ignored ) continue;
    std::vector<std::string> decls = n.interfaces[i].cpp_adapter_stubs();
    for ( unsigned k=0; k < decls.size(); k++ )
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


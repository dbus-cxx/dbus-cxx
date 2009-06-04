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
#include "parsed_object.h"

#include <cstring>
#include <algorithm>
#include <cctype>

char upper( char c ) { return std::toupper( c ); }

char lower( char c ) { return std::tolower( c ); }


Direction Arg::direction()
{
  if ( strcasecmp( direction_string.c_str(), "in" ) == 0 )
    return DIRECTION_IN;
  else if ( strcasecmp( direction_string.c_str(), "out" ) == 0 )
    return DIRECTION_OUT;
  return DIRECTION_NONE;
}

std::string Arg::cpp_type()
{
  switch ( type() ) {
    case DBus::TYPE_INVALID:     throw DBus::ErrorInvalidMessageType();
    case DBus::TYPE_BYTE:        return "uint8_t";
    case DBus::TYPE_BOOLEAN:     return "bool";
    case DBus::TYPE_INT16:       return "int16_t";
    case DBus::TYPE_UINT16:      return "uint16_t";
    case DBus::TYPE_INT32:       return "int32_t";
    case DBus::TYPE_UINT32:      return "uint32_t";
    case DBus::TYPE_INT64:       return "int64_t";
    case DBus::TYPE_UINT64:      return "uint64_t";
    case DBus::TYPE_DOUBLE:      return "double";
    case DBus::TYPE_STRING:      return "std::string";
    case DBus::TYPE_OBJECT_PATH: return "::DBus::Path";
    case DBus::TYPE_SIGNATURE:   return "::DBus::Signature";
    case DBus::TYPE_ARRAY:       throw DBus::ErrorInvalidMessageType();
    case DBus::TYPE_VARIANT:     throw DBus::ErrorInvalidMessageType();
    case DBus::TYPE_STRUCT:      throw DBus::ErrorInvalidMessageType();
    case DBus::TYPE_DICT_ENTRY:  throw DBus::ErrorInvalidMessageType();
  }

  throw DBus::ErrorInvalidMessageType();
}

std::string Arg::stubsignature()
{
  switch ( type() ) {
    case DBus::TYPE_INVALID:     throw  DBus::ErrorInvalidMessageType();
    case DBus::TYPE_BYTE:        return DBus::signature<uint8_t>();
    case DBus::TYPE_BOOLEAN:     return DBus::signature<bool>();
    case DBus::TYPE_INT16:       return DBus::signature<int16_t>();
    case DBus::TYPE_UINT16:      return DBus::signature<uint16_t>();
    case DBus::TYPE_INT32:       return DBus::signature<int32_t>();
    case DBus::TYPE_UINT32:      return DBus::signature<uint32_t>();
    case DBus::TYPE_INT64:       return DBus::signature<int64_t>();
    case DBus::TYPE_UINT64:      return DBus::signature<uint64_t>();
    case DBus::TYPE_DOUBLE:      return DBus::signature<double>();
    case DBus::TYPE_STRING:      return DBus::signature<std::string>();
    case DBus::TYPE_OBJECT_PATH: return DBus::signature<DBus::Path>();
    case DBus::TYPE_SIGNATURE:   return DBus::signature<DBus::Signature>();
    case DBus::TYPE_ARRAY:       throw  DBus::ErrorInvalidMessageType();
    case DBus::TYPE_VARIANT:     throw  DBus::ErrorInvalidMessageType();
    case DBus::TYPE_STRUCT:      throw  DBus::ErrorInvalidMessageType();
    case DBus::TYPE_DICT_ENTRY:  throw  DBus::ErrorInvalidMessageType();
  }

  throw DBus::ErrorInvalidMessageType();
}

DBus::Type Arg::type()
{
  if ( not signature.is_singleton() ) return DBus::TYPE_INVALID;
  if ( not signature.begin().is_basic() ) return DBus::TYPE_INVALID;
  return signature.begin().type();
}

std::string Arg::strfmt( int depth )
{
  std::ostringstream sout;
  out_tabs( sout, depth );
  if ( direction() != DIRECTION_NONE )
    sout << "Arg: " << dbus_name << "\t\tType: " << signature << "\t\tDir: " << direction_string << std::endl;
  else
    sout << "Arg: " << dbus_name << "\t\tType: " << signature << std::endl;
  return sout.str();
}

std::string Method::strfmt( int depth )
{
  std::ostringstream sout;
  out_tabs( sout, depth );
  sout << "Method: " << name << std::endl;
  for ( unsigned int i=0; i < in_args.size(); i++ ) sout << in_args[i].strfmt( depth+1 );
  for ( unsigned int i=0; i < out_args.size(); i++ ) sout << out_args[i].strfmt( depth+1 );
  return sout.str();
}

std::string Method::cpp_creation()
{
  std::string s;
  if ( args_valid() ) {
    s = varname() + " = this->create_method<";
    if ( out_args.size() == 0 )
      s += "void";
    else
      s += out_args[0].cpp_type();
    for ( unsigned int i = 0; i < in_args.size(); i++ )
      s += "," + in_args[i].cpp_type();
    s += ">( ";
    if ( interface != NULL )
      s += "\"" + interface->name() + "\", ";
    s += "\"" + name + "\" );";
  }
  return s;
}

std::string Method::cpp_proto()
{
  std::ostringstream sout;
  if ( args_valid() ) {
    if ( out_args.size() == 0 )
      sout << "void";
    else
      sout << out_args[0].cpp_type();
    sout << " " << name << "(";
    for ( unsigned int i = 0; i < in_args.size(); i++ )
      sout << (( i==0 )?" ":", " ) << in_args[i].cpp_type() << " " << in_args[i].name();
    sout << " ) { return (*" << varname() << ")(";
    for ( unsigned int i = 0; i < in_args.size(); i++ )
      sout << (( i==0 )?" ":", " ) << in_args[i].name();
    sout << "); }";
  }
  else {
    sout << "/* can't create proxy for method " << name << " */";
  }
  return sout.str();
}

std::string Method::cpp_decl()
{
  std::string s;
  if ( args_valid() ) {
    s = "::DBus::MethodProxy<";
    if ( out_args.size() == 0 )
      s += "void";
    else
      s += out_args[0].cpp_type();
    for ( unsigned int i = 0; i < in_args.size(); i++ )
      s += "," + in_args[i].cpp_type();
    s += ">::pointer " + varname() + ";";
  }
  return s;
}

std::string Method::stubsignature()
{
  std::string ss;
  if ( out_args.size() == 0 ) ss += 'v';
  else ss += out_args[0].stubsignature();
  for ( size_t i = 0; i < in_args.size(); i++ )
    ss += in_args[i].stubsignature();
  return ss;
}

std::string Method::cpp_adapter_creation()
{
  std::ostringstream sout;
  if ( args_valid() ) {
    sout << "temp_method = this->create_method<";
    if ( out_args.size() == 0 )
      sout << "void";
    else
      sout << out_args[0].cpp_type();
    for ( unsigned int i = 0; i < in_args.size(); i++ )
      sout << "," << in_args[i].cpp_type();
    sout << ">( ";
    if ( interface != NULL )
      sout << "\"" << interface->name() << "\", ";
    sout << "\"" << name << "\", sigc::mem_fun(*this, &" << interface->node->name() << "Adapter::" << stubname() << ") );";
  }
  return sout.str();
}

std::vector<std::string> Method::adapter_arg_names()
{
  std::vector<std::string> decls;
  std::ostringstream sout;
  if ( out_args.size() > 0 )
  {
    sout << "temp_method->set_arg_name( " << 0 << ", \"" << out_args[0].dbus_name << "\" );";
    decls.push_back(sout.str());
    sout.str("");
  }
  for ( size_t i = 0; i < in_args.size(); i++ )
  {
    sout << "temp_method->set_arg_name( " << i+1 << ", \"" << in_args[i].dbus_name << "\" );";
    decls.push_back(sout.str());
    sout.str("");
  }
  return decls;
}


std::string Method::cpp_adapter_stub()
{
  std::ostringstream sout;
  if ( args_valid() ) {
    if ( out_args.size() == 0 )
      sout << "void";
    else
      sout << out_args[0].cpp_type();
    sout << " " << stubname() << "(";
    for ( unsigned int i = 0; i < in_args.size(); i++ )
      sout << (( i==0 )?" ":", " ) << in_args[i].cpp_type() << " " << in_args[i].name();
    sout << " ) { this->check_adaptee(); ";
    if (out_args.size() != 0 ) sout << "return ";
    sout << "m_adaptee->";
    if ( cppname.empty() ) sout << name;
    else sout << cppname;
    sout << "(";
    for ( unsigned int i = 0; i < in_args.size(); i++ )
      sout << (( i==0 )?" ":", " ) << in_args[i].name();
    sout << "); }";
  }
  else {
    sout << "/* can't create adapter for method " << name << " */";
  }
  return sout.str();
}

bool Method::args_valid()
{
  if ( in_args.size() > 7 ) return false;

  for ( unsigned int i=0; i < in_args.size(); i++ )
    if ( in_args[i].type() == DBus::TYPE_INVALID ) return false;

  for ( unsigned int i=0; i < out_args.size(); i++ )
    if ( out_args[i].type() == DBus::TYPE_INVALID ) return false;

  if ( out_args.size() > 1 ) return false;

  return true;
}

std::string Signal::cpp_creation()
{
  std::ostringstream sout;
  if ( args_valid() ) {
    sout << varname() << " = this->create_signal<void";
    for ( size_t i = 0; i < args.size(); i++ )
      sout << "," << args[i].cpp_type();
    sout << ">( ";
    if ( interface == NULL ) throw("bad signal interface");
    sout << "\"" << interface->name() << "\", ";
    sout << "\"" << name << "\" );";
  }
  return sout.str();
}

std::vector<std::string> Signal::proxy_arg_names()
{
  std::vector<std::string> decls;
  std::ostringstream sout;
  for ( size_t i = 0; i < args.size(); i++ )
  {
    sout << varname() << "->set_arg_name( " << i << ", \"" << args[i].dbus_name << "\" );";
    decls.push_back(sout.str());
    sout.str("");
  }
  return decls;
}

std::string Signal::cpp_proto()
{
  std::ostringstream sout;
  if ( args_valid() ) {
    sout << "::DBus::signal_proxy<void";
    for ( unsigned int i = 0; i < args.size(); i++ )
      sout << "," << args[i].cpp_type();
    sout << " >& signal_" << name << "() { return *" << varname() << "; }";
  }
  else {
    sout << "/* can't create proxy for signal " << name << " */";
  }
  return sout.str();
}

std::string Signal::cpp_decl()
{
  std::string s;
  if ( args_valid() ) {
    s = "::DBus::signal_proxy<void";
    for ( unsigned int i = 0; i < args.size(); i++ )
      s += "," + args[i].cpp_type();
    s += ">::pointer " + varname() + ";";
  }
  return s;
}

bool Signal::args_valid()
{
  if ( args.size() > 7 ) return false;

  for ( unsigned int i=0; i < args.size(); i++ )
    if ( args[i].type() == DBus::TYPE_INVALID ) return false;

  return true;
}

std::string Signal::strfmt( int depth )
{
  std::ostringstream sout;
  out_tabs( sout, depth );
  sout << "Signal: " << name << std::endl;
  for ( unsigned int i=0; i < args.size(); i++ ) sout << args[i].strfmt( depth+1 );
  return sout.str();
}

std::string Signal::adapter_signal_create()
{
  std::ostringstream sout;
  if ( args_valid() ) {
    sout << adapter_name() << " = this->create_signal<void";
    for ( unsigned int i=0; i < args.size(); i++ ) sout << "," << args[i].cpp_type();
    sout << ">(\"" << interface->name() << "\",\"" << name << "\");";
  }
  return sout.str();
}

std::vector<std::string> Signal::adapter_arg_names()
{
  std::vector<std::string> decls;
  std::ostringstream sout;
  for ( size_t i = 0; i < args.size(); i++ )
  {
    sout << adapter_name() << "->set_arg_name( " << i << ", \"" << args[i].dbus_name << "\" );";
    decls.push_back(sout.str());
    sout.str("");
  }
  return decls;
}

std::string Signal::adapter_signal_declare()
{
  std::ostringstream sout;
  if ( args_valid() ) {
    sout << "::DBus::signal<void";
    for ( unsigned int i = 0; i < args.size(); i++ ) sout << "," << args[i].cpp_type();
    sout << ">::pointer " << adapter_name() << ";";
  }
  return sout.str();
}

std::string Signal::adapter_signal_connect()
{
  if ( accessor.empty() ) return "";
  std::ostringstream sout;
  sout << "if ( m_adaptee ) " << adapter_conn_name() << " = m_adaptee->" << accessor << ".connect( *" << adapter_name() << " );";
  return sout.str();
}

std::string Interface::strfmt( int depth )
{
  std::ostringstream sout;
  out_tabs( sout, depth );
  sout << "Interface: " << dbus_name << "  prefix=" << cppprefix << "  " << ( ignored?" [ignored]":" [built]" ) << std::endl;
  for ( unsigned int i=0; i < methods.size(); i++ ) sout << methods[i].strfmt( depth+1 );
  for ( unsigned int i=0; i < signals.size(); i++ ) sout << signals[i].strfmt( depth+1 );
  return sout.str();
}

std::vector< std::string > Interface::cpp_method_creation()
{
  std::vector<std::string> strings;
  for ( unsigned int i = 0; i < methods.size(); i++ ) {
    methods[i].interface = this;
    if ( methods[i].args_valid() )
      strings.push_back( methods[i].cpp_creation() );
  }
  for ( unsigned int i = 0; i < signals.size(); i++ ) {
    signals[i].interface = this;
    if ( signals[i].args_valid() )
      strings.push_back( signals[i].cpp_creation() );
  }
  return strings;
}

std::vector< std::string > Interface::cpp_method_proto()
{
  std::vector<std::string> strings;
  for ( unsigned int i = 0; i < methods.size(); i++ ) {
    methods[i].interface = this;
    strings.push_back( methods[i].cpp_proto() );
  }
  for ( unsigned int i = 0; i < signals.size(); i++ ) {
    signals[i].interface = this;
    strings.push_back( signals[i].cpp_proto() );
  }
  return strings;
}

std::vector< std::string > Interface::cpp_method_decl()
{
  std::vector<std::string> strings;
  for ( unsigned int i = 0; i < methods.size(); i++ ) {
    methods[i].interface = this;
    if ( methods[i].args_valid() )
      strings.push_back( methods[i].cpp_decl() );
  }
  for ( unsigned int i = 0; i < signals.size(); i++ ) {
    signals[i].interface = this;
    if ( signals[i].args_valid() )
      strings.push_back( signals[i].cpp_decl() );
  }
  return strings;
}

std::vector< std::string > Interface::cpp_adapter_creation()
{
  std::vector<std::string> strings;
  for ( unsigned int i = 0; i < methods.size(); i++ ) {
    methods[i].interface = this;
    if ( methods[i].args_valid() )
    {
      strings.push_back( methods[i].cpp_adapter_creation() );
      std::vector<std::string> names = methods[i].adapter_arg_names();
      strings.insert(strings.end(), names.begin(), names.end());
    }
  }
  for ( unsigned int i = 0; i < signals.size(); i++ ) {
    signals[i].interface = this;
    if ( signals[i].args_valid() )
    {
      strings.push_back( signals[i].adapter_signal_create() );
      std::vector<std::string> names = signals[i].adapter_arg_names();
      strings.insert(strings.end(), names.begin(), names.end());
      strings.push_back( signals[i].adapter_signal_connect() );
    }
  }
  return strings;
}

std::vector< std::string > Interface::cpp_adapter_signal_connection()
{
  std::vector<std::string> strings;
  for ( size_t i = 0; i < signals.size(); i++ ) {
    signals[i].interface = this;
    if ( signals[i].args_valid() )
      strings.push_back( signals[i].adapter_signal_connect() );
  }
  return strings;
}

std::vector< std::string > Interface::cpp_adapter_signal_disconnection()
{
  std::vector<std::string> strings;
  for ( size_t i = 0; i < signals.size(); i++ ) {
    signals[i].interface = this;
    if ( signals[i].args_valid() )
      strings.push_back( signals[i].adapter_signal_disconnect() );
  }
  return strings;
}

std::vector< std::string > Interface::cpp_adapter_stubs()
{
  std::vector<std::string> strings;
  for ( unsigned int i = 0; i < methods.size(); i++ ) {
    methods[i].interface = this;
    if ( methods[i].args_valid() )
      strings.push_back( methods[i].cpp_adapter_stub() );
  }
  for ( unsigned int i = 0; i < signals.size(); i++ ) {
    signals[i].interface = this;
    if ( signals[i].args_valid() and not signals[i].ignored )
    {
      strings.push_back( signals[i].adapter_signal_declare() );
      strings.push_back( signals[i].adapter_signal_conn_declare() );
    }
    }
  return strings;
}

std::vector<std::string> Node::namespaces()
{
  const char* current = gen_namespace.c_str();
  const char* nsseparator;
  std::vector<std::string> ns;
  while (( nsseparator = strstr( current,"::" ) ) != NULL ) {
    ns.push_back( std::string( current, nsseparator-current ) );
    current = nsseparator+2;
  }

  if ( current != '\0' ) ns.push_back( std::string( current ) );

  return ns;
}


std::string Node::cppname_upper()
{
  std::string uppercppname = name();
  std::transform( uppercppname.begin(), uppercppname.end(), uppercppname.begin(), upper );
  return uppercppname;
}

std::vector< std::string > Node::namespaces_upper()
{
  std::vector<std::string> ns = this->namespaces();
  for ( unsigned int i = 0; i < ns.size(); i++ )
    std::transform( ns[i].begin(), ns[i].end(), ns[i].begin(), upper );
  return ns;
}

std::string Node::strfmt( int depth )
{
  std::ostringstream sout;
  out_tabs( sout, depth );
  sout << "Node: " << dbus_name << " cppname(" << cppname << ") namespace(" << gen_namespace << ")" << std::endl;
  out_tabs( sout,depth+1 );
  sout << "Namespaces: ";
  std::vector<std::string> ns = namespaces();
  for ( unsigned int i = 0; i < ns.size(); i++ ) sout << ns[i] << " ";
  sout << std::endl;
  for ( unsigned int i=0; i < methods.size(); i++ ) sout << methods[i].strfmt( depth+1 );
  for ( unsigned int i=0; i < signals.size(); i++ ) sout << signals[i].strfmt( depth+1 );
  for ( unsigned int i=0; i < interfaces.size(); i++ ) sout << interfaces[i].strfmt( depth+1 );
  for ( unsigned int i=0; i < children.size(); i++ ) sout << children[i].strfmt( depth+1 );
  return sout.str();
}

std::string Node::cpp_namespace_begin( const std::string& tab )
{
  std::string result;
  std::string indent;
  std::vector<std::string> ns = this->namespaces();
  if ( ns.size() == 0 ) return "";
  for ( unsigned int i = 0; i < ns.size(); i++ ) {
    result += indent;
    result += "namespace " + ns[i] + " {\n";
    indent += tab;
  }
  return result;
}

std::string Node::cpp_namespace_end( const std::string& tab )
{
  std::string result;
  std::string indent;
  std::vector<std::string> ns = this->namespaces();
  if ( ns.size() == 0 ) return "";
  for ( unsigned int i = 0; i < ns.size(); i++ ) {
    result = "}\n" + result;
    result = indent + result;
    indent += tab;
  }
  return result;
}

std::string Node::adaptee_fqn()
{
  if ( not orig_namespace.empty() )
    return orig_namespace + "::" + name();
  return name();
}

std::string Node::name_lower()
{
  std::string lname = name();
  std::transform( lname.begin(), lname.end(), lname.begin(), lower );
  return lname;
}




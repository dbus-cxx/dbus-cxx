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

void merge( std::map<std::string,std::string>& tgt, const std::map<std::string,std::string>& src )
{
  std::map<std::string,std::string>::const_iterator s;
  std::map<std::string,std::string>::iterator t;

  for ( s = src.begin(); s != src.end(); s++ )
  {
    t = tgt.find( s->first );
    if ( t != tgt.end() and t->second != s->second )
    {
      std::string errstr = "Error: multiple mappings to " + t->first;
      throw (errstr);
    }
    else
    {
      tgt[s->first] = s->second;
    }
  }
}

Direction Arg::direction()
{
  if ( strcasecmp( direction_string.c_str(), "in" ) == 0 )
    return DIRECTION_IN;
  else if ( strcasecmp( direction_string.c_str(), "out" ) == 0 )
    return DIRECTION_OUT;
  return DIRECTION_NONE;
}

// std::string Arg::cpp_cast(const std::string& var)
// {
//   if ( cpp_type_override.empty() ) return var;
//   return "static_cast< " + cpp_type_override + " >( " + var + " )";
// }
// 
// std::string Arg::dbus_cast(const std::string& var)
// {
//   if ( cpp_type_override.empty() ) return var;
//   return "static_cast< " + cpp_dbus_type() + " >( " + var + " )";
// }

std::string Arg::cpp_type(ProxyAdapter pa)
{
  std::string type;
  
  if ( cpp_type_override.empty() ) type = cpp_dbus_type();
  else type = cpp_type_override;

  if ( pa == PROXY_PARAM and is_const ) type = "const " + type;
  if ( pa == PROXY_PARAM and is_ref )   type = type + "&";

  return type;
}

std::string Arg::cpp_dbus_type()
{
  switch ( type() ) {
    case DBus::TYPE_INVALID:     throw DBus::ErrorInvalidMessageType::create();
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
    case DBus::TYPE_ARRAY:       throw DBus::ErrorInvalidMessageType::create();
    case DBus::TYPE_VARIANT:     throw DBus::ErrorInvalidMessageType::create();
    case DBus::TYPE_STRUCT:      throw DBus::ErrorInvalidMessageType::create();
    case DBus::TYPE_DICT_ENTRY:  throw DBus::ErrorInvalidMessageType::create();
  }

  throw DBus::ErrorInvalidMessageType::create();
}

std::string Arg::stubsignature()
{
  switch ( type() ) {
    case DBus::TYPE_INVALID:     throw  DBus::ErrorInvalidMessageType::create();
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
    case DBus::TYPE_ARRAY:       throw  DBus::ErrorInvalidMessageType::create();
    case DBus::TYPE_VARIANT:     throw  DBus::ErrorInvalidMessageType::create();
    case DBus::TYPE_STRUCT:      throw  DBus::ErrorInvalidMessageType::create();
    case DBus::TYPE_DICT_ENTRY:  throw  DBus::ErrorInvalidMessageType::create();
  }

  throw DBus::ErrorInvalidMessageType::create();
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

std::string Method::cpp_adapter_create()
{
  std::string s;
  if ( args_valid() ) {
    s = varname() + " = this->create_method< ";
    if ( out_args.size() == 0 )
      s += "void";
    else
      s += out_args[0].cpp_type(PROXY_RET);
    for ( unsigned int i = 0; i < in_args.size(); i++ )
      s += "," + in_args[i].cpp_type(PROXY_PARAM);
    s += " >( ";
    if ( interface != NULL )
      s += "\"" + interface->name() + "\", ";
    s += "\"" + name + "\" );";
  }
  return s;
}

std::string Method::cpp_proxy_method()
{
  std::ostringstream sout;
  if ( args_valid() ) {
    if ( is_virtual ) sout << "virtual ";
    if ( out_args.size() == 0 )
      sout << "void";
    else
      sout << out_args[0].cpp_type(PROXY_RET);
    sout << " " << get_name() << "(";
    for ( unsigned int i = 0; i < in_args.size(); i++ )
      sout << (( i==0 )?" ":", " ) << in_args[i].cpp_type(PROXY_PARAM) << " " << in_args[i].name();
    sout << " )";
    if ( is_const ) sout << " const";
    sout << " { return ";
    sout << "(*" << varname() << ")(";
    for ( unsigned int i = 0; i < in_args.size(); i++ )
      sout << (( i==0 )?" ":", " ) << in_args[i].name();
    sout << ")";
    sout << "; }";
  }
  else {
    sout << "/* can't create proxy for method " << name << " */";
  }
  return sout.str();
}

std::string Method::cpp_declare_proxy()
{
  std::string s;
  if ( args_valid() ) {
    s = "::DBus::MethodProxy<";
    if ( out_args.size() == 0 )
      s += "void";
    else
      s += out_args[0].cpp_type(PROXY_RET);
    for ( unsigned int i = 0; i < in_args.size(); i++ )
      s += "," + in_args[i].cpp_type(PROXY_PARAM);
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
      sout << out_args[0].cpp_type(ADAPTER_RET);
    for ( unsigned int i = 0; i < in_args.size(); i++ )
      sout << "," << in_args[i].cpp_type(ADAPTER_PARAM);
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
      sout << out_args[0].cpp_type(ADAPTER_RET);
    sout << " " << stubname() << "(";
    for ( unsigned int i = 0; i < in_args.size(); i++ )
      sout << (( i==0 )?" ":", " ) << in_args[i].cpp_type(ADAPTER_PARAM) << " " << in_args[i].name();
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

std::map<std::string,std::string> Method::iterator_support()
{
  std::map<std::string,std::string> need_support;
  std::pair<std::string,std::string> support;
  std::vector<Arg>::iterator a;

  for ( a = in_args.begin(); a != in_args.end(); a++ )
  {
    if ( a->need_iterator_support() )
    {
      support = a->iterator_support();
      std::map<std::string,std::string> tmp;
      tmp[support.first] = support.second;
      merge(need_support, tmp);
    }
  }
  
  for ( a = out_args.begin(); a != out_args.end(); a++ )
  {
    if ( a->need_iterator_support() )
    {
      support = a->iterator_support();
      std::map<std::string,std::string> tmp;
      tmp[support.first] = support.second;
      merge(need_support, tmp);
    }
  }
  
  return need_support;
}

std::string Signal::cpp_adapter_create()
{
  std::ostringstream sout;
  if ( args_valid() ) {
    sout << varname() << " = this->create_signal<void";
    for ( size_t i = 0; i < args.size(); i++ )
      sout << "," << args[i].cpp_type(ADAPTER_PARAM);
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

std::string Signal::cpp_proxy_accessor()
{
  std::ostringstream sout;
  if ( args_valid() ) {
    sout << "::DBus::signal_proxy<void";
    for ( unsigned int i = 0; i < args.size(); i++ )
      sout << "," << args[i].cpp_type(PROXY_PARAM);
    sout << " >& signal_" << name << "() { return *" << varname() << "; }";
  }
  else {
    sout << "/* can't create proxy for signal " << name << " */";
  }
  return sout.str();
}

std::string Signal::cpp_declare_proxy()
{
  std::string s;
  if ( args_valid() ) {
    s = "::DBus::signal_proxy<void";
    for ( unsigned int i = 0; i < args.size(); i++ )
      s += "," + args[i].cpp_type(PROXY_PARAM);
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
    for ( unsigned int i=0; i < args.size(); i++ ) sout << "," << args[i].cpp_type(ADAPTER_PARAM);
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
    for ( unsigned int i = 0; i < args.size(); i++ ) sout << "," << args[i].cpp_type(ADAPTER_PARAM);
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

std::map<std::string,std::string> Signal::iterator_support()
{
  std::map<std::string,std::string> need_support;
  std::pair<std::string,std::string> support;
  std::vector<Arg>::iterator a;

  for ( a = args.begin(); a != args.end(); a++ )
  {
    if ( a->need_iterator_support() )
    {
      support = a->iterator_support();
      std::map<std::string,std::string> tmp;
      tmp[support.first] = support.second;
      merge(need_support, tmp);
    }
  }
  
  return need_support;
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

std::vector< std::string > Interface::cpp_adapter_methods_signals_create()
{
  std::vector<std::string> strings;
  for ( unsigned int i = 0; i < methods.size(); i++ ) {
    methods[i].interface = this;
    if ( methods[i].args_valid() )
      strings.push_back( methods[i].cpp_adapter_create() );
  }
  for ( unsigned int i = 0; i < signals.size(); i++ ) {
    signals[i].interface = this;
    if ( signals[i].args_valid() )
      strings.push_back( signals[i].cpp_adapter_create() );
  }
  return strings;
}

std::vector< std::string > Interface::cpp_proxy_methods_signals()
{
  std::vector<std::string> strings;
  for ( unsigned int i = 0; i < methods.size(); i++ ) {
    methods[i].interface = this;
    strings.push_back( methods[i].cpp_proxy_method() );
  }
  for ( unsigned int i = 0; i < signals.size(); i++ ) {
    signals[i].interface = this;
    strings.push_back( signals[i].cpp_proxy_accessor() );
  }
  return strings;
}

std::vector< std::string > Interface::cpp_declare_proxy_objects()
{
  std::vector<std::string> strings;
  for ( unsigned int i = 0; i < methods.size(); i++ ) {
    methods[i].interface = this;
    if ( methods[i].args_valid() )
      strings.push_back( methods[i].cpp_declare_proxy() );
  }
  for ( unsigned int i = 0; i < signals.size(); i++ ) {
    signals[i].interface = this;
    if ( signals[i].args_valid() )
      strings.push_back( signals[i].cpp_declare_proxy() );
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

std::map<std::string,std::string> Interface::iterator_support()
{
  std::map<std::string,std::string> need_support, current;
  std::vector<Method>::iterator m;
  std::vector<Signal>::iterator s;

  for ( m = methods.begin(); m != methods.end(); m++ )
  {
    current = m->iterator_support();
    merge( need_support, current );
  }
  
  for ( s = signals.begin(); s != signals.end(); s++ )
  {
    current = s->iterator_support();
    merge( need_support, current );
  }
  
  return need_support;
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

std::map<std::string,std::string> Node::iterator_support()
{
  std::map<std::string,std::string> need_support, current;
  std::vector<Interface>::iterator i;

  for ( i = interfaces.begin(); i != interfaces.end(); i++ )
  {
    current = i->iterator_support();
    merge( need_support, current );
  }
  
  return need_support;
}

std::vector< std::string > Node::other_proxy_parents()
{
  std::vector<std::string> result;
  size_t last = 0;
  for ( size_t i=0; i < other_proxy_parents_str.size(); i++ )
  {
    if ( i != 0 and other_proxy_parents_str[i] == ';' )
    {
      std::string parent = other_proxy_parents_str.substr(last, i);
      result.push_back(parent);
      last = i+1;
    }
  }
  return result;
}

std::vector< std::string > Node::other_proxy_parent_includes()
{
  std::vector<std::string> result;
  size_t last = 0;
  for ( size_t i=0; i < other_proxy_parent_includes_str.size(); i++ )
  {
    if ( i != 0 and other_proxy_parent_includes_str[i] == ';' )
    {
      std::string incl = other_proxy_parent_includes_str.substr(last, i);
      result.push_back(incl);
      last = i+1;
    }
  }
  return result;
}

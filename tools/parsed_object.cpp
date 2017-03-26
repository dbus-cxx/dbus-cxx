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
#include "dbus_signal.h"

#include <cstring>
#include <algorithm>
#include <cctype>
#include <dbus-cxx/types.h>


/*
static inline std::string dbuscxx_type_from_type( DBus::Type t ){
 switch ( t )
    {
      case TYPE_BYTE:
        return "byte";
      case TYPE_BOOLEAN:
        return "boolean";
      case TYPE_INT16:
        return "int16_t";
      case TYPE_UINT16:
        return "uint16_t";
      case TYPE_INT32:
        return "int32_t";
      case TYPE_UINT32:
        return "uint32_t";
      case TYPE_INT64:
        return "int64_t";
      case TYPE_UINT64:
        return "uint64_t";
      case TYPE_DOUBLE:
        return "double";
      case TYPE_STRING:
        return "std::string";
      case TYPE_OBJECT_PATH:
        return "DBus::Path";
      case TYPE_SIGNATURE:
        return "DBus::Signature";
      case TYPE_ARRAY:
        return "std::vector";
      case TYPE_VARIANT:
       return "Variant";
      case TYPE_STRUCT:
        return "Struct";
      case TYPE_DICT_ENTRY:
        return "Dict Entry";
      default:
        break;
    }
    return std::string("Invalid");

}
*/

std::string cppTypeFromSignature( DBus::Signature sig ){
  DBus::Signature::iterator sigIter = sig.begin();
  std::string returnVal = "";
 
  while( sigIter.is_valid() ){
    std::cout << ((char)sigIter.type()) << std::endl;
    //returnVal += cpp_dbus_type( sigIter.type() );
    sigIter.next();
  }

  return returnVal;
}


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
    methods[i].set_interface( this );
    if ( methods[i].args_valid() )
      strings.push_back( methods[i].cpp_adapter_create() );
  }
  for ( unsigned int i = 0; i < signals.size(); i++ ) {
    signals[i].set_interface( this );
    if ( signals[i].args_valid() )
      strings.push_back( signals[i].cpp_adapter_create() );
  }
  return strings;
}

std::vector< std::string > Interface::cpp_proxy_methods_signals()
{
  std::vector<std::string> strings;
  for ( unsigned int i = 0; i < methods.size(); i++ ) {
    methods[i].set_interface( this );
    strings.push_back( methods[i].cpp_proxy_method() );
  }
  for ( unsigned int i = 0; i < signals.size(); i++ ) {
    signals[i].set_interface( this );
    strings.push_back( signals[i].cpp_proxy_accessor() );
  }
  return strings;
}

std::vector< std::string > Interface::cpp_declare_proxy_objects()
{
  std::vector<std::string> strings;
  for ( unsigned int i = 0; i < methods.size(); i++ ) {
    methods[i].set_interface( this );
    if ( methods[i].args_valid() )
      strings.push_back( methods[i].cpp_declare_proxy() );
  }
  for ( unsigned int i = 0; i < signals.size(); i++ ) {
    signals[i].set_interface( this );
    if ( signals[i].args_valid() )
      strings.push_back( signals[i].cpp_declare_proxy() );
  }
  return strings;
}

std::vector< std::string > Interface::cpp_adapter_creation()
{
  std::vector<std::string> strings;
  for ( unsigned int i = 0; i < methods.size(); i++ ) {
    methods[i].set_interface( this );
    if ( methods[i].args_valid() )
    {
      strings.push_back( methods[i].cpp_adapter_creation() );
      std::vector<std::string> names = methods[i].adapter_arg_names();
      strings.insert(strings.end(), names.begin(), names.end());
    }
  }
  for ( unsigned int i = 0; i < signals.size(); i++ ) {
    signals[i].set_interface( this );
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
    signals[i].set_interface( this );
    if ( signals[i].args_valid() )
      strings.push_back( signals[i].adapter_signal_connect() );
  }
  return strings;
}

std::vector< std::string > Interface::cpp_adapter_signal_disconnection()
{
  std::vector<std::string> strings;
  for ( size_t i = 0; i < signals.size(); i++ ) {
    signals[i].set_interface( this );
    if ( signals[i].args_valid() )
      strings.push_back( signals[i].adapter_signal_disconnect() );
  }
  return strings;
}

std::vector< std::string > Interface::cpp_adapter_stubs()
{
  std::vector<std::string> strings;
  for ( unsigned int i = 0; i < methods.size(); i++ ) {
    methods[i].set_interface( this );
    if ( methods[i].args_valid() )
      strings.push_back( methods[i].cpp_adapter_stub() );
  }
  for ( unsigned int i = 0; i < signals.size(); i++ ) {
    signals[i].set_interface( this );
    if ( signals[i].args_valid() and not signals[i].get_ignored() )
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
  std::vector<DBusSignal>::iterator s;

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
    result += "namespace ";
    if ( ns[i].size() == 0 ) result += "DBusGenerated";
    else result += ns[i];
    result += " {\n";
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
  if ( name().empty() ) 
    return "AdapterName";
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


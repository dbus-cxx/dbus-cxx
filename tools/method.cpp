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
#include "method.h"
#include "parsed_object.h"

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
    if ( mis_virtual ) sout << "virtual ";
    if ( out_args.size() == 0 )
      sout << "void";
    else
      sout << out_args[0].cpp_type(PROXY_RET);
    sout << " " << get_name() << "(";
    for ( unsigned int i = 0; i < in_args.size(); i++ ) {
      sout << (( i==0 )?" ":", " ) << in_args[i].cpp_type(PROXY_PARAM) << " ";
      if( in_args[i].name().size() > 0 ){
        sout << in_args[i].name();
      } else {
        sout << "arg" << (char)(i + 48) << " ";
      }
    }
    sout << " )";
    if ( mis_const ) sout << " const";
    sout << " { return ";
    sout << "(*" << varname() << ")(";
    for ( unsigned int i = 0; i < in_args.size(); i++ ){
      sout << (( i==0 )?" ":", " );
      if( in_args[i].name().size() > 0 ){
        sout << in_args[i].name();
      } else {
        sout << "arg" << (char)(i + 48) << " ";
      }
    }
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
    sout << "temp_method->set_arg_name( " << 0 << ", \"" << out_args[0].name() << "\" );";
    decls.push_back(sout.str());
    sout.str("");
  }
  for ( size_t i = 0; i < in_args.size(); i++ )
  {
    sout << "temp_method->set_arg_name( " << i+1 << ", \"" << in_args[i].name() << "\" );";
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

std::string Method::get_name() { 
  if ( cppname.empty() ) 
    return name; 
  return cppname; 
}

void Method::set_interface( Interface* iface ){
  interface = iface;
}

void Method::set_name( std::string nm ){
  name = nm;
}

void Method::set_cppname( std::string name ){
  cppname = name;
}

void Method::set_const( bool con ){
  mis_const = con;
}

void Method::set_virtual( bool virt ){
  mis_virtual = virt;
}

bool Method::is_const(){
  return mis_const;
}

bool Method::is_virtual() {
  return mis_virtual;
}

void Method::push_in_arg( Arg a ){
  in_args.push_back( a );
}

void Method::push_out_arg( Arg a ){
  out_args.push_back( a );
}

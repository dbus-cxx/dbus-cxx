#include "parsed_object.h"

std::string DBusSignal::cpp_adapter_create()
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

std::vector<std::string> DBusSignal::proxy_arg_names()
{
  std::vector<std::string> decls;
  std::ostringstream sout;
  for ( size_t i = 0; i < args.size(); i++ )
  {
    sout << varname() << "->set_arg_name( " << i << ", \"" << args[i].name() << "\" );";
    decls.push_back(sout.str());
    sout.str("");
  }
  return decls;
}

std::string DBusSignal::cpp_proxy_accessor()
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

std::string DBusSignal::cpp_declare_proxy()
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

bool DBusSignal::args_valid()
{
  if ( args.size() > 7 ) return false;

  for ( unsigned int i=0; i < args.size(); i++ )
    if ( args[i].type() == DBus::TYPE_INVALID ) return false;

  return true;
}

std::string DBusSignal::strfmt( int depth )
{
  std::ostringstream sout;
  out_tabs( sout, depth );
  sout << "Signal: " << name << std::endl;
  for ( unsigned int i=0; i < args.size(); i++ ) sout << args[i].strfmt( depth+1 );
  return sout.str();
}

std::string DBusSignal::adapter_signal_create()
{
  std::ostringstream sout;
  if ( args_valid() ) {
    sout << adapter_name() << " = this->create_signal<void";
    for ( unsigned int i=0; i < args.size(); i++ ) sout << "," << args[i].cpp_type(ADAPTER_PARAM);
    sout << ">(\"" << interface->name() << "\",\"" << name << "\");";
  }
  return sout.str();
}

std::vector<std::string> DBusSignal::adapter_arg_names()
{
  std::vector<std::string> decls;
  std::ostringstream sout;
  for ( size_t i = 0; i < args.size(); i++ )
  {
    sout << adapter_name() << "->set_arg_name( " << i << ", \"" << args[i].name() << "\" );";
    decls.push_back(sout.str());
    sout.str("");
  }
  return decls;
}

std::string DBusSignal::adapter_signal_declare()
{
  std::ostringstream sout;
  if ( args_valid() ) {
    sout << "::DBus::signal<void";
    for ( unsigned int i = 0; i < args.size(); i++ ) sout << "," << args[i].cpp_type(ADAPTER_PARAM);
    sout << ">::pointer " << adapter_name() << ";";
  }
  return sout.str();
}

std::string DBusSignal::adapter_signal_connect()
{
  if ( accessor.empty() ) return "";
  std::ostringstream sout;
  sout << "if ( m_adaptee ) " << adapter_conn_name() << " = m_adaptee->" << accessor << ".connect( *" << adapter_name() << " );";
  return sout.str();
}

std::map<std::string,std::string> DBusSignal::iterator_support()
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

void DBusSignal::set_interface( Interface* iface ){
  interface = iface;
}

std::string DBusSignal::varname() { 
  return "m_signal_" + name; 
}

std::string DBusSignal::adapter_name() { 
  return "m_signal_adapter_" + name; 
}

std::string DBusSignal::adapter_conn_name() { 
  return "m_signal_adapter_connection_" + name; 
}

std::string DBusSignal::adapter_signal_conn_declare() { 
  if (args_valid()) 
    return "sigc::connection " + adapter_conn_name() + ";"; 
  return ""; 
}

std::string DBusSignal::adapter_signal_disconnect() { 
  return adapter_conn_name() + ".disconnect();"; 
}

int DBusSignal::get_ignored() {
  return ignored;
}

void DBusSignal::set_name( std::string nm ){
  name = nm;
}

void DBusSignal::set_ignored( int ignore ){
  ignored = ignore;
}


void DBusSignal::set_accessor( std::string acc ){
  accessor = acc;
}

void DBusSignal::push_arg( Arg a ){
  args.push_back( a );
}

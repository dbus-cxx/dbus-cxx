/***************************************************************************
 *   Copyright (C) 2007,2008,2009 by Rick L. Vinyard, Jr.                  *
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
#include <popt.h>
#include <sstream>
#include <fstream>
#include <dbus-cxx.h>
#include <cstring>
#include <expat.h>
#include <stack>
#include <cppgenerate/class.h>

#include "xml_parser.h"
#include "parsed_object.h"
#include "generate_adapter.h"
#include "generate_proxy.h"

static std::stack<std::string> tagStack;
static std::vector<cppgenerate::Class> classes;
static std::string currentInterface;
static cppgenerate::Method currentMethod;

static void start_element_handler(void* user_data, const XML_Char* name, const XML_Char** attrs ){
    std::string tagName = std::string( name );
    std::map<std::string,std::string> tagAttrs;
    int i = 0;

    while( attrs[ i ] != NULL ){
        std::string attrKey( attrs[ i ] );
        std::string attrValue( attrs[ i + 1 ] );

        tagAttrs.insert( std::make_pair( attrKey, attrValue ) );

        i += 2;
    }

    tagStack.push( tagName );

    if( tagName.compare( "node" ) == 0 ){
        cppgenerate::Class newclass;
        std::string dest;
        std::string path;

        if( tagAttrs.find( "gen-namespace" ) != tagAttrs.end() ){
            newclass.setNamespace( tagAttrs.find( "gen-namespace" )->second );
        }

        if( tagAttrs.find( "dest" ) != tagAttrs.end() ){
            dest = "\"" + tagAttrs[ "dest" ] + "\"";
        }else{
            std::cerr << "WARNING: Did not find 'dest' in XML for node" << std::endl;
        }

        if( tagAttrs.find( "path" ) != tagAttrs.end() ){
            path = "\"" + tagAttrs[ "path" ] + "\"";
        }else{
            std::cerr << "WARNING: Did not find 'path' in xml for node" << std::endl;
        }

        if( tagAttrs.find( "cppname" ) != tagAttrs.end() ){
            newclass.setName( tagAttrs[ "cppname" ] + "Proxy" );
        }else{
            newclass.setName( "NONAME" );
        }


        newclass.addSystemInclude( "dbus-cxx.h" )
          .addParentClass( "DBus::ObjectProxy", cppgenerate::AccessModifier::PUBLIC, "conn, dest, path" )
          .addConstructor( cppgenerate::Constructor::create()
            .addArgument( cppgenerate::Argument::create()
              .setType( "DBus::Connection::Pointer" )
              .setName( "conn" ) )
            .addArgument( cppgenerate::Argument::create()
              .setType( "std::string" )
              .setName( "dest" )
              .setDefaultValue( dest ) )
            .addArgument( cppgenerate::Argument::create()
              .setType( "std::string" )
              .setName( "path" )
              .setDefaultValue( path ) ) );
              
        classes.push_back( newclass );
    }else if( tagName.compare( "interface" ) == 0 ){
        if( tagAttrs.find( "name" ) == tagAttrs.end() ){
            std::cerr << "WARNING: No name for interface found" << std::endl;
            return;
        }
        currentInterface = tagAttrs[ "name" ];
    }else if( tagName.compare( "method" ) == 0 ){
        if( tagAttrs.find( "name" ) == tagAttrs.end() ){
            std::cerr << "WARNING: No name for method found" << std::endl;
            return;
        }
        currentMethod = cppgenerate::Method().setName( tagAttrs[ "name" ] );
    }else if( tagName.compare( "arg" ) == 0 ){
        cppgenerate::Argument arg;

        if( tagAttrs.find( "direction" ) == tagAttrs.end() ){
            //XML_GetCurrentLineNumber
            std::cerr << "WARNING: No direction for arg found(assuming in)." << std::endl;
        }

        DBus::Signature signature( tagAttrs[ "type" ] );

        DBus::Signature::iterator it = signature.begin();
        //typestr = type_string_from_code( it.type() );

        arg.setName( tagAttrs[ "name" ] )
           .setType( type_string_from_code( it.type() ) );

        if( tagAttrs[ "direction" ] == "out" ){
            currentMethod.setReturnType( type_string_from_code( it.type() ) );
        }else{
            currentMethod.addArgument( arg );
        }
    }
}

void end_element_handler( void* userData, const XML_Char* name ){
    std::string tagName = std::string( name );

    tagStack.pop();

    if( tagName == "method" ){
        /* Add a method to the class, along with its implementation
         * to actually call over the bus
         */
        cppgenerate::MemberVariable memberVar;
        std::vector<cppgenerate::Argument> args = currentMethod.arguments();
        std::string dbusSig;
        /* method proxy type = template args */
        std::string methodProxyType;
        /* methodArguments = arguments to proxy method */
        std::string methodArguments;
        bool argumentComma = false;

        methodProxyType += "DBus::MethodProxy<" + currentMethod.returnType();
        for( cppgenerate::Argument arg : args ){
            methodProxyType += ",";
            methodProxyType += arg.type();

            if( argumentComma ) methodArguments += ",";
            methodArguments += arg.name();
            argumentComma = true;
        }
        methodProxyType += ">";

        memberVar.setAccessModifier( cppgenerate::AccessModifier::PROTECTED )
                 .setName( "m_method_" + currentMethod.name() )
                 .setType( methodProxyType );

        currentMethod.addCode( cppgenerate::CodeBlock::create()
            .addLine( "return (*" + memberVar.name() + ")(" +  methodArguments + ");" ) );

        classes.data()[ classes.size() - 1 ]
            .addMethod( currentMethod )
            .addMemberVariable( memberVar );
    }
}

static void parse_new( const std::string& xml ){
  XML_Parser parser = XML_ParserCreate(NULL);

  XML_SetElementHandler( parser, start_element_handler, end_element_handler );

  XML_Parse( parser, xml.c_str(), xml.size(), 1 );

  XML_ParserFree(parser);


    for( cppgenerate::Class c : classes ){
        c.print( std::cout, std::cout );
    }
static std::vector<cppgenerate::Class> classes;
}

int main( int argc, const char** argv )
{

  poptContext opt_context;
  int verbose=0;
  int make_proxy=0;
  int make_adapter=0;
  int output_to_file = 0;
  const char* xml_file=NULL;
  const char* file_prefix = "";
  char c;

  struct poptOption option_table[] = {
    { "xml",          'x', POPT_ARG_STRING, &xml_file,       0, "The file containing the XML specification" },
    { "prefix",       'p', POPT_ARG_STRING, &file_prefix,    0, "A prefix to place on all output files" },
    { "file",         'f', POPT_ARG_NONE,   &output_to_file, 0, "Output to files [default=no]" },
    { "proxy",          0, POPT_ARG_NONE,   &make_proxy,     0, "Make a proxy class for the specification.  "
         "Proxies are used when you want to talk with a DBus service [default=no]" },
    { "adapter",        0, POPT_ARG_NONE,   &make_adapter,   0, "Make an adapter class for the specification.  "
         "Adapters are used when you want to implement a service [default=no]" },
    { "verbose",        0, POPT_ARG_NONE,   &verbose,        0, "Verbose printing of output" },
    POPT_AUTOHELP
    { NULL, 0, 0, NULL, 0 }
  };

  opt_context = poptGetContext(NULL, argc, argv, option_table, 0 );

  while ( (c = poptGetNextOpt(opt_context)) >= 0) { }

  if ( not (poptPeekArg(opt_context) == NULL) or not (make_proxy or make_adapter) or not xml_file )
  {
    poptPrintUsage(opt_context, stderr, 0);
    return 1;
  }

  poptFreeContext(opt_context);

  std::string specification;

    // open the file and load into specification
    std::ifstream fin;
    char buffer[1025];
    size_t n;
    fin.open( xml_file );
    if( fin.fail() ){
      std::cerr << "ERROR: Can't open file " << xml_file << std::endl;
      return 1;
    }
    while ( ! fin.eof() )
    {
      fin.read(buffer, 1024);
      n = fin.gcount();
      if ( n > 0 )
      {
        buffer[n] = '\0';
        specification += buffer;
      }
    }
    fin.close();

    parse_new( specification );
return 0;

    Nodes nodes;

    nodes = parse_xml( specification );

    std::string proxy_h, proxy_cpp, adapter_h, adapter_cpp, filename;
    std::ofstream fout;

//     std::cout << "Nodes: " << nodes.size() << std::endl;

//     std::cout << "Parsed" << std::endl;

    for ( unsigned i = 0; i < nodes.size(); i++ )
    {
      proxy_h.clear();
      proxy_cpp.clear();
      adapter_h.clear();
      adapter_cpp.clear();

//       std::cout << nodes[i].strfmt() << std::endl;

      if ( make_proxy )
      {
        proxy_h = generate_proxy_h(nodes[i]);
        proxy_cpp = generate_proxy_cpp(nodes[i]);
      }

      if ( make_adapter )
      {
        adapter_h = generate_adapter_h(nodes[i]);
        adapter_cpp = generate_adapter_cpp(nodes[i]);
      }

      if ( output_to_file )
      {
        std::string full_file_prefix = file_prefix;
        full_file_prefix += nodes[i].file_prefix;
        
        if ( not proxy_h.empty() )
        {
          filename = full_file_prefix + nodes[i].name_lower() + "_proxy.h";
          fout.open( filename.c_str(), std::ios_base::trunc );
          fout << proxy_h;
          fout.close();
        }
        if ( not proxy_cpp.empty() )
        {
          filename = full_file_prefix + nodes[i].name_lower() + "_proxy.cpp";
          fout.open( filename.c_str(), std::ios_base::trunc );
          fout << proxy_cpp;
          fout.close();
        }
        if ( not adapter_h.empty() )
        {
          filename = full_file_prefix + nodes[i].name_lower() + "_adapter.h";
          fout.open( filename.c_str(), std::ios_base::trunc );
          fout << adapter_h;
          fout.close();
        }
        if ( not adapter_cpp.empty() )
        {
          filename = full_file_prefix + nodes[i].name_lower() + "_adapter.cpp";
          fout.open( filename.c_str(), std::ios_base::trunc );
          fout << adapter_cpp;
          fout.close();
        }
      }
      else
      {
        if ( not proxy_h.empty() )     std::cout << proxy_h     << std::endl;
        if ( not proxy_cpp.empty() )   std::cout << proxy_cpp   << std::endl;
        if ( not adapter_h.empty() )   std::cout << adapter_h   << std::endl;
        if ( not adapter_cpp.empty() ) std::cout << adapter_cpp << std::endl;
      }
    }

	return 0;
}

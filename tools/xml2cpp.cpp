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
#include "code-generator.h"


int main( int argc, const char** argv )
{

  poptContext opt_context;
  int verbose=0;
  int make_proxy=0;
  int make_adapter=0;
  int output_to_file = 0;
  const char* xml_file=NULL;
  const char* file_prefix = "";
  const char* output_dir = ".";
  char c;
  DBus::CodeGenerator generator;
  std::string outputDirString;

  struct poptOption option_table[] = {
    { "xml",          'x', POPT_ARG_STRING, &xml_file,       0, "The file containing the XML specification" },
    //{ "prefix",       'p', POPT_ARG_STRING, &file_prefix,    0, "A prefix to place on all output files" },
    { "file",         'f', POPT_ARG_NONE,   &output_to_file, 0, "Output to files [default=no]" },
    { "output_dir",       'o', POPT_ARG_STRING, &output_dir,    0, 
         "The output directory for files(only if outputting to files)[default=.]" },
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

    generator.setXML( specification );

    if( !generator.parse() ){
        return 1;
    }

    outputDirString = std::string( output_dir );
    if( outputDirString.back() != '/' ){
        outputDirString += "/";
    }

    if( make_proxy ){
        generator.generateProxyClasses( output_to_file, outputDirString );
    }

    if( make_adapter ){
        generator.generateAdapterClasses( output_to_file, outputDirString );
    }
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

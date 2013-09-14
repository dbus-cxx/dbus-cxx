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
#include "xml_parser.h"

#include <expat.h>

#include <cstring>

// TODO fix parser bug when node is part of an interface

typedef enum XMLTag {
  TAG_ARG,
  TAG_METHOD,
  TAG_SIGNAL,
  TAG_INTERFACE,
  TAG_NODE,
  TAG_UNKNOWN
} XMLNode;

const char* xml_tag_strings[] = {
  "arg",
  "method",
  "signal",
  "interface",
  "node"
};

XMLTag xml_tag( const char* name )
{
  for ( int i=0; i < TAG_UNKNOWN; i++ )
    if ( strcasecmp(name, xml_tag_strings[i]) == 0 ) return (XMLTag)i;
  return TAG_UNKNOWN;
}

typedef enum XMLArgAttr { ARG_NAME, ARG_DIR, ARG_TYPE, ARG_CPPTYPE, ARG_CONST, ARG_REF, ARG_UNKNOWN } XMLArgAttr;
const char* xml_arg_strings[] = { "name", "direction", "type", "cpptype" /*extended*/, "const" /*extended*/, "ref" /*extended*/ };
XMLArgAttr xml_arg_attr( const char* name )
{
  for ( int i=0; i < ARG_UNKNOWN; i++ )
    if ( strcasecmp(name, xml_arg_strings[i]) == 0 ) return (XMLArgAttr)i;
  return ARG_UNKNOWN;
}

typedef enum XMLMethodAttr { METHOD_NAME, METHOD_CPPNAME, METHOD_CONST, METHOD_VIRTUAL, METHOD_UNKNOWN } XMLMethodAttr;
const char* xml_method_strings[] = { "name", "cppname" /*extended*/, "const" /*extended*/, "virtual" /*extended*/};
XMLMethodAttr xml_method_attr( const char* name )
{
  for ( int i=0; i < METHOD_UNKNOWN; i++ )
    if ( strcasecmp(name, xml_method_strings[i]) == 0 ) return (XMLMethodAttr)i;
  return METHOD_UNKNOWN;
}

typedef enum XMLSignalAttr { SIGNAL_NAME, SIGNAL_ACCESSOR, SIGNAL_IGNORED, SIGNAL_UNKNOWN } XMLSignalAttr;
const char* xml_signal_strings[] = { "name", "accessor" /*extended*/, "ignored" /*extended*/ };
XMLSignalAttr xml_signal_attr( const char* name )
{
  for ( int i=0; i < SIGNAL_UNKNOWN; i++ )
    if ( strcasecmp(name, xml_signal_strings[i]) == 0 ) return (XMLSignalAttr)i;
  return SIGNAL_UNKNOWN;
}

typedef enum XMLInterfaceAttr { INTERFACE_NAME, INTERFACE_IGNORED, INTERFACE_CPPPREFIX, INTERFACE_UNKNOWN } XMLInterfaceAttr;
const char* xml_interface_strings[] = { "name", "ignored" /*extended*/, "cppprefix" /*extended*/ };
XMLInterfaceAttr xml_interface_attr( const char* name )
{
  for ( int i=0; i < INTERFACE_UNKNOWN; i++ )
    if ( strcasecmp(name, xml_interface_strings[i]) == 0 ) return (XMLInterfaceAttr)i;
  return INTERFACE_UNKNOWN;
}

typedef enum XMLNodeAttr {
  NODE_NAME, NODE_CPPNAME,
  NODE_GEN_NAMESPACE, NODE_ORIG_NAMESPACE,
  NODE_DBUS_DEST, NODE_DBUS_PATH,
  NODE_CPPINCLUDE, NODE_FILE_PREFIX, NODE_IGNORED,
  NODE_ADAPTER_PARENT, NODE_ADAPTER_PARENT_INCLUDE,
  NODE_PROXY_PARENT, NODE_PROXY_PARENT_INCLUDE,
  NODE_OTHER_PROXY_PARENTS, NODE_OTHER_PROXY_PARENT_INCLUDES,
  CHILDNODE_ADAPTER_INCLUDE, CHILDNODE_ADAPTER,
  CHILDNODE_PROXY_INCLUDE, CHILDNODE_PROXY,
  CHILDNODE_ACCESSOR, CHILDNODE_ADAPTEE_ACCESSOR,
  NODE_UNKNOWN
} XMLNodeAttr;

const char* xml_node_strings[] = {
  "name", "cppname" /*extended*/,
  "gen-namespace"  /*extended*/, "orig-namespace" /*extended*/,
  "dest" /*extended*/, "path" /*extended*/,
  "cppinclude" /*extended*/, "file-prefix" /*extended*/, "ignored" /*extended*/,
  "adapter-parent" /*extended*/, "adapter-parent-include" /*extended*/,
  "proxy-parent" /*extended*/, "proxy-parent-include" /*extended*/,
  "other-proxy-parents" /*extended*/, "other-proxy-parent-includes" /*extended*/,
  "adapter-include" /*extended*/, "adapter" /*extended*/,
  "proxy-include" /*extended*/, "proxy" /*extended*/,
  "accessor" /*extended*/, "adaptee-accessor" /*extended*/
};

XMLNodeAttr xml_node_attr( const char* name )
{
  for ( int i=0; i < NODE_UNKNOWN; i++ )
    if ( strcasecmp(name, xml_node_strings[i]) == 0 ) return (XMLNodeAttr)i;
  return NODE_UNKNOWN;
}

static std::deque<XMLTag> tag_stack;
static Arg current_arg;
static Method current_method;
static bool use_current_method;
static DBusSignal current_signal;
static Interface current_interface;
static std::deque<Node> node_stack;


void start_element_handler( void* userData, const XML_Char* name, const XML_Char** attrs )
{
  const XML_Char** ptr = attrs;

  const XML_Char* ptrvaluestring;

  XMLTag current_tag = xml_tag(name);

  tag_stack.push_front(current_tag);

//   std::cout << "Start Tag: " << xml_tag_strings[current_tag] << std::endl;

  switch ( current_tag )
  {
    case TAG_ARG:
      current_arg = Arg();
      
      while ( *ptr != NULL )
      {
        ptrvaluestring = *(ptr+1);
        switch ( xml_arg_attr( *ptr ) )
        {
          case ARG_NAME:    current_arg.set_dbus_name( ptrvaluestring );          break;
          case ARG_DIR:     current_arg.set_direction_string( ptrvaluestring );   break;
          case ARG_TYPE:    current_arg.set_signature( ptrvaluestring );          break;
          case ARG_CPPTYPE: current_arg.set_cpp_type_override( ptrvaluestring );  break;
          case ARG_CONST:   current_arg.set_const( atoi(ptrvaluestring) );     break;
          case ARG_REF:     current_arg.set_ref( atoi(ptrvaluestring) );       break;
          default:                                                           break;
        }
        ptr += 2;
      }
      break;
    case TAG_METHOD:
      current_method = Method();
      
      while ( *ptr != NULL )
      {
        ptrvaluestring = *(ptr+1);
        switch ( xml_method_attr( *ptr ) )
        {
          case METHOD_NAME:    current_method.set_name( ptrvaluestring );             break;
          case METHOD_CPPNAME: current_method.set_cppname( ptrvaluestring );          break;
          case METHOD_CONST:   current_method.set_const( atoi(ptrvaluestring) );   break;
          case METHOD_VIRTUAL: current_method.set_virtual( atoi(ptrvaluestring) ); break;
          default:                                                               break;
        }
        ptr += 2;
      }
      break;
    case TAG_SIGNAL:
      current_signal = DBusSignal();
      
      while ( *ptr != NULL )
      {
        ptrvaluestring = *(ptr+1);
        switch ( xml_signal_attr( *ptr ) )
        {
          case SIGNAL_NAME:     current_signal.set_name( ptrvaluestring );           break;
          case SIGNAL_ACCESSOR: current_signal.set_accessor( ptrvaluestring );       break;
          case SIGNAL_IGNORED:  current_signal.set_ignored( atoi(ptrvaluestring) );  break;
          default:                                                              break;
        }
        ptr += 2;
      }
      break;
    case TAG_INTERFACE:
      current_interface = Interface();
      
      while ( *ptr != NULL )
      {
        ptrvaluestring = *(ptr+1);
        switch ( xml_interface_attr( *ptr ) )
        {
          case INTERFACE_NAME:      current_interface.dbus_name = ptrvaluestring;      break;
          case INTERFACE_IGNORED:   current_interface.ignored = atoi(ptrvaluestring);  break;
          case INTERFACE_CPPPREFIX: current_interface.cppprefix = ptrvaluestring;      break;
          default:                                                                     break;
        }
        ptr += 2;
      }
      break;
    case TAG_NODE:
      node_stack.push_front( Node() );
      
      while ( *ptr != NULL )
      {
        ptrvaluestring = *(ptr+1);
        switch ( xml_node_attr( *ptr ) )
        {
          case NODE_NAME:                   node_stack.front().dbus_name = ptrvaluestring;                            break;
          case NODE_CPPNAME:                node_stack.front().cppname = ptrvaluestring;                              break;
          case NODE_GEN_NAMESPACE:          node_stack.front().gen_namespace = ptrvaluestring;                        break;
          case NODE_ORIG_NAMESPACE:         node_stack.front().orig_namespace = ptrvaluestring;                       break;
          case NODE_DBUS_DEST:              node_stack.front().dbus_destination = ptrvaluestring;                     break;
          case NODE_DBUS_PATH:              node_stack.front().dbus_path = ptrvaluestring;                            break;
          case NODE_CPPINCLUDE:             node_stack.front().cppinclude = ptrvaluestring;                           break;
          case NODE_FILE_PREFIX:            node_stack.front().file_prefix = ptrvaluestring;                          break;
          case NODE_IGNORED:                node_stack.front().ignored = atoi(ptrvaluestring);                        break;
          case NODE_ADAPTER_PARENT:         node_stack.front().adapter_parent = ptrvaluestring;                       break;
          case NODE_ADAPTER_PARENT_INCLUDE: node_stack.front().adapter_parent_include = ptrvaluestring;               break;
          case NODE_PROXY_PARENT:           node_stack.front().proxy_parent = ptrvaluestring;                         break;
          case NODE_PROXY_PARENT_INCLUDE:   node_stack.front().proxy_parent_include = ptrvaluestring;                 break;
          case NODE_OTHER_PROXY_PARENTS:    node_stack.front().other_proxy_parents_str = ptrvaluestring;              break;
          case NODE_OTHER_PROXY_PARENT_INCLUDES: node_stack.front().other_proxy_parent_includes_str = ptrvaluestring; break;
          case CHILDNODE_ADAPTER_INCLUDE:   node_stack.front().adapter_include = ptrvaluestring;                      break;
          case CHILDNODE_ADAPTER:           node_stack.front().adapter = ptrvaluestring;                              break;
          case CHILDNODE_PROXY_INCLUDE:     node_stack.front().proxy_include = ptrvaluestring;                        break;
          case CHILDNODE_PROXY:             node_stack.front().proxy = ptrvaluestring;                                break;
          case CHILDNODE_ACCESSOR:          node_stack.front().accessor = ptrvaluestring;                             break;
          case CHILDNODE_ADAPTEE_ACCESSOR:  node_stack.front().adaptee_accessor = ptrvaluestring;                     break;
          default:                                                                                                    break;
        }
        ptr += 2;
      }
      break;
    default:
      break;
  }
}

void end_element_handler( void* userData, const XML_Char* name )
{
  XMLTag current_tag = xml_tag(name);

//   std::cout << "End Tag:   " << xml_tag_strings[current_tag] << std::endl;
//   std::cout << "Tag stack: " << tag_stack.size() << std::endl;
//   std::cout << "Tag stack front: " << xml_tag_strings[tag_stack.front()] << std::endl;

  if (current_tag == tag_stack.front()) tag_stack.pop_front();

  if ( tag_stack.size() == 0 )
  {
    if ( current_tag == TAG_NODE )
    {
      Node current = node_stack.front();
      node_stack.pop_front();
      node_stack.front().children.push_back( current );
    }
  }

  switch ( current_tag )
  {
    case TAG_ARG:
      switch ( tag_stack.front() )
      {
        case TAG_SIGNAL: current_signal.push_arg(current_arg); break;
        case TAG_METHOD:
          switch ( current_arg.direction() )
          {
            case DIRECTION_IN:  current_method.push_in_arg(current_arg);   break;
            case DIRECTION_OUT: current_method.push_out_arg(current_arg);  break;
            default: /* we shouldn't get here, but if we are do nothing */ break;
          }
          default: /* we shouldn't get here, but if we are do nothing */ break;
      }
      break;
    case TAG_METHOD:
      switch ( tag_stack.front() )
      {
        case TAG_INTERFACE: current_interface.methods.push_back(current_method);   break;
        case TAG_NODE:      node_stack.front().methods.push_back(current_method);  break;
        default: /* we shouldn't get here, but if we are do nothing */             break;
      }
      break;
    case TAG_SIGNAL:
      switch ( tag_stack.front() )
      {
        case TAG_INTERFACE: current_interface.signals.push_back(current_signal);   break;
        case TAG_NODE:      node_stack.front().signals.push_back(current_signal);  break;
        default: /* we shouldn't get here, but if we are do nothing */             break;
      }
      break;
    case TAG_INTERFACE:
      if ( tag_stack.front() == TAG_NODE ) node_stack.front().interfaces.push_back(current_interface);
      break;
    case TAG_NODE:
      if ( tag_stack.front() == TAG_NODE )
      {
        Node current = node_stack.front();
        node_stack.pop_front();
        node_stack.front().children.push_back(current);
      }
      break;
    default:
      break;
  }
  
}

Nodes parse_xml( const std::string& xml )
{
  XML_Parser parser = XML_ParserCreate(NULL);

  XML_SetElementHandler( parser, start_element_handler, end_element_handler );

  node_stack.clear();
  node_stack.push_front( Node() );

  XML_Parse( parser, xml.c_str(), xml.size(), 1 );

  XML_ParserFree(parser);

  return node_stack.front().children;
}

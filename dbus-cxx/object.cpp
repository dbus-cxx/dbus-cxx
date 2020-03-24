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
#include "object.h"
#include <cstring>
#include <sstream>
#include <utility>
#include "callmessage.h"
#include "connection.h"
#include "dbus-cxx-private.h"
#include <dbus/dbus.h>
#include "interface.h"
#include "message.h"
#include "objectpathhandler.h"
#include "path.h"
#include <sigc++/sigc++.h>
#include "utility.h"


namespace DBus
{
  class ErrorInvalidMessageType;
  class ReturnMessage;

  Object::Object( const std::string& path, PrimaryFallback pf ):
      ObjectPathHandler( path, pf )
  {
  }

  std::shared_ptr<Object> Object::create( const std::string& path, PrimaryFallback pf )
  {
    return std::shared_ptr<Object>( new Object( path, pf ) );
  }

  Object::~ Object( )
  {
  }

  bool Object::register_with_connection(std::shared_ptr<Connection> conn)
  {
    SIMPLELOGGER_DEBUG("dbus.Object","Object::register_with_connection");
    if ( not ObjectPathHandler::register_with_connection(conn) ) return false;

    for (Interfaces::iterator i = m_interfaces.begin(); i != m_interfaces.end(); i++)
      i->second->set_connection(conn);

    for (Children::iterator c = m_children.begin(); c != m_children.end(); c++)
      c->second->register_with_connection(conn);

    return true;
  }

  const Object::Interfaces & Object::interfaces() const
  {
    return m_interfaces;
  }

  std::shared_ptr<Interface> Object::interface( const std::string & name ) const
  {
    Interfaces::const_iterator iter;

    {
      std::shared_lock lock( m_interfaces_rwlock );

      iter = m_interfaces.find( name );
    }

    if ( iter == m_interfaces.end() ) return std::shared_ptr<Interface>();

    return iter->second;
  }

  bool Object::add_interface( std::shared_ptr<Interface> interface )
  {
    bool result = true;

    if ( not interface ) return false;
    
    SIMPLELOGGER_DEBUG("dbus.Object","Object::add_interface " << interface->name() );

    {
      std::unique_lock lock( m_interfaces_rwlock );

      InterfaceSignalNameConnections::iterator i;

      i = m_interface_signal_name_connections.find(interface);

      if ( i == m_interface_signal_name_connections.end() )
      {
        m_interface_signal_name_connections[interface] = interface->signal_name_changed().connect( sigc::bind(sigc::mem_fun(*this, &Object::on_interface_name_changed), interface));

        m_interfaces.insert(std::make_pair(interface->name(), interface));

        interface->set_object(this);
      }
      else
      {
        result = false;
      }
    }

    m_signal_interface_added.emit( interface );

    // TODO allow control over this
    if ( not m_default_interface ) this->set_default_interface( interface->name() );

    SIMPLELOGGER_DEBUG("dbus.Object","Object::add_interface " << interface->name() << " successful: " << result);

    return result;
  }

  std::shared_ptr<Interface> Object::create_interface(const std::string & name)
  {
    std::shared_ptr<Interface> interface;

    interface = Interface::create(name);

    if ( this->add_interface(interface) ) return interface;

    return std::shared_ptr<Interface>();
  }

  void Object::remove_interface( const std::string & name )
  {
    Interfaces::iterator iter;
    std::shared_ptr<Interface> interface, old_default;
    InterfaceSignalNameConnections::iterator i;
    
    bool need_emit_default_changed = false;

    {
      std::unique_lock lock( m_interfaces_rwlock );
    
      iter = m_interfaces.find( name );
      if ( iter != m_interfaces.end() )
      {
        interface = iter->second;
        m_interfaces.erase(iter);
      }

      if ( interface )
      {
        i = m_interface_signal_name_connections.find(interface);
        if ( i != m_interface_signal_name_connections.end() )
        {
          i->second.disconnect();
          m_interface_signal_name_connections.erase(i);
          interface->set_object(nullptr);
        }
    
        if ( m_default_interface == interface ) {
          old_default = m_default_interface;
          m_default_interface = std::shared_ptr<Interface>();
          need_emit_default_changed = true;
        }

      }
    }

    if ( interface ) m_signal_interface_removed.emit( interface );

    if ( need_emit_default_changed ) m_signal_default_interface_changed.emit( old_default, m_default_interface );
  }

  bool Object::has_interface( const std::string & name )
  {
    Interfaces::const_iterator i;
    std::shared_lock lock( m_interfaces_rwlock );
    
    i = m_interfaces.find( name );

    return ( i != m_interfaces.end() );
  }

  std::shared_ptr<Interface> Object::default_interface() const
  {
    return m_default_interface;
  }

  bool Object::set_default_interface( const std::string& new_default_name )
  {
    Interfaces::iterator iter;
    std::shared_ptr<Interface> old_default;
    bool result = false;

    {
      std::shared_lock lock( m_interfaces_rwlock );

      iter = m_interfaces.find( new_default_name );

      if ( iter != m_interfaces.end() )
      {
        result = true;
        old_default = m_default_interface;
        m_default_interface = iter->second;
      }
    }

    if ( result ) m_signal_default_interface_changed.emit( old_default, m_default_interface );

    return result;
  }

  void Object::remove_default_interface()
  {
    if ( not m_default_interface ) return;

    std::shared_ptr<Interface> old_default = m_default_interface;
    m_default_interface = std::shared_ptr<Interface>();
    m_signal_default_interface_changed.emit( old_default, m_default_interface );
  }

  const Object::Children& Object::children() const
  {
    return m_children;
  }

  std::shared_ptr<Object> Object::child(const std::string& name) const
  {
    Children::const_iterator i = m_children.find(name);
    if ( i == m_children.end() ) return std::shared_ptr<Object>();
    return i->second;
  }

  bool Object::add_child(const std::string& name, std::shared_ptr<Object> child, bool force)
  {
    if ( not child ) return false;
    if ( not force and this->has_child(name) ) return false;
    m_children[name] = child;
    if ( m_connection ) child->register_with_connection(m_connection);
    return true;
  }

  bool Object::remove_child(const std::string& name)
  {
    Children::iterator i = m_children.find(name);
    if ( i == m_children.end() ) return false;
    m_children.erase(i);
    return true;
  }

  bool Object::has_child(const std::string& name) const
  {
    return m_children.find(name) != m_children.end();
  }

  std::string Object::introspect(int space_depth) const
  {
    std::ostringstream sout;
    std::string spaces;
    Interfaces::const_iterator i;
    Children::const_iterator c;
    for (int i=0; i < space_depth; i++ ) spaces += " ";
    sout << spaces << "<node name=\"" << this->path() << "\">\n"
         << spaces << "  <interface name=\"" << DBUS_CXX_INTROSPECTABLE_INTERFACE << "\">\n"
         << spaces << "    <method name=\"Introspect\">\n"
         << spaces << "      <arg name=\"data\" type=\"s\" direction=\"out\"/>\n"
         << spaces << "    </method>\n"
         << spaces << "  </interface>\n";
    for ( i = m_interfaces.begin(); i != m_interfaces.end(); i++ )
      sout << i->second->introspect(space_depth+2);
    for ( c = m_children.begin(); c != m_children.end(); c++ )
      sout << spaces << "  <node name=\"" << c->first << "\"/>\n";
    sout << spaces << "</node>\n";
    return sout.str();
  }

  sigc::signal< void(std::shared_ptr<Interface>) > Object::signal_interface_added()
  {
    return m_signal_interface_added;
  }

  sigc::signal< void(std::shared_ptr<Interface>)> Object::signal_interface_removed()
  {
    return m_signal_interface_removed;
  }

  sigc::signal< void(std::shared_ptr<Interface>, std::shared_ptr<Interface>)> Object::signal_default_interface_changed()
  {
    return m_signal_default_interface_changed;
  }

  HandlerResult Object::handle_message( std::shared_ptr<Connection> connection , std::shared_ptr<const Message> message )
  {
    Interfaces::iterator current, upper;
    std::shared_ptr<Interface> interface;
    HandlerResult result = HandlerResult::NOT_HANDLED;

    SIMPLELOGGER_DEBUG("dbus.Object","Object::handle_message: before call message test");

    std::shared_ptr<const CallMessage> callmessage;
    try{
      callmessage = CallMessage::create( message );
    }catch(std::shared_ptr<DBus::ErrorInvalidMessageType> err){
      return HandlerResult::NOT_HANDLED;
    }

    if ( not callmessage ) return HandlerResult::NOT_HANDLED;

    SIMPLELOGGER_DEBUG("dbus.Object","Object::handle_message: message is good (it's a call message) for interface '" << callmessage->interface() << "'");

    if ( callmessage->interface() == nullptr ){
        //If for some reason the message that we are getting has a nullptr interface, we will segfault.
        return HandlerResult::NOT_HANDLED;
    }

    // Handle the introspection interface
    if ( strcmp(callmessage->interface(), DBUS_CXX_INTROSPECTABLE_INTERFACE) == 0 )
    {
      SIMPLELOGGER_DEBUG("dbus.Object","Object::handle_message: introspection interface called");
      std::shared_ptr<ReturnMessage> return_message = callmessage->create_reply();
      std::string introspection = DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE;
      introspection += this->introspect();
      *return_message << introspection;
      connection << return_message;
      return HandlerResult::HANDLED;
    }

    std::shared_lock lock( m_interfaces_rwlock );

    current = m_interfaces.lower_bound( callmessage->interface() );

    // Do we have an interface or do we need to use the default???
    if ( current == m_interfaces.end() )
    {
      SIMPLELOGGER_DEBUG("dbus.Object","Object::handle_message: trying to handle with the default interface");
      // Do we have have a default to use, if so use it to try and handle the message
      if ( m_default_interface )
        result = m_default_interface->handle_call_message(connection, callmessage);
    }
    else
    {
      // Set up the upper limit of interfaces
      upper = m_interfaces.upper_bound( callmessage->interface() );


    SIMPLELOGGER_DEBUG("dbus.Object","Object::handle_message: before handling lower bound interface is " << current->second->name() );
    SIMPLELOGGER_DEBUG("dbus.Object","Object::handle_message: before handling upper bound interface is " << (upper->second ? upper->second->name() : ""));

      // Iterate through each interface with a matching name
      for ( ; current != upper; current++ )
      {
        SIMPLELOGGER_DEBUG("dbus.Object","Object::handle_message: trying to handle with interface " << current->second->name() );
        // If an interface handled the message unlock and return
        if ( current->second->handle_call_message(connection, callmessage) == HandlerResult::HANDLED )
        {
          result = HandlerResult::HANDLED;
          break;
        }
      }
    }

    if (result == HandlerResult::NOT_HANDLED and m_default_interface)
      result = m_default_interface->handle_call_message(connection, callmessage);

    SIMPLELOGGER_DEBUG("dbus.Object","Object::handle_message: message was " << ((result==HandlerResult::HANDLED)?"handled":"not handled"));

    return result;
  }

  void Object::on_interface_name_changed(const std::string & oldname, const std::string & newname, std::shared_ptr<Interface> interface)
  {
  
    std::unique_lock lock( m_interfaces_rwlock );

    Interfaces::iterator current, upper;
    current = m_interfaces.lower_bound(oldname);

    if ( current != m_interfaces.end() )
    {
      upper = m_interfaces.upper_bound(oldname);

      for ( ; current != upper; current++ )
      {
        if ( current->second == interface )
        {
          m_interfaces.erase(current);
          break;
        }
      }
    }

    m_interfaces.insert( std::make_pair(newname, interface) );

    InterfaceSignalNameConnections::iterator i;
    i = m_interface_signal_name_connections.find(interface);
    if ( i == m_interface_signal_name_connections.end() )
    {
      m_interface_signal_name_connections[interface] =
          interface->signal_name_changed().connect(sigc::bind(sigc::mem_fun(*this,&Object::on_interface_name_changed),interface));
    }
    
  }


}


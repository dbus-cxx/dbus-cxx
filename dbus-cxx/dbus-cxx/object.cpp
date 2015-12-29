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

#include <map>
#include <sstream>
#include <cstring>
#include <dbus/dbus.h>

namespace DBus
{

  Object::Object( const std::string& path, PrimaryFallback pf ):
      ObjectPathHandler( path, pf )
  {
    pthread_mutex_init( &m_name_mutex, NULL );
    pthread_rwlock_init( &m_interfaces_rwlock, NULL );
  }

  Object::pointer Object::create( const std::string& path, PrimaryFallback pf )
  {
    return pointer( new Object( path, pf ) );
  }

  Object::~ Object( )
  {
    pthread_mutex_destroy( &m_name_mutex );
    pthread_rwlock_destroy( &m_interfaces_rwlock );
  }

  bool Object::register_with_connection(Connection::pointer conn)
  {
    DBUS_CXX_DEBUG("Object::register_with_connection");
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

  Interface::pointer Object::interface( const std::string & name ) const
  {
    Interfaces::const_iterator iter;

    // ========== READ LOCK ==========
    pthread_rwlock_rdlock( &m_interfaces_rwlock );

    iter = m_interfaces.find( name );

    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_interfaces_rwlock );

    if ( iter == m_interfaces.end() ) return Interface::pointer();

    return iter->second;
  }

  bool Object::add_interface( Interface::pointer interface )
  {
    bool result = true;

    if ( not interface ) return false;
    
    DBUS_CXX_DEBUG("Object::add_interface " << interface->name() );

    // ========== WRITE LOCK ==========
    pthread_rwlock_wrlock( &m_interfaces_rwlock );

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

    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_interfaces_rwlock );

    m_signal_interface_added.emit( interface );

    // TODO allow control over this
    if ( not m_default_interface ) this->set_default_interface( interface->name() );

    DBUS_CXX_DEBUG("Object::add_interface " << interface->name() << " successful: " << result);

    return result;
  }

  Interface::pointer Object::create_interface(const std::string & name)
  {
    Interface::pointer interface;

    interface = Interface::create(name);

    if ( this->add_interface(interface) ) return interface;

    return Interface::pointer();
  }

  void Object::remove_interface( const std::string & name )
  {
    Interfaces::iterator iter;
    Interface::pointer interface, old_default;
    InterfaceSignalNameConnections::iterator i;
    
    bool need_emit_default_changed = false;

    // ========== WRITE LOCK ==========
    pthread_rwlock_wrlock( &m_interfaces_rwlock );
    
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
        interface->set_object(NULL);
      }
    
      if ( m_default_interface == interface ) {
        old_default = m_default_interface;
        m_default_interface = Interface::pointer();
        need_emit_default_changed = true;
      }

    }

    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_interfaces_rwlock );

    if ( interface ) m_signal_interface_removed.emit( interface );

    if ( need_emit_default_changed ) m_signal_default_interface_changed.emit( old_default, m_default_interface );
  }

  bool Object::has_interface( const std::string & name )
  {
    Interfaces::const_iterator i;
    
    // ========== READ LOCK ==========
    pthread_rwlock_rdlock( &m_interfaces_rwlock );

    i = m_interfaces.find( name );

    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_interfaces_rwlock );

    return ( i != m_interfaces.end() );
  }

  Interface::pointer Object::default_interface() const
  {
    return m_default_interface;
  }

  bool Object::set_default_interface( const std::string& new_default_name )
  {
    Interfaces::iterator iter;
    Interface::pointer old_default;
    bool result = false;

    // ========== READ LOCK ==========
    pthread_rwlock_rdlock( &m_interfaces_rwlock );

    iter = m_interfaces.find( new_default_name );

    if ( iter != m_interfaces.end() )
    {
      result = true;
      old_default = m_default_interface;
      m_default_interface = iter->second;
    }
    
    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_interfaces_rwlock );

    if ( result ) m_signal_default_interface_changed.emit( old_default, m_default_interface );

    return result;
  }

  void Object::remove_default_interface()
  {
    if ( not m_default_interface ) return;

    Interface::pointer old_default = m_default_interface;
    m_default_interface = Interface::pointer();
    m_signal_default_interface_changed.emit( old_default, m_default_interface );
  }

  const Object::Children& Object::children() const
  {
    return m_children;
  }

  Object::pointer Object::child(const std::string& name) const
  {
    Children::const_iterator i = m_children.find(name);
    if ( i == m_children.end() ) return Object::pointer();
    return i->second;
  }

  bool Object::add_child(const std::string& name, Object::pointer child, bool force)
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

  sigc::signal< void, Interface::pointer > Object::signal_interface_added()
  {
    return m_signal_interface_added;
  }

  sigc::signal< void, Interface::pointer > Object::signal_interface_removed()
  {
    return m_signal_interface_removed;
  }

  sigc::signal< void, Interface::pointer, Interface::pointer > Object::signal_default_interface_changed()
  {
    return m_signal_default_interface_changed;
  }

  HandlerResult Object::handle_message( Connection::pointer connection , Message::const_pointer message )
  {
    Interfaces::iterator current, upper;
    Interface::pointer interface;
    HandlerResult result = NOT_HANDLED;

    DBUS_CXX_DEBUG("Object::handle_message: before call message test");

    CallMessage::const_pointer callmessage;
    try{
      callmessage = CallMessage::create( message );
    }catch(DBusCxxPointer<DBus::ErrorInvalidMessageType> err){
      return NOT_HANDLED;
    }

    if ( not callmessage ) return NOT_HANDLED;

    DBUS_CXX_DEBUG("Object::handle_message: message is good (it's a call message) for interface '" << callmessage->interface() << "'");

    if ( callmessage->interface() == NULL ){
        //If for some reason the message that we are getting has a NULL interface, we will segfault.
        return NOT_HANDLED;
    }

    // Handle the introspection interface
    if ( strcmp(callmessage->interface(), DBUS_CXX_INTROSPECTABLE_INTERFACE) == 0 )
    {
      DBUS_CXX_DEBUG("Object::handle_message: introspection interface called");
      ReturnMessage::pointer return_message = callmessage->create_reply();
      std::string introspection = DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE;
      introspection += this->introspect();
      *return_message << introspection;
      connection << return_message;
      return HANDLED;
    }

    // ========== READ LOCK ==========
    pthread_rwlock_rdlock( &m_interfaces_rwlock );

    current = m_interfaces.lower_bound( callmessage->interface() );

    // Do we have an interface or do we need to use the default???
    if ( current == m_interfaces.end() )
    {
      DBUS_CXX_DEBUG("Object::handle_message: trying to handle with the default interface");
      // Do we have have a default to use, if so use it to try and handle the message
      if ( m_default_interface )
        result = m_default_interface->handle_call_message(connection, callmessage);
    }
    else
    {
      // Set up the upper limit of interfaces
      upper = m_interfaces.upper_bound( callmessage->interface() );


    DBUS_CXX_DEBUG("Object::handle_message: before handling lower bound interface is " << current->second->name() );
    DBUS_CXX_DEBUG("Object::handle_message: before handling upper bound interface is " << (upper->second ? upper->second->name() : ""));

      // Iterate through each interface with a matching name
      for ( ; current != upper; current++ )
      {
        DBUS_CXX_DEBUG("Object::handle_message: trying to handle with interface " << current->second->name() );
        // If an interface handled the message unlock and return
        if ( current->second->handle_call_message(connection, callmessage) == HANDLED )
        {
          result = HANDLED;
          break;
        }
      }
    }

    if (result == NOT_HANDLED and m_default_interface)
      result = m_default_interface->handle_call_message(connection, callmessage);

    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_interfaces_rwlock );

    DBUS_CXX_DEBUG("Object::handle_message: message was " << ((result==HANDLED)?"handled":"not handled"));

    return result;
  }

  void Object::on_interface_name_changed(const std::string & oldname, const std::string & newname, Interface::pointer interface)
  {
  
    // ========== WRITE LOCK ==========
    pthread_rwlock_wrlock( &m_interfaces_rwlock );

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
    
    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_interfaces_rwlock );
  }


}


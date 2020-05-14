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

static const char* LOGGER_NAME = "DBus.Object";

namespace DBus
{
  class ErrorInvalidMessageType;
  class ReturnMessage;

  typedef std::map<std::shared_ptr<Interface> ,sigc::connection> InterfaceSignalNameConnections;

  class Object::priv_data{
  public:
      priv_data(){}

      Children m_children;
      mutable std::shared_mutex m_interfaces_rwlock;
      std::mutex m_name_mutex;
      Interfaces m_interfaces;
      std::shared_ptr<Interface>  m_default_interface;
      sigc::signal<void(std::shared_ptr<Interface>,std::shared_ptr<Interface>) > m_signal_default_interface_changed;
      sigc::signal<void(std::shared_ptr<Interface>) > m_signal_interface_added;
      sigc::signal<void(std::shared_ptr<Interface>) > m_signal_interface_removed;
      InterfaceSignalNameConnections m_interface_signal_name_connections;
  };

  Object::Object( const std::string& path, PrimaryFallback pf ):
      ObjectPathHandler( path, pf ),
      m_priv( std::make_unique<priv_data>() )
  {
  }

  std::shared_ptr<Object> Object::create( const std::string& path, PrimaryFallback pf )
  {
    return std::shared_ptr<Object>( new Object( path, pf ) );
  }

  Object::~ Object( )
  {
  }

  void Object::set_connection(std::shared_ptr<Connection> conn)
  {
    SIMPLELOGGER_DEBUG(LOGGER_NAME,"Object::set_connection");
    ObjectPathHandler::set_connection(conn);

    for (Children::iterator c = m_priv->m_children.begin(); c != m_priv->m_children.end(); c++)
      c->second->set_connection(conn);
  }

  const Object::Interfaces & Object::interfaces() const
  {
    return m_priv->m_interfaces;
  }

  std::shared_ptr<Interface> Object::interface( const std::string & name ) const
  {
    Interfaces::const_iterator iter;

    {
      std::shared_lock lock( m_priv->m_interfaces_rwlock );

      iter = m_priv->m_interfaces.find( name );
    }

    if ( iter == m_priv->m_interfaces.end() ) return std::shared_ptr<Interface>();

    return iter->second;
  }

  bool Object::add_interface( std::shared_ptr<Interface> interface )
  {
    bool result = true;

    if ( not interface ) return false;
    
    SIMPLELOGGER_DEBUG(LOGGER_NAME,"Object::add_interface " << interface->name() );

    {
      std::unique_lock lock( m_priv->m_interfaces_rwlock );

      InterfaceSignalNameConnections::iterator i;

      i = m_priv->m_interface_signal_name_connections.find(interface);

      if ( i == m_priv->m_interface_signal_name_connections.end() )
      {
        m_priv->m_interfaces.insert(std::make_pair(interface->name(), interface));

        interface->set_path( path() );
      }
      else
      {
        result = false;
      }
    }

    m_priv->m_signal_interface_added.emit( interface );

    SIMPLELOGGER_DEBUG(LOGGER_NAME,"Object::add_interface " << interface->name() << " successful: " << result);

    return result;
  }

  std::shared_ptr<Interface> Object::create_interface(const std::string & name)
  {
    std::shared_ptr<Interface> interface;

    interface = Interface::create(name);

    if ( !this->add_interface(interface) ) return std::shared_ptr<Interface>();

    if ( not m_priv->m_default_interface && name.empty() ) this->set_default_interface( interface->name() );
    return interface;
  }

  void Object::remove_interface( const std::string & name )
  {
    Interfaces::iterator iter;
    std::shared_ptr<Interface> interface;
    InterfaceSignalNameConnections::iterator i;

    {
      std::unique_lock lock( m_priv->m_interfaces_rwlock );
    
      iter = m_priv->m_interfaces.find( name );
      if ( iter != m_priv->m_interfaces.end() )
      {
        interface = iter->second;
        m_priv->m_interfaces.erase(iter);
      }

      if ( interface )
      {
        i = m_priv->m_interface_signal_name_connections.find(interface);
        if ( i != m_priv->m_interface_signal_name_connections.end() )
        {
          i->second.disconnect();
          m_priv->m_interface_signal_name_connections.erase(i);
        }
      }
    }

    if ( interface ) m_priv->m_signal_interface_removed.emit( interface );
  }

  bool Object::has_interface( const std::string & name )
  {
    Interfaces::const_iterator i;
    std::shared_lock lock( m_priv->m_interfaces_rwlock );
    
    i = m_priv->m_interfaces.find( name );

    return ( i != m_priv->m_interfaces.end() );
  }

  std::shared_ptr<Interface> Object::default_interface() const
  {
    return m_priv->m_default_interface;
  }

  bool Object::set_default_interface( const std::string& new_default_name )
  {
    Interfaces::iterator iter;
    std::shared_ptr<Interface> old_default;
    bool result = false;

    {
      std::shared_lock lock( m_priv->m_interfaces_rwlock );

      iter = m_priv->m_interfaces.find( new_default_name );

      if ( iter != m_priv->m_interfaces.end() )
      {
        result = true;
        old_default = m_priv->m_default_interface;
        m_priv->m_default_interface = iter->second;
      }
    }

    if ( result ) m_priv->m_signal_default_interface_changed.emit( old_default, m_priv->m_default_interface );

    return result;
  }

  bool Object::set_default_interface( std::shared_ptr<Interface> interface ){
    if( !interface ) return false;
    m_priv->m_default_interface = interface;
    return true;
  }

  void Object::remove_default_interface()
  {
    if ( not m_priv->m_default_interface ) return;

    std::shared_ptr<Interface> old_default = m_priv->m_default_interface;
    m_priv->m_default_interface = std::shared_ptr<Interface>();
    m_priv->m_signal_default_interface_changed.emit( old_default, m_priv->m_default_interface );
  }

  const Object::Children& Object::children() const
  {
    return m_priv->m_children;
  }

  std::shared_ptr<Object> Object::child(const std::string& name) const
  {
    Children::const_iterator i = m_priv->m_children.find(name);
    if ( i == m_priv->m_children.end() ) return std::shared_ptr<Object>();
    return i->second;
  }

  bool Object::add_child(const std::string& name, std::shared_ptr<Object> child, bool force)
  {
    if ( not child ) return false;
    if ( not force and this->has_child(name) ) return false;
    std::shared_ptr<Connection> conn = connection().lock();
    if ( conn ){
        m_priv->m_children[name] = child;
        child->register_with_connection(conn);
        return true;
    }

    return false;
  }

  bool Object::remove_child(const std::string& name)
  {
    Children::iterator i = m_priv->m_children.find(name);
    if ( i == m_priv->m_children.end() ) return false;
    m_priv->m_children.erase(i);
    return true;
  }

  bool Object::has_child(const std::string& name) const
  {
    return m_priv->m_children.find(name) != m_priv->m_children.end();
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
    for ( i = m_priv->m_interfaces.begin(); i != m_priv->m_interfaces.end(); i++ )
      sout << i->second->introspect(space_depth+2);
    for ( c = m_priv->m_children.begin(); c != m_priv->m_children.end(); c++ )
      sout << spaces << "  <node name=\"" << c->first << "\"/>\n";
    sout << spaces << "</node>\n";
    return sout.str();
  }

  sigc::signal< void(std::shared_ptr<Interface>) > Object::signal_interface_added()
  {
    return m_priv->m_signal_interface_added;
  }

  sigc::signal< void(std::shared_ptr<Interface>)> Object::signal_interface_removed()
  {
    return m_priv->m_signal_interface_removed;
  }

  sigc::signal< void(std::shared_ptr<Interface>, std::shared_ptr<Interface>)> Object::signal_default_interface_changed()
  {
    return m_priv->m_signal_default_interface_changed;
  }

  HandlerResult Object::handle_message( std::shared_ptr<const Message> message )
  {
      std::shared_ptr<Connection> conn = connection().lock();
      std::shared_ptr<const CallMessage> msg;

      if( !conn ){
          SIMPLELOGGER_ERROR(LOGGER_NAME,"Object::handle_call_message: unable to handle call message: invalid connection");
          return HandlerResult::Not_Handled;
      }

      if( message->type() != MessageType::CALL ){
          SIMPLELOGGER_ERROR(LOGGER_NAME,"Object::handle_call_message: unable to handle call message: message is not call message");
          return HandlerResult::Not_Handled;
      }

      msg = std::static_pointer_cast<const CallMessage>( message );

      if ( msg->interface() == DBUS_CXX_INTROSPECTABLE_INTERFACE )
      {
        SIMPLELOGGER_DEBUG(LOGGER_NAME,"Object::handle_call_message: introspection interface called");
        std::shared_ptr<ReturnMessage> return_message = msg->create_reply();
        std::string introspection = DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE;
        introspection += this->introspect();
        *return_message << introspection;
        conn << return_message;
        return HandlerResult::Handled;
      }

      std::shared_lock lock( m_priv->m_interfaces_rwlock );
      Interfaces::iterator iface_iter;
      std::shared_ptr<Interface> interface;

      SIMPLELOGGER_DEBUG(LOGGER_NAME,"Object::handle_message: message is good (it's a call message) for interface '" << msg->interface() << "'");

      iface_iter = m_priv->m_interfaces.find( msg->interface() );

      /*
       * DBus Specification:
       *
       * In the absence of an INTERFACE field, if two or more interfaces on the same object
       * have a method with the same name, it is undefined which of those methods will be
       * invoked. Implementations may choose to either return an error, or deliver the message
       * as though it had an arbitrary one of those interfaces.
       */
      if( iface_iter == m_priv->m_interfaces.end() ){
        // Unable to find an interface to use, try to use the default
          if( m_priv->m_default_interface ){
              return m_priv->m_default_interface->handle_call_message( conn, msg );
          }

          return  HandlerResult::Invalid_Interface;
      }else{
          interface = iface_iter->second;
          return interface->handle_call_message( conn, msg );
      }
  }


}


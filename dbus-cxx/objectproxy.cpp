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
#include "objectproxy.h"
#include <utility>
#include "callmessage.h"
#include "connection.h"
#include "interfaceproxy.h"
#include <sigc++/sigc++.h>

namespace DBus
{
  class MethodProxyBase;
  class PendingCall;
  class ReturnMessage;

  class ObjectProxy::priv_data {
  public:
      priv_data( std::shared_ptr<Connection> conn, const std::string& destination, const std::string& path ) :
          m_connection(conn),
          m_destination(destination),
          m_path(path)
      {}

      std::weak_ptr<Connection> m_connection;
      std::string m_destination;
      Path m_path;
      mutable std::shared_mutex m_interfaces_rwlock;
      std::mutex m_name_mutex;
      Interfaces m_interfaces;
      sigc::signal<void(std::shared_ptr<InterfaceProxy>)> m_signal_interface_added;
      sigc::signal<void(std::shared_ptr<InterfaceProxy>)> m_signal_interface_removed;
  };

  ObjectProxy::ObjectProxy( std::shared_ptr<Connection> conn, const std::string& destination, const std::string& path ):
      m_priv( std::make_unique<priv_data>( conn, destination, path ) )
  {
  }

  std::shared_ptr<ObjectProxy> ObjectProxy::create( const std::string& path )
  {
    return std::shared_ptr<ObjectProxy>( new ObjectProxy( std::shared_ptr<Connection>(), "", path ) );
  }

  std::shared_ptr<ObjectProxy> ObjectProxy::create( const std::string& destination, const std::string& path )
  {
    return std::shared_ptr<ObjectProxy>( new ObjectProxy( std::shared_ptr<Connection>(), destination, path ) );
  }

  std::shared_ptr<ObjectProxy> ObjectProxy::create( std::shared_ptr<Connection> conn, const std::string& path )
  {
    return std::shared_ptr<ObjectProxy>( new ObjectProxy( conn, "", path ) );
  }

  std::shared_ptr<ObjectProxy> ObjectProxy::create( std::shared_ptr<Connection> conn, const std::string& destination, const std::string& path )
  {
    return std::shared_ptr<ObjectProxy>( new ObjectProxy( conn, destination, path ) );
  }

  ObjectProxy::~ ObjectProxy( )
  {
  }

  std::weak_ptr<Connection> ObjectProxy::connection() const
  {
    return m_priv->m_connection;
  }

  void ObjectProxy::set_connection( std::shared_ptr<Connection> conn )
  {
    m_priv->m_connection = conn;
    for ( Interfaces::iterator i = m_priv->m_interfaces.begin(); i != m_priv->m_interfaces.end(); i++ )
    {
      i->second->on_object_set_connection( conn );
    }
  }

  const std::string& ObjectProxy::destination() const
  {
    return m_priv->m_destination;
  }

  void ObjectProxy::set_destination( const std::string& destination )
  {
    m_priv->m_destination = destination;
  }

  const Path& ObjectProxy::path() const
  {
    return m_priv->m_path;
  }

  void ObjectProxy::set_path( const std::string& path )
  {
    m_priv->m_path = path;
    for ( Interfaces::iterator i = m_priv->m_interfaces.begin(); i != m_priv->m_interfaces.end(); i++ )
    {
      i->second->on_object_set_path( path );
    }
  }

  const ObjectProxy::Interfaces & ObjectProxy::interfaces() const
  {
    return m_priv->m_interfaces;
  }

  std::shared_ptr<InterfaceProxy> ObjectProxy::interface( const std::string & name ) const
  {
    Interfaces::const_iterator iter;

    {
      std::shared_lock lock( m_priv->m_interfaces_rwlock );

      iter = m_priv->m_interfaces.find( name );
    }

    if ( iter == m_priv->m_interfaces.end() ) return std::shared_ptr<InterfaceProxy>();

    return iter->second;
  }

  bool ObjectProxy::add_interface( std::shared_ptr<InterfaceProxy> interface )
  {
    bool result = true;

    if ( not interface ) return false;

    if ( interface->object() ) interface->object()->remove_interface( interface );
    
    {
      std::unique_lock lock( m_priv->m_interfaces_rwlock );
        m_priv->m_interfaces.insert(std::make_pair(interface->name(), interface));

        interface->set_object( this );

    }

    m_priv->m_signal_interface_added.emit( interface );

    return result;
  }

  std::shared_ptr<InterfaceProxy> ObjectProxy::create_interface(const std::string & name)
  {
    std::shared_ptr<InterfaceProxy> interface;

    interface = InterfaceProxy::create(name);

    if ( this->add_interface(interface) ) return interface;

    return std::shared_ptr<InterfaceProxy>();
  }

  void ObjectProxy::remove_interface( const std::string & name )
  {
    Interfaces::iterator iter;
    std::shared_ptr<InterfaceProxy> interface, old_default;


    {
      std::unique_lock lock( m_priv->m_interfaces_rwlock );
    
      iter = m_priv->m_interfaces.find( name );
      if ( iter != m_priv->m_interfaces.end() )
      {
        interface = iter->second;
        m_priv->m_interfaces.erase(iter);
      }

    }

    if ( interface ) m_priv->m_signal_interface_removed.emit( interface );
  }

  void ObjectProxy::remove_interface( std::shared_ptr<InterfaceProxy> interface )
  {
    Interfaces::iterator location;
    std::shared_ptr<InterfaceProxy> old_default;

    bool interface_removed = false;

    if ( not interface ) return;

    {
      std::unique_lock lock( m_priv->m_interfaces_rwlock );

      location = m_priv->m_interfaces.find( interface->name() );
      if( location != m_priv->m_interfaces.end() ){
          m_priv->m_interfaces.erase( location );
          interface_removed = true;
      }

    }

    if ( interface_removed ) m_priv->m_signal_interface_removed.emit( interface );
  }

  bool ObjectProxy::has_interface( const std::string & name ) const
  {
    Interfaces::const_iterator i;
    std::shared_lock lock( m_priv->m_interfaces_rwlock );

    i = m_priv->m_interfaces.find( name );

    return ( i != m_priv->m_interfaces.end() );
  }

  bool ObjectProxy::has_interface( std::shared_ptr<InterfaceProxy> interface ) const
  {
    if ( not interface ) return false;
    
    Interfaces::const_iterator current, upper;
    bool result = false;
    std::shared_lock lock( m_priv->m_interfaces_rwlock );

    current = m_priv->m_interfaces.lower_bound(interface->name());

    if ( current != m_priv->m_interfaces.end() )
    {
      upper = m_priv->m_interfaces.upper_bound(interface->name());
      for ( ; current != upper; current++)
      {
        if ( current->second == interface )
        {
          result = true;
          break;
        }
      }
    }

    return result;
  }

  bool ObjectProxy::add_method( const std::string& ifacename, std::shared_ptr<MethodProxyBase> method )
  {
    if ( not method ) return false;
    
    std::shared_ptr<InterfaceProxy> iface = this->interface(ifacename);

    if ( not iface ) iface = this->create_interface(ifacename);

    return iface->add_method( method );
  }

  std::shared_ptr<CallMessage> ObjectProxy::create_call_message( const std::string& interface_name, const std::string& method_name ) const
  {
    std::shared_ptr<CallMessage> call_message;
    
    if ( m_priv->m_destination.empty() )
    {
      call_message = CallMessage::create( m_priv->m_path, interface_name, method_name );
    }
    else
    {
      call_message = CallMessage::create( m_priv->m_destination, m_priv->m_path, interface_name, method_name );
    }

    return call_message;
  }

  std::shared_ptr<CallMessage> ObjectProxy::create_call_message( const std::string& method_name ) const
  {
    std::shared_ptr<CallMessage> call_message;
    
    if ( m_priv->m_destination.empty() )
    {
      call_message = CallMessage::create( m_priv->m_path, method_name );
    }
    else
    {
      call_message = CallMessage::create( m_priv->m_destination, m_priv->m_path, "", method_name );
    }

    return call_message;
  }

  std::shared_ptr<const ReturnMessage> ObjectProxy::call( std::shared_ptr<const CallMessage> call_message, int timeout_milliseconds ) const
  {
      std::shared_ptr<Connection> conn = m_priv->m_connection.lock();
    if ( !conn ) return std::shared_ptr<ReturnMessage>();

    return conn->send_with_reply_blocking( call_message, timeout_milliseconds );
  }

  std::shared_ptr<PendingCall> ObjectProxy::call_async( std::shared_ptr<const CallMessage> call_message, int timeout_milliseconds ) const
  {
      std::shared_ptr<Connection> conn = m_priv->m_connection.lock();
    if ( !conn ) return std::shared_ptr<PendingCall>();

    return conn->send_with_reply_async( call_message, timeout_milliseconds );
  }

  sigc::signal< void(std::shared_ptr<InterfaceProxy>)> ObjectProxy::signal_interface_added()
  {
    return m_priv->m_signal_interface_added;
  }

  sigc::signal< void(std::shared_ptr<InterfaceProxy>)> ObjectProxy::signal_interface_removed()
  {
    return m_priv->m_signal_interface_removed;
  }

}




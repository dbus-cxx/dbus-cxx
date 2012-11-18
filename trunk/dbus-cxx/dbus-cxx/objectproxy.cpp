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
#include "connection.h"
#include "interface.h"

#include <map>
#include <dbus/dbus.h>

namespace DBus
{
  ObjectProxy::ObjectProxy( Connection::pointer conn, const std::string& destination, const std::string& path ):
      m_connection(conn),
      m_destination(destination),
      m_path(path)
  {
    pthread_mutex_init( &m_name_mutex, NULL );
    pthread_rwlock_init( &m_interfaces_rwlock, NULL );
  }

  ObjectProxy::pointer ObjectProxy::create( const std::string& path )
  {
    return pointer( new ObjectProxy( Connection::pointer(), "", path ) );
  }

  ObjectProxy::pointer ObjectProxy::create( const std::string& destination, const std::string& path )
  {
    return pointer( new ObjectProxy( Connection::pointer(), destination, path ) );
  }

  ObjectProxy::pointer ObjectProxy::create( Connection::pointer conn, const std::string& path )
  {
    return pointer( new ObjectProxy( conn, "", path ) );
  }

  ObjectProxy::pointer ObjectProxy::create( Connection::pointer conn, const std::string& destination, const std::string& path )
  {
    return pointer( new ObjectProxy( conn, destination, path ) );
  }

  ObjectProxy::~ ObjectProxy( )
  {
    pthread_mutex_destroy( &m_name_mutex );
    pthread_rwlock_destroy( &m_interfaces_rwlock );
  }

  Connection::pointer ObjectProxy::connection() const
  {
    return m_connection;
  }

  void ObjectProxy::set_connection( Connection::pointer conn )
  {
    m_connection = conn;
    for ( Interfaces::iterator i = m_interfaces.begin(); i != m_interfaces.end(); i++ )
    {
      i->second->on_object_set_connection( conn );
    }
  }

  const std::string& ObjectProxy::destination() const
  {
    return m_destination;
  }

  void ObjectProxy::set_destination( const std::string& destination )
  {
    m_destination = destination;
  }

  const Path& ObjectProxy::path() const
  {
    return m_path;
  }

  void ObjectProxy::set_path( const std::string& path )
  {
    m_path = path;
    for ( Interfaces::iterator i = m_interfaces.begin(); i != m_interfaces.end(); i++ )
    {
      i->second->on_object_set_path( path );
    }
  }

  const ObjectProxy::Interfaces & ObjectProxy::interfaces() const
  {
    return m_interfaces;
  }

  InterfaceProxy::pointer ObjectProxy::interface( const std::string & name ) const
  {
    Interfaces::const_iterator iter;

    // ========== READ LOCK ==========
    pthread_rwlock_rdlock( &m_interfaces_rwlock );

    iter = m_interfaces.find( name );

    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_interfaces_rwlock );

    if ( iter == m_interfaces.end() ) return InterfaceProxy::pointer();

    return iter->second;
  }

  InterfaceProxy::pointer ObjectProxy::operator[]( const std::string& name ) const
  {
    return this->interface(name);
  }

  bool ObjectProxy::add_interface( InterfaceProxy::pointer interface )
  {
    bool result = true;

    if ( not interface ) return false;

    if ( interface->m_object ) interface->m_object->remove_interface( interface );
    
    // ========== WRITE LOCK ==========
    pthread_rwlock_wrlock( &m_interfaces_rwlock );

    InterfaceSignalNameConnections::iterator i;

    i = m_interface_signal_name_connections.find(interface);

    if ( i == m_interface_signal_name_connections.end() )
    {
      m_interface_signal_name_connections[interface] = interface->signal_name_changed().connect( sigc::bind(sigc::mem_fun(*this, &ObjectProxy::on_interface_name_changed), interface));

      m_interfaces.insert(std::make_pair(interface->name(), interface));

      interface->m_object = this;
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

    return result;
  }

  InterfaceProxy::pointer ObjectProxy::create_interface(const std::string & name)
  {
    InterfaceProxy::pointer interface;

    interface = InterfaceProxy::create(name);

    if ( this->add_interface(interface) ) return interface;

    return InterfaceProxy::pointer();
  }

  void ObjectProxy::remove_interface( const std::string & name )
  {
    Interfaces::iterator iter;
    InterfaceProxy::pointer interface, old_default;
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
      }
    
      if ( m_default_interface == interface ) {
        old_default = m_default_interface;
        m_default_interface = InterfaceProxy::pointer();
        need_emit_default_changed = true;
      }

    }

    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_interfaces_rwlock );

    if ( interface ) m_signal_interface_removed.emit( interface );

    if ( need_emit_default_changed ) m_signal_default_interface_changed.emit( old_default, m_default_interface );
  }

  void ObjectProxy::remove_interface( InterfaceProxy::pointer interface )
  {
    Interfaces::iterator current, upper;
    InterfaceProxy::pointer old_default;
    InterfaceSignalNameConnections::iterator i;
    
    bool need_emit_default_changed = false;
    bool interface_removed = false;

    if ( not interface ) return;

    // ========== WRITE LOCK ==========
    pthread_rwlock_wrlock( &m_interfaces_rwlock );

    current = m_interfaces.lower_bound( interface->name() );
    upper = m_interfaces.upper_bound( interface->name() );

    for ( ; current != upper; current++ )
    {
      if ( current->second == interface )
      {
        i = m_interface_signal_name_connections.find(interface);
        if ( i != m_interface_signal_name_connections.end() )
        {
          i->second.disconnect();
          m_interface_signal_name_connections.erase(i);
        }
    
        if ( m_default_interface == interface )
        {
          old_default = m_default_interface;
          m_default_interface = InterfaceProxy::pointer();
          need_emit_default_changed = true;
        }

        interface->m_object = NULL;
        m_interfaces.erase(current);

        interface_removed = true;
        
        break;
      }
    }

    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_interfaces_rwlock );

    if ( interface_removed ) m_signal_interface_removed.emit( interface );

    if ( need_emit_default_changed ) m_signal_default_interface_changed.emit( old_default, m_default_interface );
  }

  bool ObjectProxy::has_interface( const std::string & name ) const
  {
    Interfaces::const_iterator i;
    
    // ========== READ LOCK ==========
    pthread_rwlock_rdlock( &m_interfaces_rwlock );

    i = m_interfaces.find( name );

    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_interfaces_rwlock );

    return ( i != m_interfaces.end() );
  }

  bool ObjectProxy::has_interface( InterfaceProxy::pointer interface ) const
  {
    if ( not interface ) return false;
    
    Interfaces::const_iterator current, upper;
    bool result = false;

    // ========== READ LOCK ==========
    pthread_rwlock_rdlock( &m_interfaces_rwlock );

    current = m_interfaces.lower_bound(interface->name());

    if ( current != m_interfaces.end() )
    {
      upper = m_interfaces.upper_bound(interface->name());
      for ( ; current != upper; current++)
      {
        if ( current->second == interface )
        {
          result = true;
          break;
        }
      }
    }

    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_interfaces_rwlock );

    return result;
  }

  InterfaceProxy::pointer ObjectProxy::default_interface() const
  {
    return m_default_interface;
  }

  bool ObjectProxy::set_default_interface( const std::string& new_default_name )
  {
    Interfaces::iterator iter;
    InterfaceProxy::pointer old_default;
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

  bool ObjectProxy::set_default_interface( InterfaceProxy::pointer interface )
  {
    Interfaces::iterator iter;
    InterfaceProxy::pointer old_default;

    if ( not interface ) return false;

    if ( not this->has_interface(interface) ) this->add_interface(interface);

    old_default = m_default_interface;
    m_default_interface = interface;

    m_signal_default_interface_changed.emit( old_default, m_default_interface );

    return true;
  }

  void ObjectProxy::remove_default_interface()
  {
    if ( not m_default_interface ) return;

    InterfaceProxy::pointer old_default = m_default_interface;
    m_default_interface = InterfaceProxy::pointer();
    m_signal_default_interface_changed.emit( old_default, m_default_interface );
  }

  bool ObjectProxy::add_method( const std::string& ifacename, MethodProxyBase::pointer method )
  {
    if ( not method ) return false;
    
    InterfaceProxy::pointer iface = this->interface(ifacename);

    if ( not iface ) iface = this->create_interface(ifacename);

    return iface->add_method( method );
  }

  bool ObjectProxy::add_method( MethodProxyBase::pointer method )
  {
    if ( not method ) return false;
    
    InterfaceProxy::pointer iface = m_default_interface;
    
    if ( not iface )
    {
      iface = this->interface("");
      if ( not iface ) iface = this->create_interface("");
      if ( not m_default_interface ) this->set_default_interface(iface);
    }

    return iface->add_method(method);
  }

  CallMessage::pointer ObjectProxy::create_call_message( const std::string& interface_name, const std::string& method_name ) const
  {
    CallMessage::pointer call_message;
    
    if ( m_destination.empty() )
    {
      call_message = CallMessage::create( m_path, interface_name, method_name );
    }
    else
    {
      call_message = CallMessage::create( m_destination, m_path, interface_name, method_name );
    }

    return call_message;
  }

  CallMessage::pointer ObjectProxy::create_call_message( const std::string& method_name ) const
  {
    CallMessage::pointer call_message;
    
    if ( m_destination.empty() )
    {
      call_message = CallMessage::create( m_path, method_name );
    }
    else
    {
      call_message = CallMessage::create( m_destination, m_path, "", method_name );
    }

    return call_message;
  }

  ReturnMessage::const_pointer ObjectProxy::call( CallMessage::const_pointer call_message, int timeout_milliseconds ) const
  {
    if ( not m_connection or not m_connection->is_valid() ) return ReturnMessage::const_pointer();

//     if ( not call_message->expects_reply() )
//     {
//       m_connection->send( call_message );
//       return ReturnMessage::const_pointer();
//     }

    try{
      return m_connection->send_with_reply_blocking( call_message, timeout_milliseconds );
    }catch(DBusCxxPointer<DBus::Error> err){
      throw err;
    }
  }

  PendingCall::pointer ObjectProxy::call_async( CallMessage::const_pointer call_message, int timeout_milliseconds ) const
  {
    if ( not m_connection or not m_connection->is_valid() ) return PendingCall::pointer();

    try{
      return m_connection->send_with_reply_async( call_message, timeout_milliseconds );
    }catch(DBusCxxPointer<DBus::Error> err){
      throw err;
    }
  }

  sigc::signal< void, InterfaceProxy::pointer > ObjectProxy::signal_interface_added()
  {
    return m_signal_interface_added;
  }

  sigc::signal< void, InterfaceProxy::pointer > ObjectProxy::signal_interface_removed()
  {
    return m_signal_interface_removed;
  }

  sigc::signal< void, InterfaceProxy::pointer, InterfaceProxy::pointer > ObjectProxy::signal_default_interface_changed()
  {
    return m_signal_default_interface_changed;
  }

  void ObjectProxy::on_interface_name_changed(const std::string & oldname, const std::string & newname, InterfaceProxy::pointer interface)
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
          interface->signal_name_changed().connect(sigc::bind(sigc::mem_fun(*this,&ObjectProxy::on_interface_name_changed),interface));
    }
    
    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_interfaces_rwlock );
  }

}




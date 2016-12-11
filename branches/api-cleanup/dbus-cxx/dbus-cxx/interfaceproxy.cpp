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
#include "interfaceproxy.h"

#include "connection.h"
#include "objectproxy.h"

#include <map>
#include <dbus/dbus.h>

namespace DBus
{

  InterfaceProxy::InterfaceProxy( const std::string& name ):
      m_object(NULL),
      m_name(name)
  {
    pthread_rwlock_init( &m_methods_rwlock, NULL );
    pthread_mutex_init( &m_name_mutex, NULL );
  }

  InterfaceProxy::pointer InterfaceProxy::create(const std::string& name)
  {
    return pointer( new InterfaceProxy(name) );
  }

  InterfaceProxy::~ InterfaceProxy( )
  {
    pthread_rwlock_destroy( &m_methods_rwlock );
    pthread_mutex_destroy( &m_name_mutex );
  }

  ObjectProxy* InterfaceProxy::object() const
  {
    return m_object;
  }

  Path InterfaceProxy::path() const
  {
    if ( m_object ) return m_object->path();
    return Path();
  }

  Connection::pointer InterfaceProxy::connection() const
  {
    if ( m_object ) return m_object->connection();
    return Connection::pointer();
  }

  const std::string & InterfaceProxy::name() const
  {
    return m_name;
  }

  void DBus::InterfaceProxy::set_name(const std::string & new_name)
  {
    pthread_mutex_lock( &m_name_mutex );
    std::string old_name = m_name;
    m_name = new_name;
    pthread_mutex_unlock( &m_name_mutex );
    m_signal_name_changed.emit(old_name, new_name);
  }

  const InterfaceProxy::Methods & InterfaceProxy::methods() const
  {
    return m_methods;
  }

  MethodProxyBase::pointer InterfaceProxy::method( const std::string& name ) const
  {
    Methods::const_iterator iter;

    // ========== READ LOCK ==========
    pthread_rwlock_rdlock( &m_methods_rwlock );
    
    iter = m_methods.find( name );

    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_methods_rwlock );

    if ( iter == m_methods.end() ) return MethodProxyBase::pointer();

    return iter->second;
  }

  bool InterfaceProxy::add_method( MethodProxyBase::pointer method )
  {
    bool result = true;
    
    if ( not method ) return false;

    if ( this->has_method(method) ) return false;

    if ( method->m_interface ) method->m_interface->remove_method(method);
    
    // ========== WRITE LOCK ==========
    pthread_rwlock_wrlock( &m_methods_rwlock );

    MethodSignalNameConnections::iterator i;

    i = m_method_signal_name_connections.find(method);

    if ( i == m_method_signal_name_connections.end() )
    {
      m_method_signal_name_connections[method] =
          method->signal_name_changed().connect(sigc::bind(sigc::mem_fun(*this,&InterfaceProxy::on_method_name_changed),method));

      m_methods.insert(std::make_pair(method->name(), method));
      method->m_interface = this;
    }
    else
    {
      result = false;
    }

    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_methods_rwlock );

    m_signal_method_added.emit( method );

    return result;
  }

  void InterfaceProxy::remove_method( const std::string & name )
  {
    Methods::iterator iter;
    MethodProxyBase::pointer method;
    MethodSignalNameConnections::iterator i;

    // ========== WRITE LOCK ==========
    pthread_rwlock_wrlock( &m_methods_rwlock );

    iter = m_methods.find( name );
    if ( iter != m_methods.end() ) {
      method = iter->second;
      m_methods.erase( iter );
    }

    if ( method )
    {
      i = m_method_signal_name_connections.find(method);
      if ( i != m_method_signal_name_connections.end() )
      {
        i->second.disconnect();
        m_method_signal_name_connections.erase(i);
      }
    }

    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_methods_rwlock );

    method->m_interface = NULL;
    
    if ( method ) m_signal_method_removed.emit( method );
  }

  void InterfaceProxy::remove_method( MethodProxyBase::pointer method )
  {
    Methods::iterator current, upper;
    MethodSignalNameConnections::iterator i;

    if ( not method ) return;

    // ========== WRITE LOCK ==========
    pthread_rwlock_wrlock( &m_methods_rwlock );

    current = m_methods.lower_bound( method->name() );

    if ( current != m_methods.end() )
    {
      upper = m_methods.upper_bound( method->name() );
      for ( ; current != upper; current++ )
      {
        if ( current->second == method )
        {
          i = m_method_signal_name_connections.find(method);
          if ( i != m_method_signal_name_connections.end() )
          {
            i->second.disconnect();
            m_method_signal_name_connections.erase(i);
          }
          m_methods.erase(current);
        }
      }
    }

    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_methods_rwlock );

    method->m_interface = NULL;
    
    if ( method ) m_signal_method_removed.emit( method );
  }

  bool InterfaceProxy::has_method( const std::string & name ) const
  {
    Methods::const_iterator iter;
    
    // ========== READ LOCK ==========
    pthread_rwlock_rdlock( &m_methods_rwlock );

    iter = m_methods.find( name );

    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_methods_rwlock );

    return ( iter != m_methods.end() );
  }

  bool InterfaceProxy::has_method( MethodProxyBase::pointer method ) const
  {
    Methods::const_iterator current, upper;
    bool found = false;

    if ( not method ) return false;
    
    // ========== READ LOCK ==========
    pthread_rwlock_rdlock( &m_methods_rwlock );

    current = m_methods.lower_bound( method->name() );

    if ( current != m_methods.end() )
    {
      upper = m_methods.upper_bound( method->name() );
      for ( ; current != upper; current++ )
      {
        if ( current->second == method )
        {
          found = true;
          break;
        }
      }
    }

    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_methods_rwlock );

    return found;
  }

  CallMessage::pointer InterfaceProxy::create_call_message( const std::string& method_name ) const
  {
    if ( not m_object ) return CallMessage::pointer();
    return m_object->create_call_message( m_name, method_name );
  }

  ReturnMessage::const_pointer InterfaceProxy::call( CallMessage::const_pointer call_message, int timeout_milliseconds ) const
  {
    if ( not m_object ) return ReturnMessage::const_pointer();
    try{
      return m_object->call( call_message, timeout_milliseconds );
    }catch(DBusCxxPointer<DBus::Error> err){
      throw err;
    }
  }

  PendingCall::pointer InterfaceProxy::call_async( CallMessage::const_pointer call_message, int timeout_milliseconds ) const
  {
    if ( not m_object ) return PendingCall::pointer();
    try{  
      return m_object->call_async( call_message, timeout_milliseconds );
    }catch(DBusCxxPointer<DBus::Error> err){
      throw err;
    }
  }

  const InterfaceProxy::Signals& InterfaceProxy::signals() const
  {
    return m_signals;
  }

  signal_proxy_base::pointer InterfaceProxy::signal(const std::string & signame)
  {
    for ( Signals::iterator i = m_signals.begin(); i != m_signals.end(); i++ )
    {
      if ( (*i)->name() == signame ) return *i;
    }
    return signal_proxy_base::pointer();
  }

  bool InterfaceProxy::add_signal(signal_proxy_base::pointer sig)
  {
    // is it a valid signal?
    if ( not sig ) return false;

    // is it already added?
    if ( m_signals.find(sig) != m_signals.end() ) return false;

    // make sure the path and interface are set
    sig->set_path( this->path() );
    sig->set_interface( this->name() );

    // add it to the signal set
    m_signals.insert(sig);

    // connect it
    if ( this->connection() and this->connection()->is_valid() )
      this->connection()->add_signal_proxy(sig);

    return true;
  }

  bool InterfaceProxy::remove_signal(const std::string & signame)
  {
    return this->remove_signal( this->signal(signame) );
  }

  bool InterfaceProxy::remove_signal(signal_proxy_base::pointer sig)
  {
    if ( not sig ) return false;
    if ( not this->has_signal(sig) ) return false;
    this->connection()->remove_signal_proxy(sig);
    m_signals.erase(sig);
    return true;
  }

  bool InterfaceProxy::has_signal(const std::string & signame) const
  {
    for ( Signals::iterator i = m_signals.begin(); i != m_signals.end(); i++ )
    {
      if ( (*i)->name() == signame ) return true;
    }
    return false;
  }

  bool InterfaceProxy::has_signal(signal_proxy_base::pointer sig) const
  {
    return m_signals.find(sig) != m_signals.end();
  }

  sigc::signal< void, const std::string &, const std::string & > InterfaceProxy::signal_name_changed()
  {
    return m_signal_name_changed;
  }

  sigc::signal< void, MethodProxyBase::pointer > InterfaceProxy::signal_method_added()
  {
    return m_signal_method_added;
  }

  sigc::signal< void, MethodProxyBase::pointer > InterfaceProxy::signal_method_removed()
  {
    return m_signal_method_removed;
  }

  void InterfaceProxy::on_method_name_changed(const std::string & oldname, const std::string & newname, MethodProxyBase::pointer method)
  {
  
    // ========== WRITE LOCK ==========
    pthread_rwlock_wrlock( &m_methods_rwlock );

    Methods::iterator current, upper;
    current = m_methods.lower_bound(oldname);

    if ( current != m_methods.end() )
    {
      upper = m_methods.upper_bound(oldname);

      for ( ; current != upper; current++ )
      {
        if ( current->second == method )
        {
          m_methods.erase(current);
          break;
        }
      }
    }

    m_methods.insert( std::make_pair(newname, method) );

    MethodSignalNameConnections::iterator i;
    i = m_method_signal_name_connections.find(method);
    if ( i == m_method_signal_name_connections.end() )
    {
      m_method_signal_name_connections[method] =
          method->signal_name_changed().connect(sigc::bind(sigc::mem_fun(*this,&InterfaceProxy::on_method_name_changed),method));
    }
    
    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_methods_rwlock );
  }

  void InterfaceProxy::on_object_set_connection(DBusCxxPointer< Connection > conn)
  {
    for ( Signals::iterator i = m_signals.begin(); i != m_signals.end(); i++ )
    {
      if ( (not conn or not conn->is_valid()) and (*i)->connection() ) (*i)->connection()->remove_signal_proxy(*i);
      if ( conn and conn->is_valid() )
      {
        for ( Signals::iterator i = m_signals.begin(); i != m_signals.end(); i++ )
          conn->add_signal_proxy( *i );
      }
    }
  }

  void InterfaceProxy::on_object_set_path(const std::string & path)
  {
    for ( Signals::iterator i = m_signals.begin(); i != m_signals.end(); i++ )
      (*i)->set_path(path);
  }














// ######################### OLD ################################################################################  
//   InterfaceProxy::InterfaceProxy( Connection conn, const std::string& destination, const std::string& path, const std::string& interface ):
//       ObjectProxy( conn, destination, path ),
//       m_interface( interface )
//   {}
// 
//   InterfaceProxy::InterfaceProxy( const std::string & destination, const std::string & path, const std::string & interface ):
//           ObjectProxy( destination, path ),
//                        m_interface( interface )
//                        {}
// 
//   InterfaceProxy::InterfaceProxy( const std::string & path, const std::string & interface ):
//                            ObjectProxy( path ),
//                                         m_interface( interface )
//                                         {}
// 
//   InterfaceProxy::InterfaceProxy( const std::string& interface ):
//       m_interface( interface )
//   {}
// 
//   InterfaceProxy::~InterfaceProxy()
//   {}
// 
//   const std::string & InterfaceProxy::interface( )
//   {
//     return m_interface;
//   }
// 
//   bool InterfaceProxy::set_interface( const std::string & interface )
//   {
//     m_interface = interface;
//     return true;
//   }

}


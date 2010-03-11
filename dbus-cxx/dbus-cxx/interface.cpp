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
#include "interface.h"

#include <sstream>
#include <map>
#include <dbus/dbus.h>

namespace DBus
{

  Interface::Interface( const std::string& name ):
      m_object(NULL),
      m_name(name)
  {
    pthread_rwlock_init( &m_methods_rwlock, NULL );
    pthread_rwlock_init( &m_signals_rwlock, NULL );
    pthread_mutex_init( &m_name_mutex, NULL );
  }

  Interface::pointer Interface::create(const std::string& name)
  {
    return pointer( new Interface(name) );
  }

  Interface::~ Interface( )
  {
    pthread_rwlock_destroy( &m_methods_rwlock );
    pthread_rwlock_destroy( &m_signals_rwlock );
    pthread_mutex_destroy( &m_name_mutex );
  }

  Object* Interface::object() const
  {
    return m_object;
  }

  Path Interface::path() const
  {
    if ( m_object ) return m_object->path();
    return Path();
  }

  Connection::pointer Interface::connection() const
  {
    if ( m_object ) return m_object->connection();
    return Connection::pointer();
  }

  const std::string & Interface::name() const
  {
    return m_name;
  }

  void DBus::Interface::set_name(const std::string & new_name)
  {
    pthread_mutex_lock( &m_name_mutex );
    std::string old_name = m_name;
    m_name = new_name;

    for ( Signals::iterator i = m_signals.begin(); i != m_signals.end(); i++ )
      (*i)->set_interface( new_name );
    
    pthread_mutex_unlock( &m_name_mutex );

    m_signal_name_changed.emit(old_name, new_name);
  }

  const Interface::Methods & Interface::methods() const
  {
    return m_methods;
  }

  MethodBase::pointer Interface::method( const std::string& name ) const
  {
    Methods::const_iterator iter;

    // ========== READ LOCK ==========
    pthread_rwlock_rdlock( &m_methods_rwlock );
    
    iter = m_methods.find( name );

    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_methods_rwlock );

    if ( iter == m_methods.end() ) return MethodBase::pointer();

    return iter->second;
  }

  bool Interface::add_method( MethodBase::pointer method )
  {
    bool result = true;
    
    if ( not method ) return false;
    
    // ========== WRITE LOCK ==========
    pthread_rwlock_wrlock( &m_methods_rwlock );

    MethodSignalNameConnections::iterator i;

    i = m_method_signal_name_connections.find(method);

    if ( i == m_method_signal_name_connections.end() )
    {
      m_method_signal_name_connections[method] =
          method->signal_name_changed().connect(sigc::bind(sigc::mem_fun(*this,&Interface::on_method_name_changed),method));

      m_methods.insert(std::make_pair(method->name(), method));
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

  void Interface::remove_method( const std::string & name )
  {
    Methods::iterator iter;
    MethodBase::pointer method;
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
    
    if ( method ) m_signal_method_removed.emit( method );
  }

  bool Interface::has_method( const std::string & name ) const
  {
    Methods::const_iterator iter;
    
    // ========== READ LOCK ==========
    pthread_rwlock_rdlock( &m_methods_rwlock );

    iter = m_methods.find( name );

    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_methods_rwlock );

    return ( iter != m_methods.end() );
  }

  bool Interface::add_signal( signal_base::pointer sig )
  {
    bool result = false;

    if ( not sig ) return false;

    DBUS_CXX_DEBUG("Interface(" << this->name() << ")::add_signal (" << sig->name() << ")");
    
    // ========== WRITE LOCK ==========
    pthread_rwlock_wrlock( &m_signals_rwlock );

    // Do we already have the signal?
    if ( m_signals.find(sig) != m_signals.end() )
    {
      DBUS_CXX_DEBUG("Interface(" << this->name() << ")::add_signal (" << sig->name() << ") failed since signal is already present");
      // If so, we won't add it again
      result = false;
    }
    else
    {
      DBUS_CXX_DEBUG("Interface(" << this->name() << ")::add_signal (" << sig->name() << ") succeeded");
      m_signals.insert(sig);
      sig->set_connection( this->connection() );
      sig->set_path( this->path() );
      sig->set_interface( m_name );
      result = true;
    }

    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_signals_rwlock );

    return result;
  }

  bool Interface::remove_signal( signal_base::pointer signal )
  {
    bool result = false;
    
    // ========== WRITE LOCK ==========
    pthread_rwlock_wrlock( &m_signals_rwlock );

    Signals::iterator i = m_signals.find(signal);

    if ( i != m_signals.end() )
    {
      m_signals.erase(i);
      result = true;
    }

    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_signals_rwlock );

    return result;
  }

  bool Interface::remove_signal( const std::string& name )
  {
    bool result = false;
    
    // ========== WRITE LOCK ==========
    pthread_rwlock_wrlock( &m_signals_rwlock );

    Signals::iterator i = m_signals.begin();
    while ( i != m_signals.end() )
    {
      if ( (*i)->name() == name )
      {
        Signals::iterator temp = i++;
        m_signals.erase(temp);
      }
      else
      {
        i++;
      }
    }
    
    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_signals_rwlock );

    return result;
  }

  bool Interface::has_signal( signal_base::pointer signal ) const
  {
    bool result;
    
    // ========== READ LOCK ==========
    pthread_rwlock_rdlock( &m_signals_rwlock );

    result = m_signals.find(signal) != m_signals.end();
    
    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_signals_rwlock );

    return result;
  }

  bool Interface::has_signal( const std::string& name ) const
  {
    bool result = false;
    
    // ========== READ LOCK ==========
    pthread_rwlock_rdlock( &m_signals_rwlock );

    for ( Signals::iterator i = m_signals.begin(); i != m_signals.end(); i++ )
    {
      if ( (*i)->name() == name )
      {
        result = true;
        break;
      }
    }
    
    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_signals_rwlock );

    return result;
  }

  const Interface::Signals& Interface::signals()
  {
    return m_signals;
  }

  signal_base::pointer Interface::signal(const std::string& signal_name)
  {
    signal_base::pointer sig;
    
    // ========== READ LOCK ==========
    pthread_rwlock_rdlock( &m_signals_rwlock );

    for ( Signals::iterator i = m_signals.begin(); i != m_signals.end(); i++ )
    {
      if ( (*i)->name() == signal_name )
      {
        sig = *i;
        break;
      }
    }

    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_signals_rwlock );

    return sig;
  }

  sigc::signal< void, const std::string &, const std::string & > Interface::signal_name_changed()
  {
    return m_signal_name_changed;
  }

  sigc::signal< void, MethodBase::pointer > Interface::signal_method_added()
  {
    return m_signal_method_added;
  }

  sigc::signal< void, MethodBase::pointer > Interface::signal_method_removed()
  {
    return m_signal_method_removed;
  }

  std::string Interface::introspect(int space_depth) const
  {
    std::ostringstream sout;
    std::string spaces;
    Methods::const_iterator miter;
    Signals::const_iterator siter;
    for ( int i=0; i < space_depth; i++) spaces += " ";
    sout << spaces << "<interface name=\"" << this->name() << "\">\n";
    for ( miter = m_methods.begin(); miter != m_methods.end(); miter++ )
      sout << miter->second->introspect(space_depth+2);
    for ( siter = m_signals.begin(); siter != m_signals.end(); siter++ )
      sout << (*siter)->introspect(space_depth+2);
    sout << spaces << "</interface>\n";
    return sout.str();
  }

  HandlerResult Interface::handle_call_message( Connection::pointer connection, CallMessage::const_pointer message )
  {
    DBUS_CXX_DEBUG( "Interface::handle_call_message  interface=" << m_name );
    
    Methods::iterator current, upper;
    MethodBase::pointer method;
    HandlerResult result = NOT_HANDLED;

    // ========== READ LOCK ==========
//     pthread_rwlock_rdlock( &m_methods_rwlock );

    current = m_methods.lower_bound( message->member() );

    if ( current == m_methods.end() )
    {
      // ========== UNLOCK ==========
//       pthread_rwlock_unlock( &m_methods_rwlock );

      return NOT_HANDLED;
    }
    
    upper = m_methods.upper_bound( message->member() );

    for ( ; current != upper; current++ )
    {
      if ( current->second and current->second->handle_call_message( connection, message ) == HANDLED )
      {
        result = HANDLED;
        break;
      }
    }
    
    // ========== UNLOCK ==========
//     pthread_rwlock_unlock( &m_methods_rwlock );

    return result;
  }

  void Interface::on_method_name_changed(const std::string & oldname, const std::string & newname, MethodBase::pointer method)
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
          method->signal_name_changed().connect(sigc::bind(sigc::mem_fun(*this,&Interface::on_method_name_changed),method));
    }
    
    // ========== UNLOCK ==========
    pthread_rwlock_unlock( &m_methods_rwlock );
  }

  void Interface::set_connection(Connection::pointer conn)
  {
    DBUS_CXX_DEBUG("Interface(" << this->name() << ")::set_connection for " << m_signals.size() << " sub signals");
    for (Signals::iterator i = m_signals.begin(); i != m_signals.end(); i++)
      (*i)->set_connection(conn);
  }

  void Interface::set_path(const std::string& new_path)
  {
    for (Signals::iterator i = m_signals.begin(); i != m_signals.end(); i++)
      (*i)->set_path(new_path);
  }

  void Interface::set_object( Object* object )
  {
    m_object = object;
    
    if ( object == NULL )
    {
      for ( Signals::iterator i = m_signals.begin(); i != m_signals.end(); i++ )
      {
        (*i)->set_connection( Connection::pointer() );
        (*i)->set_path(std::string());
      }
    }
    else
    {
      for ( Signals::iterator i = m_signals.begin(); i != m_signals.end(); i++ )
      {
        (*i)->set_connection( m_object->connection() );
        (*i)->set_path( m_object->path() );
      }
    }
  }

}


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
#include "utility.h"
#include "interface.h"
#include "dbus-cxx-private.h"

#include <sstream>
#include <map>
#include <dbus/dbus.h>
#include <dbus-cxx/object.h>
#include <dbus-cxx/connection.h>

namespace DBus
{

  Interface::Interface( const std::string& name ):
      m_object(NULL),
      m_name(name)
  {
  }

  std::shared_ptr<Interface> Interface::create(const std::string& name)
  {
    return std::shared_ptr<Interface>( new Interface(name) );
  }

  Interface::~ Interface( )
  {
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

  std::shared_ptr<Connection> Interface::connection() const
  {
    if ( m_object ) return m_object->connection();
    return std::shared_ptr<Connection>();
  }

  const std::string & Interface::name() const
  {
    return m_name;
  }

  void DBus::Interface::set_name(const std::string & new_name)
  {
    std::string old_name;
    {
      std::lock_guard<std::mutex> lock( m_name_mutex );
      old_name = m_name;
      m_name = new_name;

      for ( Signals::iterator i = m_signals.begin(); i != m_signals.end(); i++ )
        (*i)->set_interface( new_name );
    }

    m_signal_name_changed.emit(old_name, new_name);
  }

  const Interface::Methods & Interface::methods() const
  {
    return m_methods;
  }

  std::shared_ptr<MethodBase> Interface::method( const std::string& name ) const
  {
    Methods::const_iterator iter;

    {
      std::shared_lock lock( m_methods_rwlock );
    
      iter = m_methods.find( name );
    }

    if ( iter == m_methods.end() ) return std::shared_ptr<MethodBase>();

    return iter->second;
  }

  bool Interface::add_method( std::shared_ptr<MethodBase> method )
  {
    bool result = true;
    
    if ( not method ) return false;
    
    {
      std::unique_lock lock( m_methods_rwlock );

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
    }

    m_signal_method_added.emit( method );

    return result;
  }

  void Interface::remove_method( const std::string & name )
  {
    Methods::iterator iter;
    std::shared_ptr<MethodBase> method;
    MethodSignalNameConnections::iterator i;

    {
      std::unique_lock lock( m_methods_rwlock );

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
    }

    if ( method ) m_signal_method_removed.emit( method );
  }

  bool Interface::has_method( const std::string & name ) const
  {
    Methods::const_iterator iter;
    std::shared_lock lock( m_methods_rwlock );
    
    iter = m_methods.find( name );

    return ( iter != m_methods.end() );
  }

  bool Interface::add_signal( std::shared_ptr<signal_base> sig )
  {
    bool result = false;

    if ( not sig ) return false;

    SIMPLELOGGER_DEBUG("dbus.Interface", "Interface(" << this->name() << ")::add_signal (" << sig->name() << ")");
    
    std::unique_lock lock( m_signals_rwlock );

    // Do we already have the signal?
    if ( m_signals.find(sig) != m_signals.end() )
    {
      SIMPLELOGGER_DEBUG("dbus.Interface", "Interface(" << this->name() << ")::add_signal (" << sig->name() << ") failed since signal is already present");
      // If so, we won't add it again
      result = false;
    }
    else
    {
      SIMPLELOGGER_DEBUG("dbus.Interface", "Interface(" << this->name() << ")::add_signal (" << sig->name() << ") succeeded");
      m_signals.insert(sig);
      sig->set_connection( this->connection() );
      sig->set_path( this->path() );
      sig->set_interface( m_name );
      result = true;
    }

    return result;
  }

  bool Interface::remove_signal( std::shared_ptr<signal_base> signal )
  {
    bool result = false;
    std::unique_lock lock( m_signals_rwlock );
    
    Signals::iterator i = m_signals.find(signal);

    if ( i != m_signals.end() )
    {
      m_signals.erase(i);
      result = true;
    }

    return result;
  }

  bool Interface::remove_signal( const std::string& name )
  {
    bool result = false;
    std::unique_lock lock( m_signals_rwlock );
    
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

    return result;
  }

  bool Interface::has_signal( std::shared_ptr<signal_base> signal ) const
  {
    bool result;
    std::shared_lock lock( m_signals_rwlock );
    
    result = m_signals.find(signal) != m_signals.end();

    return result;
  }

  bool Interface::has_signal( const std::string& name ) const
  {
    bool result = false;
    std::shared_lock lock( m_signals_rwlock );

    for ( Signals::iterator i = m_signals.begin(); i != m_signals.end(); i++ )
    {
      if ( (*i)->name() == name )
      {
        result = true;
        break;
      }
    }

    return result;
  }

  const Interface::Signals& Interface::signals()
  {
    return m_signals;
  }

  std::shared_ptr<signal_base> Interface::signal(const std::string& signal_name)
  {
    std::shared_ptr<signal_base> sig;
    std::shared_lock lock( m_signals_rwlock );

    for ( Signals::iterator i = m_signals.begin(); i != m_signals.end(); i++ )
    {
      if ( (*i)->name() == signal_name )
      {
        sig = *i;
        break;
      }
    }

    return sig;
  }

  sigc::signal< void( const std::string &, const std::string &) > Interface::signal_name_changed()
  {
    return m_signal_name_changed;
  }

  sigc::signal< void( std::shared_ptr<MethodBase>)> Interface::signal_method_added()
  {
    return m_signal_method_added;
  }

  sigc::signal< void(std::shared_ptr<MethodBase>)> Interface::signal_method_removed()
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

  HandlerResult Interface::handle_call_message( std::shared_ptr<Connection> connection, std::shared_ptr<const CallMessage> message )
  {
    SIMPLELOGGER_DEBUG( "dbus.Interface", "handle_call_message  interface=" << m_name );
    
    Methods::iterator current, upper;
    std::shared_ptr<MethodBase> method;
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

  void Interface::on_method_name_changed(const std::string & oldname, const std::string & newname, std::shared_ptr<MethodBase> method)
  {
    std::unique_lock lock( m_methods_rwlock );

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
  }

  void Interface::set_connection(std::shared_ptr<Connection> conn)
  {
    SIMPLELOGGER_DEBUG("dbus.Interface", "Interface(" << this->name() << ")::set_connection for " << m_signals.size() << " sub signals");
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
        (*i)->set_connection( std::shared_ptr<Connection>() );
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


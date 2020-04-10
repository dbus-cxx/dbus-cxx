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
#include <dbus-cxx/object.h>
#include <map>
#include <utility>
#include "callmessage.h"
#include "dbus-cxx-private.h"
#include "methodbase.h"
#include <sigc++/sigc++.h>
#include "signal_base.h"

static const char* LOGGER_NAME = "DBus.Interface";

namespace DBus
{
  class Connection;

  Interface::Interface( const std::string& name ):
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


  Path Interface::path() const
  {
    return Path( m_path );
  }

  const std::string & Interface::name() const
  {
    return m_name;
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

    if( has_method( method->name() ) ) return false;
    
    {
      std::unique_lock lock( m_methods_rwlock );

      m_methods.insert(std::make_pair(method->name(), method));
    }

    m_signal_method_added.emit( method );

    return result;
  }

  bool Interface::remove_method( const std::string & name )
  {
    Methods::iterator iter;
    std::shared_ptr<MethodBase> method;

    {
      std::unique_lock lock( m_methods_rwlock );

      iter = m_methods.find( name );
      if ( iter != m_methods.end() ) {
        method = iter->second;
        m_methods.erase( iter );
      }
    }

    if ( method ){
        m_signal_method_removed.emit( method );
        return true;
    }

    return false;
  }

  bool Interface::remove_method( std::shared_ptr<MethodBase> torem )
  {
    Methods::iterator iter;
    std::shared_ptr<MethodBase> method;

    {
      std::unique_lock lock( m_methods_rwlock );

      iter = m_methods.find( torem->name() );
      if ( iter != m_methods.end() ) {
        method = iter->second;
      }

      if ( method == torem )
      {
        m_methods.erase( iter );
      }
    }

    if ( method ){
        m_signal_method_removed.emit( method );
        return true;
    }

    return false;
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

  HandlerResult Interface::handle_call_message( std::shared_ptr<Connection> conn, std::shared_ptr<const CallMessage> message )
  {
    SIMPLELOGGER_DEBUG( LOGGER_NAME, "handle_call_message  interface=" << m_name );
    
    Methods::iterator method_it;
    std::shared_ptr<MethodBase> method;

    method_it = m_methods.find( message->member() );

    if ( method_it == m_methods.end() )
    {
      return HandlerResult::NOT_HANDLED;
    }

    return method_it->second->handle_call_message( conn, message );
  }

  void Interface::set_path(const std::string& new_path)
  {
      m_path = new_path;
    for (Signals::iterator i = m_signals.begin(); i != m_signals.end(); i++)
      (*i)->set_path(new_path);
  }

}


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
#ifndef DBUSCXXINTERFACEPROXY_H
#define DBUSCXXINTERFACEPROXY_H

#include <sigc++/sigc++.h>

#include <string>
#include <map>
#include <set>

#include <dbus-cxx/methodproxy.h>
#include <dbus-cxx/signal_proxy.h>

namespace DBus {

  class ObjectProxy;

  /**
   * @ingroup objects
   * @ingroup proxy
   * 
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class InterfaceProxy
  {
    protected:
      InterfaceProxy(const std::string& name);

    public:
      typedef DBusCxxPointer<InterfaceProxy> pointer;

      typedef DBusCxxWeakPointer<InterfaceProxy> weak_pointer;
      
      typedef std::multimap<std::string, MethodProxyBase::pointer> Methods;

      typedef std::set<signal_proxy_base::pointer> Signals;

      static pointer create( const std::string& name = std::string() );

      virtual ~InterfaceProxy();

      ObjectProxy* object() const;

      Path path() const;

      DBusCxxPointer<Connection> connection() const;

      const std::string& name() const;

      void set_name( const std::string& new_name );

      const Methods& methods() const;

      /** Returns the first method with the given name */
      MethodProxyBase::pointer method( const std::string& name ) const;

      template <class T_return, class T_arg1=nil, class T_arg2=nil, class T_arg3=nil, class T_arg4=nil, class T_arg5=nil, class T_arg6=nil, class T_arg7=nil>
      DBusCxxPointer<MethodProxy<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7> > create_method( const std::string& name )
      {
        DBusCxxPointer< MethodProxy<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7> > method;
        method = MethodProxy<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7>::create(name);
        this->add_method( method );
        return method;
      }

      /** Adds the named method */
      bool add_method( MethodProxyBase::pointer method );

      /** Removes the first method with the given name */
      void remove_method( const std::string& name );

      /** Removed the specific method */
      void remove_method( MethodProxyBase::pointer method );

      /** True if the interface has a method with the given name */
      bool has_method( const std::string& name ) const;

      /** True if the interface has the specified method */
      bool has_method( MethodProxyBase::pointer method ) const;
      
      CallMessage::pointer create_call_message( const std::string& method_name ) const;

      ReturnMessage::const_pointer call( CallMessage::const_pointer, int timeout_milliseconds=-1 ) const;

      PendingCall::pointer call_async( CallMessage::const_pointer, int timeout_milliseconds=-1 ) const;

      template <class T_return, class T_arg1=nil, class T_arg2=nil, class T_arg3=nil, class T_arg4=nil, class T_arg5=nil, class T_arg6=nil, class T_arg7=nil>
      DBusCxxPointer<signal_proxy<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7> > create_signal( const std::string& sig_name )
      {
        DBusCxxPointer< signal_proxy<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7> > sig;
        sig = signal_proxy<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7>::create(this->path(), m_name, sig_name);
        this->add_signal( sig );
        return sig;
      }

      const Signals& signals() const;

      signal_proxy_base::pointer signal( const std::string& signame );

      bool add_signal( signal_proxy_base::pointer sig );

      bool remove_signal( const std::string& signame );

      bool remove_signal( signal_proxy_base::pointer sig );

      bool has_signal( const std::string& signame ) const;

      bool has_signal( signal_proxy_base::pointer sig ) const;

      /** Signal emitted when the name is changed */
      sigc::signal<void,const std::string&/*old name*/,const std::string&/*new name*/> signal_name_changed();

      /** Signal emitted when a method of the given name is added */
      sigc::signal<void,MethodProxyBase::pointer> signal_method_added();

      /** Signal emitted when a method of the given name is removed */
      sigc::signal<void,MethodProxyBase::pointer> signal_method_removed();

    protected:

      friend class ObjectProxy;
      
      ObjectProxy* m_object;
      
      std::string m_name;
      
      Methods m_methods;

      Signals m_signals;

      mutable pthread_rwlock_t m_methods_rwlock;

      /** Ensures that the name doesn't change while the name changed signal is emitting */
      pthread_mutex_t m_name_mutex;

      sigc::signal<void,const std::string&,const std::string&> m_signal_name_changed;
      
      sigc::signal<void,MethodProxyBase::pointer> m_signal_method_added;
      
      sigc::signal<void,MethodProxyBase::pointer> m_signal_method_removed;

      typedef std::map<MethodProxyBase::pointer,sigc::connection> MethodSignalNameConnections;

      MethodSignalNameConnections m_method_signal_name_connections;

      void on_method_name_changed(const std::string& oldname, const std::string& newname, MethodProxyBase::pointer method);

      void on_object_set_connection( DBusCxxPointer<Connection> conn );

      void on_object_set_path( const std::string& path );
  };

}

#endif


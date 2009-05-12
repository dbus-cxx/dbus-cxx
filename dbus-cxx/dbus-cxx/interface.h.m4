dnl ***************************************************************************
dnl *   Copyright (C) 2009 by Rick L. Vinyard, Jr.                            *
dnl *   rvinyard@cs.nmsu.edu                                                  *
dnl *                                                                         *
dnl *   This file is part of the dbus-cxx library.                            *
dnl *                                                                         *
dnl *   The dbus-cxx library is free software; you can redistribute it and/or *
dnl *   modify it under the terms of the GNU General Public License           *
dnl *   version 3 as published by the Free Software Foundation.               *
dnl *                                                                         *
dnl *   The dbus-cxx library is distributed in the hope that it will be       *
dnl *   useful, but WITHOUT ANY WARRANTY; without even the implied warranty   *
dnl *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   *
dnl *   General Public License for more details.                              *
dnl *                                                                         *
dnl *   You should have received a copy of the GNU General Public License     *
dnl *   along with this software. If not see <http://www.gnu.org/licenses/>.  *
dnl ***************************************************************************

divert(-1)

include(template.macros.m4)

define([CREATE_METHOD],[dnl
      dnl
template <LIST(class T_return, LOOP(class T_arg%1, [$1]))>
      DBusCxxPointer<Method<LIST(T_return, LOOP(T_arg%1, $1))> > create_method( const std::string& name, sigc::slot$1<LIST(T_return, LOOP(T_arg%1, $1))> slot )
      {
        DBusCxxPointer< Method<LIST(T_return, LOOP(T_arg%1, $1))> > method;
        method = Method<LIST(T_return, LOOP(T_arg%1, $1))>::create(name);
        method->set_method( slot );
        this->add_method( method );
        return method;
      }
      dnl
])
    
divert(0)
dnl
#ifndef DBUSCXXINTERFACE_H
#define DBUSCXXINTERFACE_H

#include <sigc++/sigc++.h>

#include <string>
#include <map>
#include <set>

#include <dbus-cxx/methodbase.h>
#include <dbus-cxx/method.h>
#include <dbus-cxx/dbus_signal.h>


namespace DBus {

  class Object;

  /**
   * @ingroup objects
   * @ingroup local
   *
   * @todo Do something about the signals when the interface name is changed
   * 
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class Interface
  {
    protected:
      Interface(const std::string& name);

    public:
      typedef DBusCxxPointer<Interface> pointer;

      typedef DBusCxxWeakPointer<Interface> weak_pointer;
      
      typedef std::multimap<std::string, MethodBase::pointer> Methods;

      typedef std::set<signal_base::pointer> Signals;

      static pointer create( const std::string& name = std::string() );

      virtual ~Interface();

      /**
       * Returns the object associated with this interface
       *
       * Note that there is no set_object() method as an interface must be
       * added to an object through the object's add_interface() method.
       */
      Object* object() const;

      /** Returns the path of the object associated with this interface or a null string if no object is associated */
      std::string path() const;

      /** Returns the connection associated with this interface's object or a null pointer if no object is associated */
      DBusCxxPointer<Connection> connection() const;

      HandlerResult handle_call_message( DBusCxxPointer<Connection> connection, CallMessage::const_pointer message );

      const std::string& name() const;

      void set_name( const std::string& new_name );

      const Methods& methods() const;

      /** Returns the first method with the given name */
      MethodBase::pointer method( const std::string& name ) const;

      template <class T_return, class T_arg1=nil, class T_arg2=nil, class T_arg3=nil, class T_arg4=nil, class T_arg5=nil, class T_arg6=nil, class T_arg7=nil>
      DBusCxxPointer<Method<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7> > create_method( const std::string& name )
      {
        DBusCxxPointer< Method<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7> > method;
        method = Method<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7>::create(name);
        this->add_method( method );
        return method;
      }

FOR(0, eval(CALL_SIZE),[[CREATE_METHOD(%1)
]])
      /** Adds the named method */
      bool add_method( MethodBase::pointer method );

      /** Removes the first method with the given name */
      void remove_method( const std::string& name );

      /** True if the interface has a method with the given name */
      bool has_method( const std::string& name ) const;

      /**
       * Adds the given signal
       * @return true if the signal was successfully added, false otherwise
       */
      bool add_signal( signal_base::pointer signal );

      /**
       * Removes the given signal
       * @return true if the signal was successfully removed, false otherwise
       *
       * One reason a signal couldn't be removed is if it wasn't a part of the interface.
       */
      bool remove_signal( signal_base::pointer signal );

      /**
       * Removes all signals with the given name
       */
      bool remove_signal( const std::string& name );

      /**
       * True if the given signal is part of this interface
       */
      bool has_signal( signal_base::pointer signal ) const;

      /** True if this interface has at least one signal with the given name */
      bool has_signal( const std::string& name ) const;

      template <class T_return, class T_arg1=nil, class T_arg2=nil, class T_arg3=nil, class T_arg4=nil, class T_arg5=nil, class T_arg6=nil, class T_arg7=nil>
      DBusCxxPointer<signal<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7> > create_signal( const std::string& name ) {
        DBusCxxPointer<DBus::signal<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7> > sig;
        sig = DBus::signal<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7>::create(m_name, name);

        if ( this->add_signal(sig) ) return sig;

        return DBusCxxPointer<DBus::signal<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7> >();
      }

      /** Returns the signals associated with this interface */
      const Signals& signals();

      /**
       * Returns the first signal found with a matching name.
       * If more than one signal has a given name there are no guarantees
       * as to which signal will be returned.
       */
      signal_base::pointer signal(const std::string& signal_name);

      /** Signal emitted when the name is changed */
      sigc::signal<void,const std::string&/*old name*/,const std::string&/*new name*/> signal_name_changed();

      /** Signal emitted when a method of the given name is added */
      sigc::signal<void,MethodBase::pointer> signal_method_added();

      /** Signal emitted when a method of the given name is removed */
      sigc::signal<void,MethodBase::pointer> signal_method_removed();

    private:

      Object* m_object;
      
    protected:

      friend class Object;

      void set_object( Object* object );

      std::string m_name;
      
      Methods m_methods;

      Signals m_signals;

      mutable pthread_rwlock_t m_methods_rwlock;

      mutable pthread_rwlock_t m_signals_rwlock;

      /** Ensures that the name doesn't change while the name changed signal is emitting */
      pthread_mutex_t m_name_mutex;

      sigc::signal<void,const std::string&,const std::string&> m_signal_name_changed;
      
      sigc::signal<void,MethodBase::pointer> m_signal_method_added;
      
      sigc::signal<void,MethodBase::pointer> m_signal_method_removed;

      typedef std::map<MethodBase::pointer,sigc::connection> MethodSignalNameConnections;

      MethodSignalNameConnections m_method_signal_name_connections;

      void on_method_name_changed(const std::string& oldname, const std::string& newname, MethodBase::pointer method);

      void set_connection(DBusCxxPointer<Connection> conn);

      void set_path( const std::string& new_path );

  };

} /* namespace DBus */

#endif /* DBUS_CXX_INTERFACE_H */


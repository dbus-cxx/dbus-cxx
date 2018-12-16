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

define([DECLARE_CREATE_METHOD],[dnl
      /**
       * Creates a method with a return value (possibly \c void ) and $1 parameters
       * @return A smart pointer to the newly created method
       * @param name The name that will be associated with this method
       */
      template <LIST(class T_return, LOOP(class T_arg%1, [$1]))>
      DBusCxxPointer<Method<LIST(T_return, LOOP(T_arg%1, $1))> >
      create_method( const std::string& name );
])
    

define([DEFINE_CREATE_METHOD],[dnl
      template <LIST(class T_return, LOOP(class T_arg%1, [$1]))>
      DBusCxxPointer<Method<LIST(T_return, LOOP(T_arg%1, $1))> >
      Interface::create_method( const std::string& name )
      {
        DBusCxxPointer< Method<LIST(T_return, LOOP(T_arg%1, $1))> > method;
        method = Method<LIST(T_return, LOOP(T_arg%1, $1))>::create(name);
        this->add_method( method );
        return method;
      }
])

define([DECLARE_CREATE_METHOD_SLOT],[dnl
      /**
       * Creates a method with a signature equivalent to the provided @param slot parameter's signature
       * @return A smart pointer to the newly created method
       * @param slot This slot will be called with the given signature when the method is invoked
       *
       * Template parameters of the sigc::slot will determine the signature of
       * the method created.
       */
      template <LIST(class T_return, LOOP(class T_arg%1, [$1]))>
      DBusCxxPointer<Method<LIST(T_return, LOOP(T_arg%1, $1))> >
      create_method( const std::string& name, sigc::slot$1<LIST(T_return, LOOP(T_arg%1, $1))> slot );
])

define([DEFINE_CREATE_METHOD_SLOT],[dnl
      template <LIST(class T_return, LOOP(class T_arg%1, [$1]))>
      DBusCxxPointer<Method<LIST(T_return, LOOP(T_arg%1, $1))> >
      Interface::create_method( const std::string& name, sigc::slot$1<LIST(T_return, LOOP(T_arg%1, $1))> slot )
      {
        DBusCxxPointer< Method<LIST(T_return, LOOP(T_arg%1, $1))> > method;
        method = Method<LIST(T_return, LOOP(T_arg%1, $1))>::create(name);
        method->set_method( slot );
        this->add_method( method );
        return method;
      }
])

define([DECLARE_CREATE_SIGNAL],[dnl
      /**
       * Creates a signal with a return value (possibly \c void ) and $1 parameters
       * @return A smart pointer to the newly created signal
       * @param name The name that will be associated with this signal
       */
      template <LIST(class T_return, LOOP(class T_arg%1, [$1]))>
      DBusCxxPointer<signal<LIST(T_return, LOOP(T_arg%1, $1))> >
      create_signal( const std::string& name );
])
    
define([DEFINE_CREATE_SIGNAL],[dnl
      template <LIST(class T_return, LOOP(class T_arg%1, [$1]))>
      DBusCxxPointer<signal<LIST(T_return, LOOP(T_arg%1, $1))> >
      Interface::create_signal( const std::string& name )
      {
        DBusCxxPointer<DBus::signal<LIST(T_return, LOOP(T_arg%1, $1))> > sig;
        sig = DBus::signal<LIST(T_return, LOOP(T_arg%1, $1))>::create(m_name, name);
        if ( this->add_signal(sig) ) return sig;
        return DBusCxxPointer<DBus::signal<LIST(T_return,LOOP(T_arg%1, [$1]))> >();
      }
])
    
divert(0)
dnl
[/***************************************************************************
 *   Copyright (C) 2009 by Rick L. Vinyard, Jr.                            *
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
 ***************************************************************************/]
#include <sigc++/sigc++.h>

#include <string>
#include <map>
#include <set>

#include <dbus-cxx/forward_decls.h>
#include <dbus-cxx/methodbase.h>
#include <dbus-cxx/dbus_signal.h>

#ifndef DBUSCXX_INTERFACE_H
#define DBUSCXX_INTERFACE_H

namespace DBus {

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
      /**
       * This class has a protected constructor. Use the \c create() methods
       * to obtain a smart pointer to a new instance.
       */
      Interface(const std::string& name);

    public:
      /**
       * Typedef to smart pointers to Interface.
       * 
       * Can access \e type as \c Interface::pointer
       */
      typedef DBusCxxPointer<Interface> pointer;

      /**
       * Typedef to weak smart pointers to Interface.
       *
       * Can access \e type as \c Interface::weak_pointer
       */
      typedef DBusCxxWeakPointer<Interface> weak_pointer;
      
      /**
       * Typedef to the storage structure for methods.
       *
       * \b Data \b Structure - multimap is used since multiple methods can have the same name. This is what allows for overloading.
       * \b Key - method name
       * \b Value -smart pointer to a method.
       *
       * Can access \e type as \c Interface::Methods
       */
      typedef std::multimap<std::string, MethodBase::pointer> Methods;

      /**
       * Typedef to the storage structure for signals.
       *
       * \b Data \b Structure - sets is used since signal names are not needed for the interface, but must be unique.
       *
       * Can access \e type as \c Interface::Signals
       */
      typedef std::set<signal_base::pointer> Signals;

      /**
       * Creates a named Interface
       * @param name The name of this interface
       */
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
      Path path() const;

      /** Returns the connection associated with this interface's object or a null pointer if no object is associated */
      DBusCxxPointer<Connection> connection() const;

      /**
       * Handles the specified call message on the specified connection
       *
       * Looks for methods matching the name specified in the message, then
       * calls handle_call_message() for each matching message.
       *
       * Once a method returns \c HANDLED no further methods will be tried.
       *
       * @return \c HANDLED if one method in this interface handled the message, \c NOT_HANDLED otherwise
       * @param conn The Connection to send the reply message on
       * @param msg The CallMessage to handle
       */
      HandlerResult handle_call_message( DBusCxxPointer<Connection> connection, CallMessage::const_pointer message );

      /** Get the name of this interface */
      const std::string& name() const;

      /** Sets the name of this interface */
      void set_name( const std::string& new_name );

      /** Returns the methods associated with this interface */
      const Methods& methods() const;

      /** Returns the first method with the given name */
      MethodBase::pointer method( const std::string& name ) const;

FOR(0, eval(CALL_SIZE),[[DECLARE_CREATE_METHOD(%1)
]])

FOR(0, eval(CALL_SIZE),[[DECLARE_CREATE_METHOD_SLOT(%1)
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

FOR(0, eval(CALL_SIZE),[[DECLARE_CREATE_SIGNAL(%1)
]])

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

      /** Returns a DBus XML description of this interface */
      std::string introspect(int space_depth=0) const;

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

      /**
       * Callback point that updates the method name map when a method changes
       * its name.
       */
      void on_method_name_changed(const std::string& oldname, const std::string& newname, MethodBase::pointer method);

      void set_connection(DBusCxxPointer<Connection> conn);

      void set_path( const std::string& new_path );

  };

}

#include <dbus-cxx/method.h>

namespace DBus {

FOR(0, eval(CALL_SIZE),[[DEFINE_CREATE_METHOD(%1)
]])

FOR(0, eval(CALL_SIZE),[[DEFINE_CREATE_METHOD_SLOT(%1)
]])

FOR(0, eval(CALL_SIZE),[[DEFINE_CREATE_SIGNAL(%1)
]])

} /* namespace DBus */

#endif /* DBUS_CXX_INTERFACE_H */


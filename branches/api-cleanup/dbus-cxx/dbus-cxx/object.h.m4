dnl ***************************************************************************
dnl *   Copyright (C) 2009,2010 by Rick L. Vinyard, Jr.                       *
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
       * Creates a method with a signature based on the @param slot parameter signature and adds it to the default interface
       * @return A smart pointer to the newly created method
       * @param slot This slot will be called with the given signature when the method is invoked
       *
       * Template parameters of the sigc::slot will determine the signature of
       * the method created.
       */
      template <LIST(class T_return, LOOP(class T_arg%1, [$1]))>
      DBusCxxPointer<Method<LIST(T_return, LOOP(T_arg%1, $1))> >
      create_method( const std::string& method_name, sigc::slot$1<LIST(T_return, LOOP(T_arg%1, $1))> slot );
])
    
define([DEFINE_CREATE_METHOD],[dnl
  template <LIST(class T_return, LOOP(class T_arg%1, [$1]))>
  DBusCxxPointer<Method<LIST(T_return, LOOP(T_arg%1, $1))> >
  Object::create_method( const std::string& method_name, sigc::slot$1<LIST(T_return, LOOP(T_arg%1, $1))> slot )
  {
    if ( not m_default_interface )
    {
      this->create_interface("");
      this->set_default_interface("");
    }
    // TODO throw an error if the default interface still doesn't exist

    DBusCxxPointer< Method<LIST(T_return, LOOP(T_arg%1, $1))> > method;
    method = m_default_interface->create_method<LIST(T_return, LOOP(T_arg%1, $1))>(method_name);
    method->set_method( slot );
    return method;
  }
])

define([DECLARE_CREATE_INTERFACE_METHOD],[dnl
      /**
       * Creates a method with a signature based on the @param slot parameter signature and adds it to the named interface
       * @return A smart pointer to the newly created method
       * @param slot This slot will be called with the given signature when the method is invoked
       *
       * Template parameters of the sigc::slot will determine the signature of
       * the method created.
       */
      template <LIST(class T_return, LOOP(class T_arg%1, [$1]))>
      DBusCxxPointer<Method<LIST(T_return, LOOP(T_arg%1, $1))> >
      create_method( const std::string& interface_name, const std::string& method_name, sigc::slot$1<LIST(T_return, LOOP(T_arg%1, $1))> slot );
])
    
define([DEFINE_CREATE_INTERFACE_METHOD],[dnl
  template <LIST(class T_return, LOOP(class T_arg%1, [$1]))>
  DBusCxxPointer<Method<LIST(T_return, LOOP(T_arg%1, $1))> >
  Object::create_method( const std::string& interface_name, const std::string& method_name, sigc::slot$1<LIST(T_return, LOOP(T_arg%1, $1))> slot )
  {
    Interface::pointer interface;
    interface = this->interface(interface_name);
    if ( not interface ) interface = this->create_interface(interface_name);
    // TODO throw an error if the interface still doesn't exist

    DBusCxxPointer< Method<LIST(T_return, LOOP(T_arg%1, $1))> > method;
    method = interface->create_method<LIST(T_return, LOOP(T_arg%1, $1))>(method_name);
    method->set_method( slot );
    return method;
  }
])
    

define([DECLARE_CREATE_SIGNAL_N],[dnl
      /**
       * Creates a signal with a return value (possibly \c void ) and $1 parameters and adds it to the default interface
       * @return A smart pointer to the newly created signal
       *
       * Template parameters for the \c create_signal() call will determine the
       * signature of the signal created.
       */
      template <LIST(class T_return, LOOP(class T_arg%1, [$1]))>
      DBusCxxPointer<signal<LIST(T_return, LOOP(T_arg%1, $1))> >
      create_signal( const std::string& name );
])
    
define([DEFINE_CREATE_SIGNAL_N],[dnl
  template <LIST(class T_return, LOOP(class T_arg%1, [$1]))>
  DBusCxxPointer<signal<LIST(T_return, LOOP(T_arg%1, $1))> >
  Object::create_signal( const std::string& name )
  {
    DBusCxxPointer<DBus::signal<LIST(T_return, LOOP(T_arg%1, $1))> > sig;
    Interface::pointer iface = this->default_interface();
    if ( not iface ) iface = this->create_interface("");
    sig = iface->create_signal<LIST(T_return, LOOP(T_arg%1, $1))>(name);
    return sig;
  }
])
    
define([DECLARE_CREATE_SIGNAL_IN],[dnl
      /**
       * Creates a signal with a return value (possibly \c void ) and $1 parameters and adds it to the named interface
       * @return A smart pointer to the newly created signal
       *
       * Template parameters for the \c create_signal() call will determine the
       * signature of the signal created.
       */
      template <LIST(class T_return, LOOP(class T_arg%1, [$1]))>
      DBusCxxPointer<signal<LIST(T_return, LOOP(T_arg%1, $1))> >
      create_signal( const std::string& iface, const std::string& name );
])
    
define([DEFINE_CREATE_SIGNAL_IN],[dnl
  template <LIST(class T_return, LOOP(class T_arg%1, [$1]))>
  DBusCxxPointer<signal<LIST(T_return, LOOP(T_arg%1, $1))> >
  Object::create_signal( const std::string& iface, const std::string& name )
  {
    DBusCxxPointer<DBus::signal<LIST(T_return, LOOP(T_arg%1, $1))> > sig;
    if ( not has_interface(iface) ) this->create_interface(iface);
    sig = this->interface(iface)->create_signal<LIST(T_return, LOOP(T_arg%1, $1))>(name);
    return sig;
  }
])
    
divert(0)
dnl
[/***************************************************************************
 *   Copyright (C) 2009,2010 by Rick L. Vinyard, Jr.                       *
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

#include <dbus-cxx/forward_decls.h>
#include <dbus-cxx/objectpathhandler.h>
#include <dbus-cxx/dbus_signal.h>

#ifndef DBUSCXXOBJECT_H
#define DBUSCXXOBJECT_H

namespace DBus
{
  /**
   * @example calculator_server.cpp
   *
   * This example is one part of three example applications that demonstrate
   * client, server and watcher applications that use adapters and proxies
   * generated by dbus-cxx-xml2cpp from a modified dbus introspection XML
   * document.
   *
   * These three examples are:
   * <ul>
   * <li> \c calculator_server.cpp
   * <li> \c calculator_client.cpp
   * <li> \c calculator_watcher.cpp
   * </ul>
   *
   * This particular piece is the server that uses the generated Object
   * derived class to provide an adapter interface for the Calculator
   * class.
   *
   * Here is the calculator class, which by itself knows nothing of dbus:
   *
   * @include xml2cpp/calculator/calculator.h
   * @include xml2cpp/calculator/calculator.cpp
   *
   * The adapter is generated with this command:
   * @code
   * dbus-cxx-xml2cpp --xml calculator.xml --adapter -f
   * @endcode
   *
   * The modified introspection XML document is here:
   * @include xml2cpp/calculator/calculator.xml
   *
   * Here is the generated adapter:
   * @include xml2cpp/calculator/calculator_adapter.h
   *
   * And here is the server application:
   *
   */

  /**
   * @defgroup local Local Objects
   *
   * This group contains objects which are natively local and
   * provide interfaces such as methods and signals that can
   * be exported to dbus.
   */

  /**
   * @defgroup objects Objects
   */

  /**
   * @ingroup local
   * @ingroup objects
   *
   * @todo TODO rethink whether it might be beneficial to have multiple connections for one object
   * 
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class Object: public ObjectPathHandler
  {
    protected:

      /**
       * This class has a protected constructor. Use the \c create() methods
       * to obtain a smart pointer to a new instance.
       */
      Object( const std::string& path, PrimaryFallback pf=PRIMARY );

    public:

      /**
       * Typedef to smart pointers to Object.
       * 
       * Can access \e type as \c Object::pointer
       */
      typedef DBusCxxPointer<Object> pointer;

      /**
       * Typedef to the storage structure for an \c Object instance's
       * interfaces.
       *
       * \b Data \b Structure - Multimap: There is no restriction that interfaces must be uniquely named
       * \b Key - interface name
       * \b Value -smart pointer to an interface.
       *
       * Can access \e type as \c Object::Interfaces
       */
      typedef std::multimap<std::string, DBusCxxPointer<Interface> > Interfaces;

      /**
       * Typedef to storage structure for an \c Object instance's
       * children. The \e key is the child's name and the \e value
       *
       * \b Data \b Structure - map: an object's children must be uniquely named
       * \b Key - child name
       * \b Value -smart pointer to a child object.
       *
       * Can access \e type as \c Object::Children
       */
      typedef std::map<std::string, Object::pointer> Children;

      /**
       * Creates a named Object that will register as a primary or fallback handler
       * @param path The path the object will handle
       * @param pf Determines whether the Object will registar as a primary or fallback handler (\e default = \c PRIMARY)
       */
      static pointer create( const std::string& path = std::string(), PrimaryFallback pf=PRIMARY );

      virtual ~Object();

      /** Extends base version to include registering signals */
      virtual bool register_with_connection(DBusCxxPointer<Connection> conn);

      /** Get all the interfaces associated with this Object instance */
      const Interfaces& interfaces() const;

      /** Returns the first interface with the given name */
      DBusCxxPointer<Interface> interface( const std::string& name ) const;

      /** Adds the interface to this object */
      bool add_interface( DBusCxxPointer<Interface> interface );

      /**
       * Creates and adds the named interface to this object
       *
       * @return the newly created interface
       */
      DBusCxxPointer<Interface> create_interface( const std::string& name );

FOR(0, eval(CALL_SIZE),[[DECLARE_CREATE_METHOD(%1)
]])
FOR(0, eval(CALL_SIZE),[[DECLARE_CREATE_INTERFACE_METHOD(%1)
]])
      /** Removes the first interface found with the given name */
      void remove_interface( const std::string& name );

      /**
       * Test whether an Object has a named interface
       * @return \c True if the object has the named interface, \c false otherwise
       * @param name The name of the interface to look for
       */
      bool has_interface( const std::string& name );

      /**
       * Get the default interface associated with this object
       * @return The default interface, or a null smart pointer if no default is set
       */
      DBusCxxPointer<Interface> default_interface() const;

      /**
       * Set the default interface to a specific name
       *
       * The first interface found with a matching name is used. If there is
       * already a default interface set it will be replaced.
       * 
       * @return \c True if an interface with the specified name was found, \c false otherwise.
       * @param new_default_name The name of the interface to use as the default.
       */
      bool set_default_interface( const std::string& new_default_name );

      /**
       * Removes the currently set (if any) default interface. There wil not
       * be a default interface set.
       */
      void remove_default_interface();

FOR(0, eval(CALL_SIZE),[[DECLARE_CREATE_SIGNAL_N(%1)
]])
FOR(0, eval(CALL_SIZE),[[DECLARE_CREATE_SIGNAL_IN(%1)
]])

      /** Get the children associated with this object instance */
      const Children& children() const;

      /**
       * Get a named child of this object
       * @return A smart pointer to a child with the specified name, or a null smart pointer if no child found.
       * @param name The name of the child to return
       */
      Object::pointer child(const std::string& name) const;

      /**
       * Add an object as a child with a specified name
       * This method will fail if the object already has a child with the
       * specified name and \c force is not set.
       * @return \c True if the child was successfully added, \c false otherwise.
       * @param name The name to use for the child.
       * @param child A smart pointer to an object to add as a child.
       * @param force If \c true a child with the same name will be replaced.
       */
      bool add_child(const std::string& name, Object::pointer child, bool force=false);

      /**
       * Remove the named child from this object
       * @return \c True if the child was found and removed, \c false if no child by the name was found to remove.
       * @param name The name of the child to remove.
       */
      bool remove_child(const std::string& name);

      /**
       * Test whether an object has a child with a specified name
       * @return \c True if the object has the named child, \c false otherwise.
       * @param name The name of the child to test.
       */
      bool has_child(const std::string& name) const;

      /** Returns a DBus XML description of this interface */
      std::string introspect(int space_depth=0) const;

      /**
       * Signal emitted when an interface is added to this object.
       *
       * The first parameter of the callback is a pointer to the newly added interface.
       */
      sigc::signal<void,DBusCxxPointer<Interface> > signal_interface_added();

      /**
       * Signal emitted when an interface is removed from this object.
       *
       * The first parameter of the callback is a pointer to the removed interface.
       */
      sigc::signal<void,DBusCxxPointer<Interface> > signal_interface_removed();

      /**
       * Signal emitted when the default interface of this object is changed.
       *
       * The first parameter of the callback is a pointer to the old default
       * interface, and the second parameter is a callback to the new default
       * interface.
       */
      sigc::signal<void,DBusCxxPointer<Interface> /*old default*/,DBusCxxPointer<Interface> /*new default*/> signal_default_interface_changed();

      /**
       * Handles the specified message on the specified connection
       *
       * If \c msg is an introspection message, the object will rely on its
       * \c introspection() method to provide a reply.
       *
       * Looks for interfaces specified in the message first. If the message
       * does not specify an interface or the specified interface is not found
       * the default interface will be used.
       *
       * If the specified interface is found, all matching interfaces will be
       * tried until an interface handler returns \c HANDLED. Once an interface
       * returns \c HANDLED no further interfaces will be tried.
       *
       * If no interface returns \c HANDLED the default interface (if one is
       * set) will be tried.
       * 
       * @return \c HANDLED if this object handled the message, \c NOT_HANDLED otherwise
       * @param conn The Connection to send the reply message on
       * @param msg The message to handle; must be a CallMessage or it will not be handled
       */
      virtual HandlerResult handle_message( DBusCxxPointer<Connection> conn, Message::const_pointer msg);

    protected:

      Children m_children;
      
      mutable pthread_rwlock_t m_interfaces_rwlock;

      pthread_mutex_t m_name_mutex;

      Interfaces m_interfaces;

      DBusCxxPointer<Interface>  m_default_interface;

      sigc::signal<void,DBusCxxPointer<Interface> ,DBusCxxPointer<Interface> > m_signal_default_interface_changed;

      sigc::signal<void,DBusCxxPointer<Interface> > m_signal_interface_added;

      sigc::signal<void,DBusCxxPointer<Interface> > m_signal_interface_removed;

      typedef std::map<DBusCxxPointer<Interface> ,sigc::connection> InterfaceSignalNameConnections;

      InterfaceSignalNameConnections m_interface_signal_name_connections;

      /**
       * Callback point that updates the interface name map when an interface
       * changes its name.
       */
      void on_interface_name_changed(const std::string& oldname, const std::string& newname, DBusCxxPointer<Interface>  interface);

  };

}

#include <dbus-cxx/interface.h>

/************************
 * Template definitions *
 ************************/

namespace DBus {

FOR(0, eval(CALL_SIZE),[[DEFINE_CREATE_METHOD(%1)
]])
FOR(0, eval(CALL_SIZE),[[DEFINE_CREATE_INTERFACE_METHOD(%1)
]])
FOR(0, eval(CALL_SIZE),[[DEFINE_CREATE_SIGNAL_N(%1)
]])
FOR(0, eval(CALL_SIZE),[[DEFINE_CREATE_SIGNAL_IN(%1)
]])

}


#endif

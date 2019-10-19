/***************************************************************************
 *   Copyright (C) 2009,2010 by Rick L. Vinyard, Jr.                       *
 *   rvinyard@cs.nmsu.edu                                                  *
 *   Copyright (C) 2019 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
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
#include <sigc++/sigc++.h>

#include <string>
#include <map>
#include <shared_mutex>

#include <dbus-cxx/forward_decls.h>
#include <dbus-cxx/objectpathhandler.h>
#include <dbus-cxx/dbus_signal.h>

#ifndef DBUSCXXOBJECT_H
#define DBUSCXXOBJECT_H

namespace DBus
{

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
      typedef std::shared_ptr<Object> pointer;

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
      typedef std::multimap<std::string, std::shared_ptr<Interface> > Interfaces;

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
      virtual bool register_with_connection(std::shared_ptr<Connection> conn);

      /** Get all the interfaces associated with this Object instance */
      const Interfaces& interfaces() const;

      /** Returns the first interface with the given name */
      std::shared_ptr<Interface> interface( const std::string& name ) const;

      /** Adds the interface to this object */
      bool add_interface( std::shared_ptr<Interface> interface );

      /**
       * Creates and adds the named interface to this object
       *
       * @return the newly created interface
       */
      std::shared_ptr<Interface> create_interface( const std::string& name );

      /**
       * Creates a method with a signature based on the @param slot parameter signature and adds it to the default interface
       * @return A smart pointer to the newly created method
       * @param slot This slot will be called with the given signature when the method is invoked
       *
       * Template parameters of the sigc::slot will determine the signature of
       * the method created.
       */
      template <typename T_type>
      std::shared_ptr<Method<T_type> >
      create_method( const std::string& method_name, sigc::slot<T_type> slot );

      /**
       * Creates a method with a signature based on the @param slot parameter signature and adds it to the named interface
       * @return A smart pointer to the newly created method
       * @param interface_name The DBus interface, in org.example.Foo style
       * @param method_name The name of the method that this is for.
       * @param slot This slot will be called with the given signature when the method is invoked
       *
       * Template parameters of the sigc::slot will determine the signature of
       * the method created.
       */
      template <typename T_type>
      std::shared_ptr<Method<T_type> >
      create_method( const std::string& interface_name, const std::string& method_name, sigc::slot<T_type> slot );

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
      std::shared_ptr<Interface> default_interface() const;

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

      /**
       * Creates a signal with a return value (possibly \c void ) and $1 parameters and adds it to the default interface
       * @return A smart pointer to the newly created signal
       *
       * Template parameters for the \c create_signal() call will determine the
       * signature of the signal created.
       */
      template <class... T_type>
      std::shared_ptr<signal<T_type...> >
      create_signal( const std::string& name );

      /**
       * Creates a signal with a return value (possibly \c void ) and $1 parameters and adds it to the named interface
       * @return A smart pointer to the newly created signal
       *
       * Template parameters for the \c create_signal() call will determine the
       * signature of the signal created.
       */
      template <class... T_type>
      std::shared_ptr<signal<T_type...> >
      create_signal( const std::string& iface, const std::string& name );

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
      sigc::signal<void(std::shared_ptr<Interface>) > signal_interface_added();

      /**
       * Signal emitted when an interface is removed from this object.
       *
       * The first parameter of the callback is a pointer to the removed interface.
       */
      sigc::signal<void(std::shared_ptr<Interface>) > signal_interface_removed();

      /**
       * Signal emitted when the default interface of this object is changed.
       *
       * The first parameter of the callback is a pointer to the old default
       * interface, and the second parameter is a callback to the new default
       * interface.
       */
      sigc::signal<void(std::shared_ptr<Interface> /*old default*/,std::shared_ptr<Interface> /*new default*/)> signal_default_interface_changed();

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
      virtual HandlerResult handle_message( std::shared_ptr<Connection> conn, Message::const_pointer msg);

    protected:

      Children m_children;
      
      mutable std::shared_mutex m_interfaces_rwlock;

      std::mutex m_name_mutex;

      Interfaces m_interfaces;

      std::shared_ptr<Interface>  m_default_interface;

      sigc::signal<void(std::shared_ptr<Interface>,std::shared_ptr<Interface>) > m_signal_default_interface_changed;

      sigc::signal<void(std::shared_ptr<Interface>) > m_signal_interface_added;

      sigc::signal<void(std::shared_ptr<Interface>) > m_signal_interface_removed;

      typedef std::map<std::shared_ptr<Interface> ,sigc::connection> InterfaceSignalNameConnections;

      InterfaceSignalNameConnections m_interface_signal_name_connections;

      /**
       * Callback point that updates the interface name map when an interface
       * changes its name.
       */
      void on_interface_name_changed(const std::string& oldname, const std::string& newname, std::shared_ptr<Interface>  interface);

  };

}

#include <dbus-cxx/interface.h>

/************************
 * Template definitions *
 ************************/

namespace DBus {

  template <typename T_type>
  std::shared_ptr<Method<T_type> >
  Object::create_method( const std::string& method_name, sigc::slot<T_type> slot )
  {
    if ( not m_default_interface )
    {
      this->create_interface("");
      this->set_default_interface("");
    }
    // TODO throw an error if the default interface still doesn't exist

    std::shared_ptr< Method<T_type> > method;
    method = m_default_interface->create_method<T_type>(method_name);
    method->set_method( slot );
    return method;
  }

  template <typename T_type>
  std::shared_ptr<Method<T_type> >
  Object::create_method( const std::string& interface_name, const std::string& method_name, sigc::slot<T_type> slot )
  {
    Interface::pointer interface;
    interface = this->interface(interface_name);
    if ( not interface ) interface = this->create_interface(interface_name);
    // TODO throw an error if the interface still doesn't exist

    std::shared_ptr< Method<T_type> > method;
    method = interface->create_method<T_type>(method_name);
    method->set_method( slot );
    return method;
  }

  template <class... T_type>
  std::shared_ptr<signal<T_type...> >
  Object::create_signal( const std::string& name )
  {
    std::shared_ptr<DBus::signal<T_type...> > sig;
    Interface::pointer iface = this->default_interface();
    if ( not iface ) iface = this->create_interface("");
    sig = iface->create_signal<T_type...>(name);
    return sig;
  }

  template <class... T_type>
  std::shared_ptr<signal<T_type...> >
  Object::create_signal( const std::string& iface, const std::string& name )
  {
    std::shared_ptr<DBus::signal<T_type...> > sig;
    if ( not has_interface(iface) ) this->create_interface(iface);
    sig = this->interface(iface)->create_signal<T_type...>(name);
    return sig;
  }

}

#endif

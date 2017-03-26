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

define([CREATE_METHOD],[dnl
      /**
       * Creates a proxy method with a signature based on the template parameters and adds it to the named interface
       * @return A smart pointer to the newly created method proxy
       * @param interface_name The name of the interface to add this proxy method to
       * @param method_name The name to assign to the newly create method proxy. This name will be used to perform the dbus-call.
       */
      template <LIST(class T_return, LOOP(class T_arg%1, [$1]))>
      DBusCxxPointer<MethodProxy<LIST(T_return, LOOP(T_arg%1, $1))> >
      create_method( const std::string& interface_name, const std::string& method_name )
      {
        InterfaceProxy::pointer interface = this->interface(interface_name);
        if ( not interface ) interface = this->create_interface( interface_name );
        return interface->create_method<LIST(T_return, LOOP(T_arg%1, $1))>(method_name);
      }

])
    
define([CREATE_SIGNAL],[dnl
      /**
       * Creates a signal proxy with a signature based on the template parameters and adds it to the named interface
       * @return A smart pointer to the newly created signal proxy
       * @param interface_name The name of the interface to add this proxy signal to
       * @param sig_name The name to assign to the newly created signal proxy.
       */
      template <LIST(class T_return, LOOP(class T_arg%1, [$1]))>
      DBusCxxPointer<signal_proxy<LIST(T_return, LOOP(T_arg%1, $1))> >
      create_signal( const std::string& interface_name, const std::string& sig_name )
      {
        InterfaceProxy::pointer interface = this->interface(interface_name);
        if ( not interface ) interface = this->create_interface( interface_name );
        return interface->create_signal<LIST(T_return, LOOP(T_arg%1, $1))>(sig_name);
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

#include <dbus-cxx/signal_proxy.h>
#include <dbus-cxx/interfaceproxy.h>

#ifndef DBUSCXX_OBJECTPROXY_H
#define DBUSCXX_OBJECTPROXY_H

namespace DBus
{

  class Connection;

  /**
   * @example calculator_client.cpp
   *
   * This example is one part of three example applications that demonstrate
   * client, server and watcher applications that use adapters and proxies
   * generated by dbus-cxx-xml2cpp from a modified dbus introspection XML
   * document.
   *
   * These three examples are:
   * <ul>
   * <li> @c calculator_server.cpp
   * <li> @c calculator_client.cpp
   * <li> @c calculator_watcher.cpp
   * </ul>
   *
   * This particular piece is the client that uses the generated ObjectProxy
   * derived class to call upon the calculator server to perform calculations.
   *
   * The client and watcher both use the same proxy generated from the XML
   * document (see the calculator server example for the modified document).
   *
   * The proxy is generated with this command:
   * @code
   * dbus-cxx-xml2cpp --xml calculator.xml --proxy -f
   * @endcode
   *
   * Here is the generated proxy file:
   * @include xml2cpp/calculator/calculator_proxy.h
   *
   * And here is the client application:
   * 
   */

  /**
   * @example calculator_watcher.cpp
   *
   * This example is one part of three example applications that demonstrate
   * client, server and watcher applications that use adapters and proxies
   * generated by dbus-cxx-xml2cpp from a modified dbus introspection XML
   * document.
   *
   * These three examples are:
   * <ul>
   * <li> @c calculator_server.cpp
   * <li> @c calculator_client.cpp
   * <li> @c calculator_watcher.cpp
   * </ul>
   *
   * This particular piece is the watcher that uses the generated ObjectProxy
   * derived class to observe calculation signals from the server.
   *
   * The client and watcher both use the same proxy generated from the XML
   * document (see the calculator server example for the modified document).
   *
   * The proxy is generated with this command:
   * @code
   * dbus-cxx-xml2cpp --xml calculator.xml --proxy -f
   * @endcode
   *
   * Here is the generated proxy file:
   * @include xml2cpp/calculator/calculator_proxy.h
   *
   * And here is the watcher application:
   *
   */

  /**
   * @example hal_proxy.cpp
   *
   * This is an example of an ObjectProxy generated by dbus-cxx-xml2cpp from a
   * modified dbus introspection XML document.
   *
   * \par dbus-cxx-xml2cpp
   *
   * \par
   * The command used to generate hal_manager_proxy.h from
   * hal_manager_introspection.xml is:
   * @code
   * dbus-cxx-xml2cpp --xml hal_manager_introspection.xml --proxy -f --prefix="hal_"
   * @endcode
   *
   * \par Modifying the DBus introspected XML
   *
   * \par
   * dbus-cxx-xml2cpp uses a modified XML document obtained via introspection.
   * In this example several attributes were added to the node tag and the
   * introspection interface tag.
   *
   * \par
   * The node tag was changed from this:
   * @code
   * <node>
   * @endcode
   * to this:
   * @code
   * <node gen-namespace="DBus::Hal" cppname="Manager" dest="org.freedesktop.Hal" path="/org/freedesktop/Hal/Manager" >
   * @endcode
   *
   * \par
   * The introspection interface tag had the @em ignored attribute added to
   * prevent the interface from being built changing it from this:
   * @code
   * <interface name="org.freedesktop.DBus.Introspectable">
   * @endcode
   * to this:
   * @code
   * <interface name="org.freedesktop.DBus.Introspectable" ignored="1">
   * @endcode
   *
   * \par Source xml
   * Here is the modified xml document:
   * @include xml2cpp/hal-proxy/hal_manager_introspection.xml
   *
   * \par Generated Proxy Object
   * After running dbus-cxx-xml2cpp results in a .h file containing the definition
   * of the proxy class. Here is the resulting hal_manager_proxy.h:
   * @include xml2cpp/hal-proxy/hal_manager_proxy.h
   *
   * And finally, here is the example application that uses the proxy to watch for
   * device added/removed signals.
   *
   */

  /**
   * @defgroup proxy Proxy
   *
   * This group contains items that are of interest in creating local proxies
   * for dbus objects, interfaces, methods and signals.
   */

  /**
   * Object proxies are local proxies that provide local methods and signals for
   * remote objects with dbus interfaces.
   *
   * @ingroup proxy
   * @ingroup objects
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class ObjectProxy
  {
    protected:

      /**
       * This class has a protected constructor. Use the \c create() methods
       * to obtain a smart pointer to a new instance.
       */
      ObjectProxy( DBusCxxPointer<Connection> conn, const std::string& destination, const std::string& path );

    public:

      /**
       * Typedef to smart pointers to ObjectProxy.
       * 
       * Can access \e type as \c ObjectProxy::pointer
       */
      typedef DBusCxxPointer<ObjectProxy> pointer;

      /**
       * Creates an ObjectProxy with a specific path
       * @param path The path of the object that this will be a proxy for
       */
      static pointer create( const std::string& path );

      /**
       * Creates an ObjectProxy with a specific path
       * @param destination The destination address that this will be a proxy for
       * @param path The path of the object that this will be a proxy for
       */
      static pointer create( const std::string& destination, const std::string& path );

      static pointer create( DBusCxxPointer<Connection> conn, const std::string& path );

      static pointer create( DBusCxxPointer<Connection> conn, const std::string& destination, const std::string& path );

      virtual ~ObjectProxy();

      DBusCxxPointer<Connection> connection() const;

      void set_connection( DBusCxxPointer<Connection> conn );

      const std::string& destination() const;

      void set_destination( const std::string& destination );

      const Path& path() const;

      void set_path( const std::string& path );

      typedef std::multimap<std::string, InterfaceProxy::pointer> Interfaces;

      const Interfaces& interfaces() const;

      /** Returns the first interface with the given name */
      InterfaceProxy::pointer interface( const std::string& name ) const;

      /** Alias for interface(name) */
      InterfaceProxy::pointer operator[[]]( const std::string& name ) const;

      /** Adds the interface to this object */
      bool add_interface( InterfaceProxy::pointer interface );

      /**
       * Creates and adds the named interface to this object
       *
       * @return the newly created interface
       */
      InterfaceProxy::pointer create_interface( const std::string& name );

      /** Removes the first interface with the given name */
      void remove_interface( const std::string& name );

      /** Removes the given interface */
      void remove_interface( InterfaceProxy::pointer interface );

      bool has_interface( const std::string& name ) const;

      bool has_interface( InterfaceProxy::pointer interface ) const;

      InterfaceProxy::pointer default_interface() const;

      bool set_default_interface( const std::string& new_default_name );

      bool set_default_interface( InterfaceProxy::pointer new_default );

      void remove_default_interface();

      /** Adds the method to the named interface */
      bool add_method( const std::string& interface, MethodProxyBase::pointer method );

      /** Adds the method to the default interface */
      bool add_method( MethodProxyBase::pointer method );

      CallMessage::pointer create_call_message( const std::string& interface_name, const std::string& method_name ) const;

      CallMessage::pointer create_call_message( const std::string& method_name ) const;

      ReturnMessage::const_pointer call( CallMessage::const_pointer, int timeout_milliseconds=-1 ) const;

      PendingCall::pointer call_async( CallMessage::const_pointer, int timeout_milliseconds=-1 ) const;

FOR(0, eval(CALL_SIZE),[[CREATE_METHOD(%1)]])
FOR(0, eval(CALL_SIZE),[[CREATE_SIGNAL(%1)]])
      sigc::signal<void,InterfaceProxy::pointer> signal_interface_added();

      sigc::signal<void,InterfaceProxy::pointer> signal_interface_removed();

      sigc::signal<void,InterfaceProxy::pointer/*old default*/,InterfaceProxy::pointer/*new default*/> signal_default_interface_changed();

    protected:

      DBusCxxPointer<Connection> m_connection;

      std::string m_destination;

      Path m_path;
      
      mutable pthread_rwlock_t m_interfaces_rwlock;

      pthread_mutex_t m_name_mutex;

      Interfaces m_interfaces;

      InterfaceProxy::pointer m_default_interface;

      sigc::signal<void,InterfaceProxy::pointer,InterfaceProxy::pointer> m_signal_default_interface_changed;

      sigc::signal<void,InterfaceProxy::pointer> m_signal_interface_added;

      sigc::signal<void,InterfaceProxy::pointer> m_signal_interface_removed;

      typedef std::map<InterfaceProxy::pointer,sigc::connection> InterfaceSignalNameConnections;

      InterfaceSignalNameConnections m_interface_signal_name_connections;

      void on_interface_name_changed(const std::string& oldname, const std::string& newname, InterfaceProxy::pointer interface);

  };

}

#endif

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

define([CREATE_SIGNAL_PROXY_IN],[dnl
      dnl
template <LIST(class T_return, LOOP(class T_arg%1, [$1]))>
      DBusCxxPointer<signal_proxy<LIST(T_return, LOOP(T_arg%1, $1))> > create_signal_proxy( const std::string& interface, const std::string& name )
      {
        DBusCxxPointer<signal_proxy<LIST(T_return, LOOP(T_arg%1, $1))> > sig;
        sig = signal_proxy<LIST(T_return, LOOP(T_arg%1, $1))>::create(interface, name);
        this->add_signal_proxy( sig );
        return sig;
      }
      dnl
])
    
define([CREATE_SIGNAL_PROXY_PIN],[dnl
      dnl
template <LIST(class T_return, LOOP(class T_arg%1, [$1]))>
      DBusCxxPointer<signal_proxy<LIST(T_return, LOOP(T_arg%1, $1))> > create_signal_proxy( const std::string& path, const std::string& interface, const std::string& name )
      {
        DBusCxxPointer<signal_proxy<LIST(T_return, LOOP(T_arg%1, $1))> > sig;
        sig = signal_proxy<LIST(T_return, LOOP(T_arg%1, $1))>::create(path, interface, name);
        this->add_signal_proxy( sig );
        return sig;
      }
      dnl
])
    
define([CREATE_SIGNAL_IN],[dnl
      dnl
template <LIST(class T_return, LOOP(class T_arg%1, [$1]))>
      DBusCxxPointer<signal<LIST(T_return, LOOP(T_arg%1, $1))> > create_signal( const std::string& interface, const std::string& name )
      {
        DBusCxxPointer<signal<LIST(T_return, LOOP(T_arg%1, $1))> > sig;
        sig = signal<LIST(T_return, LOOP(T_arg%1, $1))>::create(interface, name);
        sig->set_connection(this->self());
        return sig;
      }
      dnl
])
    
define([CREATE_SIGNAL_PIN],[dnl
      dnl
template <LIST(class T_return, LOOP(class T_arg%1, [$1]))>
      DBusCxxPointer<signal<LIST(T_return, LOOP(T_arg%1, $1))> > create_signal( const std::string& path, const std::string& interface, const std::string& name )
      {
        DBusCxxPointer<signal<LIST(T_return, LOOP(T_arg%1, $1))> > sig;
        sig = signal<LIST(T_return, LOOP(T_arg%1, $1))>::create(path, interface, name);
        sig->set_connection(this->self());
        return sig;
      }
      dnl
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
#include <list>
#include <deque>

#include <dbus-cxx/pointer.h>
#include <dbus-cxx/message.h>
#include <dbus-cxx/returnmessage.h>
#include <dbus-cxx/pendingcall.h>
#include <dbus-cxx/watch.h>
#include <dbus-cxx/timeout.h>
#include <dbus-cxx/accumulators.h>
#include <dbus-cxx/object.h>
#include <dbus-cxx/objectproxy.h>
#include <dbus-cxx/signal_proxy.h>
#include <dbus-cxx/dbus_signal.h>
#include <dbus-cxx/messagefilter.h>
#include <dbus-cxx/method.h>

#include <iostream>

#ifndef DBUSCXX_CONNECTION_H
#define DBUSCXX_CONNECTION_H

namespace DBus
{

  class Object;
  class signal_base;
  class PendingCall;

  /**
   * Connection point to the DBus
   *
   * @ingroup core
   *
   * @todo TODO need to deal with signal proxies that change name and/or interface
   *
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class Connection
  {
    
    protected:
    
      // TODO dbus_connection_open

      Connection( DBusConnection* cobj = NULL, bool is_private=false );

      /**
       * Connects to a bus daemon and registers the client with it.
       *
       * @param is_private if true a private connection will be created. Otherwise a shared connection is created.
       */
      Connection( BusType type, bool is_private=false );

      Connection( const Connection& other );

    public:
      
      typedef DBusCxxPointer<Connection> pointer;
      
      typedef DBusCxxWeakPointer<Connection> weak_pointer;

      // TODO dbus_connection_open

      static pointer create( DBusConnection* cobj = NULL, bool is_private=false );

      /**
       * Connects to a bus daemon and registers the client with it.
       *
       * @param is_private if true a private connection will be created. Otherwise a shared connection is created.
       */
      static pointer create( BusType type, bool is_private=false );

      static pointer create( const Connection& other );

      virtual ~Connection();

      /** True if this is a valid connection; false otherwise */
      operator bool() const;

      /** True if this is a valid connection; false otherwise */
      bool is_valid() const;

      /** True if this is a valid connection @e and is private; false otherwise */
      bool is_private() const;

      /** Registers this connection with the bus */
      bool bus_register();

      /** True if this connection is already registered */
      bool is_registered() const;

      /** Gets the unique name of the connection as assigned by the message bus. */
      const char* unique_name() const;

      // TODO set_unique_name() 

      /**
       * The unix user id associated with the name connection or -1 if an
       * error occurred
       */
      unsigned long unix_user( const std::string& name ) const;

      /** The bus' globally unique ID, as described in the D-Bus specification */
      const char* bus_id() const;

      int request_name( const std::string& name, unsigned int flags = 0 );

      int release_name( const std::string& name );

      bool name_has_owner( const std::string& name ) const;

      StartReply start_service( const std::string& name, uint32_t flags ) const;

      bool add_match( const std::string& rule );

      void add_match_nonblocking( const std::string& rule );

      bool remove_match( const std::string& rule );

      void remove_match_nonblocking( const std::string& rule );

      // TODO dbus_connection_close 

      bool is_connected() const;

      bool is_authenticated() const;

      bool is_anonymous() const;

      const char* server_id() const;

      // TODO dbus_connection_preallocate_send
      // TODO dbus_connection_free_preallocated_send
      // TODO dbus_connection_send_preallocated

      uint32_t send( const Message::const_pointer );

      /**
       * Sends the message on the connection
       */
      Connection& operator<<( Message::const_pointer msg );

      PendingCall::pointer send_with_reply_async( Message::const_pointer message, int timeout_milliseconds=-1 ) const;

      ReturnMessage::const_pointer send_with_reply_blocking( Message::const_pointer msg, int timeout_milliseconds=-1 ) const;

      void flush();

      bool read_write_dispatch( int timeout_milliseconds=-1 );

      bool read_write( int timeout_milliseconds=-1 );

      Message::pointer borrow_message();

      void return_message( Message::pointer message );

      void steal_borrowed_message( Message::pointer message );

      Message::pointer pop_message( );

      DispatchStatus dispatch_status( ) const;

      DispatchStatus dispatch( );

      int unix_fd() const;

      int socket() const;

      unsigned long unix_user() const;

      unsigned long unix_process_id() const;

      // TODO dbus_connection_get_adt_audit_session_data

      // TODO dbus_connection_set_unix_user_function

      // TODO dbus_connection_get_windows_user

      // TODO dbus_connection_set_windows_user_function

      void set_allow_anonymous( bool allow=true );

      void set_route_peer_messages( bool route=true );

      // TODO dbus_connection_try_register_object_path
      // TODO dbus_connection_register_object_path
      // TODO dbus_connection_try_register_fallback
      // TODO dbus_connection_register_fallback
      // TODO dbus_connection_unregister_object_path
      // TODO dbus_connection_get_object_path_data
      // TODO dbus_connection_list_registered

      void set_max_message_size( long size );

      long max_message_size();

      void set_max_received_size( long size );

      long max_received_size();

      long outgoing_size();

      bool has_messages_to_send();

      typedef sigc::signal1<bool,Watch::pointer,InterruptablePredicateAccumulatorDefaultFalse> AddWatchSignal;

      /** Cannot call watch.handle() in a slot connected to this signal */
      AddWatchSignal& signal_add_watch();

      sigc::signal<bool,Watch::pointer>& signal_remove_watch();

      sigc::signal<bool,Watch::pointer>& signal_watch_toggled();

      typedef sigc::signal1<bool,Timeout::pointer,InterruptablePredicateAccumulatorDefaultFalse> AddTimeoutSignal;
      
      /** Cannot call timeout.handle() in a slot connected to this signal */
      AddTimeoutSignal& signal_add_timeout();

      sigc::signal<bool,Timeout::pointer>& signal_remove_timeout();

      sigc::signal<bool,Timeout::pointer>& signal_timeout_toggled();

      sigc::signal<void>& signal_wakeup_main();

      /** Cannot call dispatch() in a slot connected to this signal */
      sigc::signal<void,DispatchStatus>& signal_dispatch_status_changed();

      /**
       * Signal emitted during dispatch. A slot returning true will cause the
       * message to be filtered.
       */
      FilterSignal& signal_filter();

      typedef std::deque<Watch::pointer> Watches;
      
      const Watches& unhandled_watches() const;
      
      void remove_unhandled_watch(const Watch::pointer w);

      Object::pointer create_object( const std::string& path, PrimaryFallback pf=PRIMARY );

      bool register_object( Object::pointer object );

      ObjectPathHandler::pointer create_object( const std::string& path, sigc::slot<HandlerResult,Connection::pointer,Message::const_pointer>& slot, PrimaryFallback pf=PRIMARY );

      ObjectPathHandler::pointer create_object( const std::string& path, HandlerResult (*MessageFunction)(Connection::pointer,Message::const_pointer), PrimaryFallback pf=PRIMARY );

      ObjectProxy::pointer create_object_proxy( const std::string& path );

      ObjectProxy::pointer create_object_proxy( const std::string& destination, const std::string& path );

      bool unregister_object( const std::string& path );

      /**
       * Adds a signal with the given interface and name
       *
       * @return Smart pointer to the newly added signal or a null smart pointer if it couldn't be added
       */
      signal_proxy_simple::pointer create_signal_proxy( const std::string& interface, const std::string& name );

      /**
       * Adds a signal with the given path, interface and name
       *
       * @return Smart pointer to the newly added signal or a null smart pointer if it couldn't be added
       */
      signal_proxy_simple::pointer create_signal_proxy( const std::string& path, const std::string& interface, const std::string& name );

FOR(0, eval(CALL_SIZE),[[CREATE_SIGNAL_PROXY_IN(%1)
]])
      
FOR(0, eval(CALL_SIZE),[[CREATE_SIGNAL_PROXY_PIN(%1)
]])

      /**
       * Adds the given signal proxy to the connection
       */
      signal_proxy_base::pointer add_signal_proxy( signal_proxy_base::pointer signal );

      bool remove_signal_proxy( signal_proxy_base::pointer proxy );

      typedef std::list<signal_proxy_base::pointer> ProxySignals;
      
      typedef std::map<std::string,ProxySignals> NameToProxySignalMap;

      typedef std::map<std::string,NameToProxySignalMap> InterfaceToNameProxySignalMap;

      /** Gets all the signal handlers */
      const InterfaceToNameProxySignalMap& get_signal_proxies();

      /** Gets the signal handlers for a specific interface */
      NameToProxySignalMap get_signal_proxies( const std::string& interface );

      /** Gets the signal handlers for a specific interface and member */
      ProxySignals get_signal_proxies( const std::string& interface, const std::string& member );

FOR(0, eval(CALL_SIZE),[[CREATE_SIGNAL_IN(%1)
]])
      
FOR(0, eval(CALL_SIZE),[[CREATE_SIGNAL_PIN(%1)
]])

      //       bool register_object( Object& obj, const std::string & path );
//
//       bool register_signal( signal_base& );

      /** Returns a smart pointer to itself */
      pointer self();

      /**
       * Given a dbus connection, if it was established through a call to
       * Connection::create() this method will return a copy of the smart
       * pointer returned by the create method.
       *
       * This is accomplished safely because the create() methods store a weak
       * pointer in a dbus data slot associated with the DBusConnection C
       * object, and this method creates the smart pointer from the weak pointer
       * if the weak pointer is still valid.
       *
       * If the DBusConnection was created outside a Connection::create() method
       * or the underlying object has already been deleted this method will
       * return an empty pointer.
       */
      static pointer self(DBusConnection* c);
      
      DBusConnection* cobj();

      static void set_global_change_sigpipe(bool will_modify_sigpipe=true);

      std::string introspect( const std::string& destination, const std::string& path );

    protected:
      DBusConnection* m_cobj;
      
      bool m_private_flag;
      
      AddWatchSignal m_add_watch_signal;
      
      sigc::signal<bool,Watch::pointer> m_remove_watch_signal;
      
      sigc::signal<bool,Watch::pointer> m_watch_toggled_signal;
      
      AddTimeoutSignal m_add_timeout_signal;
      
      sigc::signal<bool,Timeout::pointer> m_remove_timeout_signal;
      
      sigc::signal<bool,Timeout::pointer> m_timeout_toggled_signal;
      
      sigc::signal<void> m_wakeup_main_signal;
      
      sigc::signal<void,DispatchStatus> m_dispatch_status_signal;
      
      FilterSignal m_filter_signal;
      
      Watches m_unhandled_watches;
      
      typedef std::map<DBusTimeout*,Timeout::pointer> Timeouts;

      Timeouts m_timeouts;

      friend void init(bool);

      static dbus_int32_t m_weak_pointer_slot;

      void initialize( bool is_private );

      std::map<std::string,ObjectPathHandler::pointer> m_created_objects;

      InterfaceToNameProxySignalMap m_proxy_signal_interface_map;

//       std::map<SignalReceiver::pointer, sigc::connection> m_sighandler_iface_conn;

//       std::map<SignalReceiver::pointer, sigc::connection> m_sighandler_member_conn;

//       void on_signal_handler_interface_changed(SignalReceiver::pointer handler);

//       void on_signal_handler_member_changed(SignalReceiver::pointer handler);

//       typedef std::map<std::string,signal_base> SignalVTable;
//
//       typedef std::map<std::string, SignalVTable> InterfaceVTable;

      static dbus_bool_t on_add_watch_callback( DBusWatch* cwatch, void* data );

      static void on_remove_watch_callback( DBusWatch* cwatch, void* data );

      static void on_watch_toggled_callback( DBusWatch* cwatch, void* data );

      static dbus_bool_t on_add_timeout_callback( DBusTimeout* ctimeout, void* data );

      static void on_remove_timeout_callback( DBusTimeout* ctimeout, void* data );

      static void on_timeout_toggled_callback( DBusTimeout* ctimeout, void* data );

      static void on_wakeup_main_callback( void* data );

      static void on_dispatch_status_callback( DBusConnection* connection, DBusDispatchStatus new_status, void* data );

      static DBusHandlerResult on_filter_callback( DBusConnection* connection, DBusMessage* message, void* data );

  };
  
}

inline
DBus::Connection::pointer operator<<(DBus::Connection::pointer ptr, DBus::Message::pointer msg)
{
  if (not ptr) return ptr;
  *ptr << msg;
  return ptr;
}

inline
DBus::Connection::pointer operator<<(DBus::Connection::pointer ptr, DBus::Message::const_pointer msg)
{
  if (not ptr) return ptr;
  *ptr << msg;
  return ptr;
}

inline
DBus::Connection::pointer operator<<(DBus::Connection::pointer ptr, DBus::ReturnMessage::pointer msg)
{
  if (not ptr) return ptr;
  *ptr << msg;
  return ptr;
}

inline
DBus::Connection::pointer operator<<(DBus::Connection::pointer ptr, DBus::ReturnMessage::const_pointer msg)
{
  if (not ptr) return ptr;
  *ptr << msg;
  return ptr;
}

inline
DBus::Connection::pointer operator<<(DBus::Connection::pointer ptr, DBus::SignalMessage::pointer msg)
{
  if (not ptr) return ptr;
  *ptr << msg;
  return ptr;
}

inline
DBus::Connection::pointer operator<<(DBus::Connection::pointer ptr, DBus::SignalMessage::const_pointer msg)
{
  if (not ptr) return ptr;
  *ptr << msg;
  return ptr;
}

#endif

// #include <dbus-cxx/method_impl.h>

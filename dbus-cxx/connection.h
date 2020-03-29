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
#include <stdint.h>
#include <dbus-cxx/dbus_signal.h>
#include <dbus-cxx/messagefilter.h>
#include <dbus-cxx/signal_proxy_base.h>
#include <dbus/dbus.h>
#include <deque>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <dbus/dbus.h>
#include "enums.h"
#include <sigc++/sigc++.h>
#include <future>

#ifndef DBUSCXX_CONNECTION_H
#define DBUSCXX_CONNECTION_H

namespace DBus
{
  class Message;
  class Object;
  class ObjectPathHandler;
  class ObjectProxy;
  class PendingCall;
  class ReturnMessage;
  class SignalMessage;
  class Timeout;
  class Watch;
  class Transport;
 struct InterruptablePredicateAccumulatorDefaultFalse;

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

      Connection( std::string address, bool is_private=false );

      Connection( const Connection& other );

    public:
      // TODO dbus_connection_open

      static std::shared_ptr<Connection> create( DBusConnection* cobj = NULL, bool is_private=false );

      /**
       * Connects to a bus daemon and registers the client with it.
       *
       * @param is_private if true a private connection will be created. Otherwise a shared connection is created.
       */
      static std::shared_ptr<Connection> create( BusType type, bool is_private=false );

      static std::shared_ptr<Connection> create( const Connection& other );

      static std::shared_ptr<Connection> create( std::string address, bool is_private=false );

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

      /**
       * Sends the message to the bus.
       * @return The number of bytes written
       */
      ssize_t send( const std::shared_ptr<const Message> );

      /**
       * Blindly sends the message on the connection.  Since you don't get any kind of handle
       * back from this, you should really only use it for sending method returns and signals.
       */
      Connection& operator<<( std::shared_ptr<const Message> msg );

      std::shared_ptr<PendingCall> send_with_reply_async( std::shared_ptr<const Message> message, int timeout_milliseconds=-1 ) const;

      std::shared_ptr<const ReturnMessage> send_with_reply_blocking( std::shared_ptr<const Message> msg, int timeout_milliseconds=-1 ) const;

      /**
       * Send a message, and get a future to the response.
       *
       * @param message
       * @param timeout_milliseconds
       * @return
       */
      std::future<const ReturnMessage> new_send_with_reply_async( std::shared_ptr<const Message> message, int timeout_milliseconds=-1 ) const;

      void flush();

      bool read_write_dispatch( int timeout_milliseconds=-1 );

      bool read_write( int timeout_milliseconds=-1 );

      std::shared_ptr<Message> borrow_message();

      void return_message( std::shared_ptr<Message> message );

      void steal_borrowed_message( std::shared_ptr<Message> message );

      std::shared_ptr<Message> pop_message( );

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

      typedef sigc::signal<bool(std::shared_ptr<Watch>)>::accumulated<InterruptablePredicateAccumulatorDefaultFalse> AddWatchSignal;

      /** Cannot call watch.handle() in a slot connected to this signal */
      AddWatchSignal& signal_add_watch();

      sigc::signal<bool(std::shared_ptr<Watch>)>& signal_remove_watch();

      sigc::signal<void(std::shared_ptr<Watch>)>& signal_watch_toggled();

      typedef sigc::signal<bool(std::shared_ptr<Timeout>)>::accumulated<InterruptablePredicateAccumulatorDefaultFalse> AddTimeoutSignal;
      
      /** Cannot call timeout.handle() in a slot connected to this signal */
      AddTimeoutSignal& signal_add_timeout();

      sigc::signal<bool(std::shared_ptr<Timeout>)>& signal_remove_timeout();

      sigc::signal<bool(std::shared_ptr<Timeout>)>& signal_timeout_toggled();

      sigc::signal<void()>& signal_wakeup_main();

      /** Cannot call dispatch() in a slot connected to this signal */
      sigc::signal<void(DispatchStatus)>& signal_dispatch_status_changed();

      /**
       * Signal emitted during dispatch. A slot returning true will cause the
       * message to be filtered.
       */
      FilterSignal& signal_filter();

      const std::deque<std::shared_ptr<Watch>>& unhandled_watches() const;
      
      void remove_unhandled_watch(const std::shared_ptr<Watch> w);

      std::shared_ptr<Object> create_object( const std::string& path, PrimaryFallback pf=PrimaryFallback::PRIMARY );

      bool register_object( std::shared_ptr<Object> object );

      std::shared_ptr<ObjectPathHandler> create_object( const std::string& path, 
                      sigc::slot<HandlerResult(std::shared_ptr<Connection>, std::shared_ptr<const Message>)>& slot, 
                      PrimaryFallback pf=PrimaryFallback::PRIMARY );

      std::shared_ptr<ObjectPathHandler> create_object( const std::string& path, 
                      HandlerResult (*MessageFunction)(std::shared_ptr<Connection>, std::shared_ptr<const Message>), 
                      PrimaryFallback pf=PrimaryFallback::PRIMARY );

      std::shared_ptr<ObjectProxy> create_object_proxy( const std::string& path );

      std::shared_ptr<ObjectProxy> create_object_proxy( const std::string& destination, const std::string& path );

      bool unregister_object( const std::string& path );

      template<typename... T_arg>
      std::shared_ptr<signal_proxy<T_arg...> > create_signal_proxy( const SignalMatchRule& rule )
      {
        std::shared_ptr<signal_proxy<T_arg...> > sig;
        sig = signal_proxy<T_arg...>::create(rule);
        this->add_signal_proxy( sig );
        return sig;
      }

      /**
       * Adds the given signal proxy to the connection
       */
      std::shared_ptr<signal_proxy_base> add_signal_proxy( std::shared_ptr<signal_proxy_base> signal );

      bool remove_signal_proxy( std::shared_ptr<signal_proxy_base> proxy );

      typedef std::vector<std::shared_ptr<signal_proxy_base>> ProxySignals;
      
      typedef std::map<std::string,ProxySignals> NameToProxySignalMap;

      /** Gets all the signal handlers */
      const ProxySignals& get_signal_proxies();

      /** Gets the signal handlers for a specific interface */
      ProxySignals get_signal_proxies( const std::string& interface );

      /** Gets the signal handlers for a specific interface and member */
      ProxySignals get_signal_proxies( const std::string& interface, const std::string& member );

      /** Create a signal, that when it is emitted will send that signal over the DBus  */
      template <class... T_arg>
      std::shared_ptr<signal<T_arg...> > create_signal( const std::string& path, const std::string& interface, const std::string& member )
      {
        std::shared_ptr<signal<T_arg...> > sig;
        sig = signal<T_arg...>::create(path, interface, member);
        sig->set_connection(this->self());
        return sig;
      }

      //       bool register_object( Object& obj, const std::string & path );
//
//       bool register_signal( signal_base& );

      /** Returns a smart pointer to itself */
      std::shared_ptr<Connection> self();

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
      static std::shared_ptr<Connection> self(DBusConnection* c);
      
      DBusConnection* cobj();

      static void set_global_change_sigpipe(bool will_modify_sigpipe=true);

      std::string introspect( const std::string& destination, const std::string& path );

    protected:
        int m_fd;
        std::vector<uint8_t> m_sendBuffer;
        uint32_t m_currentSerial;
        std::shared_ptr<Transport> m_transport;

      DBusConnection* m_cobj;
      
      bool m_private_flag;
      
      AddWatchSignal m_add_watch_signal;
      
      sigc::signal<bool(std::shared_ptr<Watch>)> m_remove_watch_signal;
      
      sigc::signal<void(std::shared_ptr<Watch>)> m_watch_toggled_signal;
      
      AddTimeoutSignal m_add_timeout_signal;
      
      sigc::signal<bool(std::shared_ptr<Timeout>)> m_remove_timeout_signal;
      
      sigc::signal<bool(std::shared_ptr<Timeout>)> m_timeout_toggled_signal;
      
      sigc::signal<void()> m_wakeup_main_signal;
      
      sigc::signal<void(DispatchStatus)> m_dispatch_status_signal;
      
      FilterSignal m_filter_signal;
      
      std::deque<std::shared_ptr<Watch>> m_unhandled_watches;
      
      typedef std::map<DBusTimeout*,std::shared_ptr<Timeout>> Timeouts;

      Timeouts m_timeouts;

      friend void init(bool);

      static dbus_int32_t m_weak_pointer_slot;

      void initialize( bool is_private );

      std::map<std::string,std::shared_ptr<ObjectPathHandler>> m_created_objects;

      ProxySignals m_proxy_signals;

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
  


inline
std::shared_ptr<DBus::Connection> operator<<(std::shared_ptr<DBus::Connection> ptr, std::shared_ptr<DBus::Message> msg)
{
  if (not ptr) return ptr;
  *ptr << msg;
  return ptr;
}

inline
std::shared_ptr<DBus::Connection> operator<<(std::shared_ptr<DBus::Connection> ptr, std::shared_ptr<const DBus::Message> msg)
{
  if (not ptr) return ptr;
  *ptr << msg;
  return ptr;
}

inline
std::shared_ptr<DBus::Connection> operator<<(std::shared_ptr<DBus::Connection> ptr, std::shared_ptr<DBus::ReturnMessage> msg)
{
  if (not ptr) return ptr;
  *ptr << msg;
  return ptr;
}

inline
std::shared_ptr<DBus::Connection> operator<<(std::shared_ptr<DBus::Connection> ptr, std::shared_ptr<const DBus::ReturnMessage> msg)
{
  if (not ptr) return ptr;
  *ptr << msg;
  return ptr;
}

inline
std::shared_ptr<DBus::Connection> operator<<(std::shared_ptr<DBus::Connection> ptr, std::shared_ptr<DBus::SignalMessage> msg)
{
  if (not ptr) return ptr;
  *ptr << msg;
  return ptr;
}

inline
std::shared_ptr<DBus::Connection> operator<<(std::shared_ptr<DBus::Connection> ptr, std::shared_ptr<const DBus::SignalMessage> msg)
{
  if (not ptr) return ptr;
  *ptr << msg;
  return ptr;
}

}

#endif

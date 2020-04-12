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
#include <dbus-cxx/threaddispatcher.h>
#include <dbus-cxx/errormessage.h>
#include <deque>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <dbus/dbus.h>
#include "enums.h"
#include <sigc++/sigc++.h>
#include <future>
#include <queue>

#ifndef DBUSCXX_CONNECTION_H
#define DBUSCXX_CONNECTION_H

/** Same as DBUS_NAME_FLAG_ALLOW_REPLACEMENT.  See request_name(). */
#define DBUSCXX_NAME_FLAG_ALLOW_REPLACEMENT 0x01
/** Same as DBUS_NAME_FLAG_REPLACE_EXISTING.  See request_name(). */
#define DBUSCXX_NAME_FLAG_REPLACE_EXISTING 0x02
/** Same as DBUS_NAME_FLAG_DO_NOT_QUEUE.  See request_name(). */
#define DBUSCXX_NAME_FLAG_DO_NOT_QUEUE 0x04

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
 struct InterruptablePredicateAccumulatorDefaultFalse;
 class ThreadDispatcher;
 class ErrorMessage;
 class DBusDaemonProxy;

 namespace priv{
    class Transport;
 }

  /**
   * Connection point to the DBus
   *
   * @ingroup core
   *
   * @todo TODO need to deal with signal proxies that change name and/or interface
   *
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class Connection : public std::enable_shared_from_this<Connection>
  {
    
    protected:
      Connection( BusType type );

      Connection( std::string address );

    public:
      /**
       * Connects to a bus daemon.  The returned Connection will have authenticated
       * with the bus at this point, but it will not be registered.  bus_register()
       * must be called - the Dispatcher should do this automatically.
       *
       * @param is_private if true a private connection will be created. Otherwise a shared connection is created.
       */
      static std::shared_ptr<Connection> create( BusType type );

      /**
       * Create a new connection, connecting to the specified address.  This address
       * needs to be in the in DBus transport format (e.g. unix:path=/tmp/dbus-test).
       * The returned connection will have authenticated with the bus at this point,
       * but it will not be registered.  bus_register() must be called - the Dispatcher
       * should do this automatically.
       *
       * @param address
       * @return
       */
      static std::shared_ptr<Connection> create( std::string address );

      virtual ~Connection();

      /** True if this is a valid connection; false otherwise */
      operator bool() const;

      /** True if this is a valid connection; false otherwise */
      bool is_valid() const;

      /** True if this connection is already registered */
      bool is_registered() const;

      /**
       * Registers this connection with the bus.  It is safe to call this
       * method multiple times.
       */
      bool bus_register();

      /** Gets the unique name of the connection as assigned by the message bus. */
      std::string unique_name() const;

      /**
       * Request the given name on the bus.
       *
       * @param name The name to request, e.g. "com.example.foo"
       * @param flags Any flags with the name; see DBUSCXX_NAME_FLAG_XXX macros
       * @return
       */
      RequestNameResponse request_name( const std::string& name, unsigned int flags = 0 );

      /**
       * Release the specified name, after requesting it via request_name
       *
       * @param name The name to release on the bus
       * @return
       */
      ReleaseNameResponse release_name( const std::string& name );

      /**
       * Check to see if the given name currently has an owner.
       *
       * Note that usage of this method in the form
       * if( !connection->name_has_owner( "foo.bar" ) ){
       *   connection->request_name( "foo.bar" );
       * }
       *
       * may be subject to race conditions, as multiple applications may be attempting
       * to request the bus name at the same time.  Instead, you should set the flags
       * in the request_name method to do this in a bus-atomic manner.
       *
       * @param name
       * @return
       */
      bool name_has_owner( const std::string& name ) const;

      /**
       * @brief start_service
       * @param name
       * @param flags
       * @return
       */
      StartReply start_service( const std::string& name, uint32_t flags=0 ) const;

      bool add_match( const std::string& rule );

      void add_match_nonblocking( const std::string& rule );

      bool remove_match( const std::string& rule );

      void remove_match_nonblocking( const std::string& rule );

      bool is_connected() const;

      bool is_authenticated() const;

      bool is_anonymous() const;

      const char* server_id() const;

      /**
       * Queues up the message to be sent on the bus.
       *
       * @param message The message to send
       * @return The serial of the message
       */
      uint32_t send( const std::shared_ptr<const Message> message );

      /**
       * Blindly sends the message on the connection.  Since you don't get any kind of handle
       * back from this, you should really only use it for sending method returns and signals.
       */
      Connection& operator<<( std::shared_ptr<const Message> msg );

      /**
       * Send a message and return a handle to the pending call.
       *
       * @param message
       * @param timeout_milliseconds
       * @return
       */
      std::shared_ptr<PendingCall> send_with_reply_async( std::shared_ptr<const Message> message, int timeout_milliseconds=-1 );

      /**
       * Send a CallMessage, and wait for the reply.
       *
       * If a timeout is processed, this will throw ErrorNoReply
       *
       * @param msg The message to send
       * @param timeout_milliseconds How long to wait for.  If -1, will wait the maximum time
       * @return The return message
       */
      std::shared_ptr<ReturnMessage> send_with_reply_blocking( std::shared_ptr<const CallMessage> msg, int timeout_milliseconds=-1 );

      /**
       * Send a message, and get a future to the response.
       *
       * @param message
       * @param timeout_milliseconds
       * @return
       */
      std::future<const ReturnMessage> new_send_with_reply_async( std::shared_ptr<const Message> message, int timeout_milliseconds=-1 ) const;

      /**
       * Flushes all data out to the bus.  This should generally
       * be called from the dispatching thread, but it should be
       * able to be called from any thread.
       */
      void flush();

      DispatchStatus dispatch_status( ) const;

      /**
       * Dispatch the connection.  Dispatching involves writing messages
       * to the bus, reading messages from the bus, and acting on those
       * messages, if available.  This method can only be called from
       * the dispatching thread, and will throw an exception if it is
       * called from the wrong thread.
       *
       * This method, when called, will process at most one message.  This
       * is required so that responses to method calls will appear to be fully
       * blocking.
       *
       * @return The status of dispatching.  This method should be called
       * util the status is DispatchStatus::COMPLETE
       */
      DispatchStatus dispatch( );

      int unix_fd() const;

      int socket() const;

      bool has_messages_to_send();

      /**
       * This signal is emitted whenever we need to be dispatched.
       *
       * Cannot call dispatch() in a slot connected to this signal.
       *
       * Any slots that listen to this signal must be threadsafe, as this
       * may be emitted from any thread.
       */
      sigc::signal<void()>& signal_needs_dispatch();

      /**
       * Signal emitted during dispatch. A slot returning true will cause the
       * message to be filtered.
       */
      FilterSignal& signal_filter();

      /**
       * Create and return a new object, registering the object automatically.  If the registering
       * fails, an invalid pointer will be returned.
       *
       * @param path The path of a new object, e.g. /opt/freedesktop/DBus
       * @param calling How to call this object(what thread)
       * @return An invalid shared_ptr if registration was not successful, else a valid shared_ptr
       */
      std::shared_ptr<Object> create_object( const std::string& path, ThreadForCalling calling );

      /**
       * Register an object with this connection.  The path that this object is accessible at
       * is part of the object.  You may also select which thread the object's methods should
       * be called on.
       *
       * @param object The object to export
       * @param calling Select how to call methods on this object.
       * @return The status of registering an object to be exported.
       */
      RegistrationStatus register_object( std::shared_ptr<ObjectPathHandler> object,
                                          ThreadForCalling calling );

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
        sig->set_connection( shared_from_this() );
        return sig;
      }

      //       bool register_object( Object& obj, const std::string & path );
//
//       bool register_signal( signal_base& );

      static void set_global_change_sigpipe(bool will_modify_sigpipe=true);

      std::string introspect( const std::string& destination, const std::string& path );

      /**
       * Set the ID of the thread that all of the dispatching hapens from.
       * If a blocking call is attempted on the non-dispatching thread,
       * that thread will block until the dispatching thread handles the data.
       *
       * By default, the dispatching thread is the thread that this Connection
       * was created in.  If using the default Dispatcher, this will be set automatically.
       *
       * @param tid
       */
      void set_dispatching_thread( std::thread::id tid );

      /**
       * Add a thread dispatcher that will handle messages for a given thread.
       * This method must be called from the thread that this ThreadDispatcher
       * is being called from.
       *
       * This is a weak_ptr, as the intention is to simply declare a local
       * ThreadDispatcher object inside of the thread.  When the thread exits,
       * the ThreadDispatcher will be deconstructed.  The next time a message
       * for this thread comes in, anything that is associated with this thread
       * will become unexported.
       *
       * @param disp The thread dispatcher to associate with this connection.
       */
      void add_thread_dispatcher( std::weak_ptr<ThreadDispatcher> disp );

  private:
      /**
       * Depending on what thread this is called from,
       * will either notify the dispatcher that we need to be
       * dispatched, or will do the dispatching needed.
       */
      void notify_dispatcher_or_dispatch();

      /**
       * Write a single message, return the serial of this message.
       * This should me called with a lock on m_outgoingLock
       *
       * @param msg
       * @return
       */
      uint32_t write_single_message( std::shared_ptr<const Message> msg );

      void process_single_message();

      void remove_invalid_threaddispatchers_and_associated_objects();

    protected:
      struct ExpectingResponse;
      struct OutgoingMessage{
          std::shared_ptr<const Message> msg;
          uint32_t serial;
      };
      struct PathHandlingEntry{
          std::shared_ptr<ObjectPathHandler> handler;
          std::thread::id handlingThread;
      };
        std::vector<uint8_t> m_sendBuffer;
        uint32_t m_currentSerial;
        std::shared_ptr<priv::Transport> m_transport;
        std::string m_uniqueName;
        std::thread::id m_dispatchingThread;
        std::queue<std::shared_ptr<Message>> m_incomingMessages;
        std::mutex m_outgoingLock;
        std::queue<OutgoingMessage> m_outgoingMessages;
        std::mutex m_expectingResponsesLock;
        std::map<uint32_t,std::shared_ptr<ExpectingResponse>> m_expectingResponses;
        DispatchStatus m_dispatchStatus;
        std::mutex m_pathHandlerLock;
        std::map<std::string,PathHandlingEntry> m_path_handler;
        std::map<std::string,std::shared_ptr<ObjectPathHandler>> m_path_handler_fallback;
        std::mutex m_threadDispatcherLock;
        std::map<std::thread::id,std::weak_ptr<ThreadDispatcher>> m_threadDispatchers;
        std::shared_ptr<DBusDaemonProxy> m_daemonProxy;
        sigc::signal<void()> m_needsDispatching;
      
      
      FilterSignal m_filter_signal;

      ProxySignals m_proxy_signals;

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

inline
std::shared_ptr<DBus::Connection> operator<<(std::shared_ptr<DBus::Connection> ptr, std::shared_ptr<DBus::ErrorMessage> msg)
{
  if (not ptr) return ptr;
  *ptr << msg;
  return ptr;
}

inline
std::shared_ptr<DBus::Connection> operator<<(std::shared_ptr<DBus::Connection> ptr, std::shared_ptr<const DBus::ErrorMessage> msg)
{
  if (not ptr) return ptr;
  *ptr << msg;
  return ptr;
}

}

#endif

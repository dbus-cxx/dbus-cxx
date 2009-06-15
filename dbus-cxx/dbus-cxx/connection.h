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
#ifndef __DBUSXX_CONNECTION_H
#define __DBUSXX_CONNECTION_H

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

      template <class T_return, class T_arg1=nil, class T_arg2=nil, class T_arg3=nil, class T_arg4=nil, class T_arg5=nil, class T_arg6=nil, class T_arg7=nil>
      DBusCxxPointer<signal_proxy<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7> > create_signal_proxy( const std::string& interface, const std::string& name ) {
//         signal<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7> sig;
        DBusCxxPointer<signal_proxy<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7> > sig;
        sig = signal_proxy<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7>::create(interface, name);
        this->add_signal_proxy(sig);
        return sig;
      }

      template <class T_return, class T_arg1=nil, class T_arg2=nil, class T_arg3=nil, class T_arg4=nil, class T_arg5=nil, class T_arg6=nil, class T_arg7=nil>
      DBusCxxPointer<signal_proxy<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7> > create_signal_proxy( const std::string& path, const std::string& interface, const std::string& name ) {
//         signal<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7> sig;
        DBusCxxPointer<signal_proxy<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7> > sig;
        sig = signal_proxy<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7>::create(path, interface, name);
        this->add_signal_proxy(sig);
        return sig;
      }

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

      template <class T_return, class T_arg1=nil, class T_arg2=nil, class T_arg3=nil, class T_arg4=nil, class T_arg5=nil, class T_arg6=nil, class T_arg7=nil>
      DBusCxxPointer<signal<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7> > create_signal( const std::string& interface, const std::string& name ) {
        DBusCxxPointer<signal<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7> > sig;
        sig = signal<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7>::create(interface, name);
        sig->m_connection = this->self();
        return sig;
      }

      template <class T_return, class T_arg1=nil, class T_arg2=nil, class T_arg3=nil, class T_arg4=nil, class T_arg5=nil, class T_arg6=nil, class T_arg7=nil>
      DBusCxxPointer<signal<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7> > create_signal( const std::string& path, const std::string& interface, const std::string& name ) {
        DBusCxxPointer<signal<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7> > sig;
        sig = signal<T_return,T_arg1,T_arg2,T_arg3,T_arg4,T_arg5,T_arg6,T_arg7>::create(path, interface, name);
        sig->set_connection(this->self());
        return sig;
      }

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

      void initialize( bool is_private );

      static dbus_int32_t m_weak_pointer_slot;

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
  
  inline
  Connection::pointer operator<<(Connection::pointer ptr, Message::pointer msg)
  {
    if (not ptr) return ptr;
    *ptr << msg;
    return ptr;
  }

  inline
  Connection::pointer operator<<(Connection::pointer ptr, Message::const_pointer msg)
  {
    if (not ptr) return ptr;
    *ptr << msg;
    return ptr;
  }

  inline
  Connection::pointer operator<<(Connection::pointer ptr, ReturnMessage::pointer msg)
  {
    if (not ptr) return ptr;
    *ptr << msg;
    return ptr;
  }

  inline
  Connection::pointer operator<<(Connection::pointer ptr, ReturnMessage::const_pointer msg)
  {
    if (not ptr) return ptr;
    *ptr << msg;
    return ptr;
  }

  inline
  Connection::pointer operator<<(Connection::pointer ptr, SignalMessage::pointer msg)
  {
    if (not ptr) return ptr;
    *ptr << msg;
    return ptr;
  }

  inline
  Connection::pointer operator<<(Connection::pointer ptr, SignalMessage::const_pointer msg)
  {
    if (not ptr) return ptr;
    *ptr << msg;
    return ptr;
  }

}

#endif

#include <dbus-cxx/method_impl.h>


  /**
   * \mainpage dbus-cxx Library
   *
   * <br><hr>
   *
   * \par About
   * dbus-cxx is yet another dbus C++ wrapper. What differentiates dbus-cxx from other C++
   * wrappers is that dbus-cxx relies heavily on sigc++ and explicitly exposes the wrapped
   * dbus C API.
   * 
   * \htmlonly
   * <script type="text/javascript" src="http://www.ohloh.net/p/330958/widgets/project_users.js?style=red"></script><br>
   * <script type="text/javascript" src="http://www.ohloh.net/p/330958/widgets/project_partner_badge.js"></script>
   * \endhtmlonly
   *
   * <br><hr>
   * 
   * \par dbus-cxx Sourceforge Project Page
   * The dbus-cxx sourceforge project page can be found here:
   *
   * \par
   * <a href="http://sourceforge.net/projects/dbus-cxx">http://sourceforge.net/projects/dbus-cxx</a>
   *
   * \par
   * <a href="http://sourceforge.net/news/?group_id=259994">News</a>
   *
   * <br><hr>
   *
   * \par Download dbus-cxx rpms or get the source code
   *
   * 
   * \htmlonly <img src="fedora-logo-tiny.png" alt="Fedora"/> \endhtmlonly
   * \par
   * dbus-cxx is available in Fedora 9 and higher.
   *
   * \par
   * Fedora package description
   * - \b dbus-cxx - Libraries needed to run applications
   * - \b dbus-cxx-devel - Headers, libraries and documentation for developing applications with dbus-cxx
   * - \b dbus-cxx-doc - Developer's documentation including devhelp docs
   * - \b dbus-cxx-tools - Development tools, et. al. such as \c dbus-cxx-xml2cpp
   *
   * \htmlonly <img src="sourcecode-small.png" alt=""/> \endhtmlonly
   * \par Source Code
   * \htmlonly <img src="download-small.png" alt=""/> \endhtmlonly
   * \b Releases - (.bz2, .gz, .zip) can be found <a href="http://sourceforge.net/project/showfiles.php?group_id=259994">here</a>
   *
   * \par
   * \b Subversion \b Repository
   * - You can browse the subversion repository at this url:
   *   - <a href="http://dbus-cxx.svn.sourceforge.net/viewvc/dbus-cxx/trunk/dbus-cxx">http://dbus-cxx.svn.sourceforge.net/viewvc/dbus-cxx/trunk/dbus-cxx</a>
   * - You can also check out a copy of the repository with this command:
   *   - \verbatim svn co https://dbus-cxx.svn.sourceforge.net/svnroot/dbus-cxx/trunk/dbus-cxx \endverbatim
   *
   * <br><hr>
   *
   * \par Dependencies... and where to get them
   * \b dbus: <a href="http://dbus.freedesktop.org">http://dbus.freedesktop.org</a>
   *
   * <br><hr>
   *
   * \htmlonly <img src="documents-small.png" alt=""/> \endhtmlonly
   * \par Documentation, Tutorials, Guides, Quick Start, et. al.
   *
   * \par dbus-cxx-xml2cpp Reference
   * Documentation on using \ref dbus-cxx-xml2cpp to convert DBus XML introspection
   * documents into C++ proxies and adapters.
   * 
   * \par API Reference
   * The API documentation (including the pages you are reading now) have been generated
   * with Doxygen.
   *
   * \par devhelp
   * The built documentation also includes a devhelp index. The index is installed
   * automatically in the dbus-cxx-doc RPM and can be browsed simply by starting
   * devhelp.
   *
   * \par
   * The most current documentation for dbus-cxx is available online here:
   * - dbus-cxx - <a href="http://dbus-cxx.sourceforge.net">http://dbus-cxx.sourceforge.net</a>
   *
   * \par Key sections within the API reference
   * - Namespaces
   *   - DBus
   * - Detailed class documentation in the <a href="hierarchy.html">Class Hierarchy</a>
   * - <a href="annotated.html">Class List</a>
   * - <a href="examples.html">Examples</a>
   *
   * <br><hr>
   *
   * \htmlonly <img src="envelope-small.png" alt=""/> \endhtmlonly
   * \par Mailing Lists
   * - Users List (for those using dbus-cxx in their applications)
   *   - <a href="http://sourceforge.net/mailarchive/forum.php?forum_name=dbus-cxx-users">Archives</a>
   *   - <a href="http://lists.sourceforge.net/mailman/listinfo/dbus-cxx-users">Subscribe/Unsubscribe</a>
   * - Development List (for discussion related to the development of dbus-cxx itself)
          *   - <a href="http://sourceforge.net/mailarchive/forum.php?forum_name=dbus-cxx-devel">Archives</a>
          *   - <a href="http://lists.sourceforge.net/mailman/listinfo/dbus-cxx-devel">Subscribe/Unsubscribe</a>
   *
   * <br><hr>
   *
   * \par Sourceforge Forums
   * Although mailing lists are preferred, the sourceforge forums are available for discussion as well.
   * - <a href="http://sourceforge.net/forum/forum.php?forum_id=943815">Help forum</a> (for those using dbus-cxx in their applications)
   * - <a href="http://sourceforge.net/forum/forum.php?forum_id=943816">Developers forum</a> (for discussion related to the development of dbus-cxx itself)
   * 
   * <br><hr>
   *
   * \par Bugs
   * \htmlonly <img src="bugs-small.png" alt=""/> \endhtmlonly
   * <a href="http://sourceforge.net/tracker/?atid=1127519&group_id=259994&func=browse">View reported bugs in the bug tracker</a>
   *
   * \par
   * \htmlonly <img src="bug-small.png" alt=""/> \endhtmlonly
   * <a href="http://sourceforge.net/tracker/?atid=1127519&group_id=259994&func=add">Submit a bug report here</a>
   *
   * <br><hr>
   *
   * \par Patches
   * Patches are always welcome!!!
   *
   * \par
   * <a href="http://sourceforge.net/tracker/?atid=1127520&group_id=259994&func=browse">View submitted patches</a>
   * 
   * \par
   * <a href="http://sourceforge.net/tracker/?atid=1127520&group_id=259994&func=add">Submit patches here</a>
   *
   * <br><hr>
   *
   * \par Feature Requests
   *
   * \par
   * <a href="http://sourceforge.net/tracker/?atid=1127521&group_id=259994&func=add">Submit feature requests here</a>
   *
   * <br><hr>
   *
   * \par License
   * dbus-cxx is released under the \htmlonly <a href="http://www.gnu.org/licenses/gpl-3.0-standalone.html"><img src="gplv3.png" alt="GPLv3"/></a> \endhtmlonly
   *
   */

  /**
   * \page xml2cpp dbus-cxx-xml2cpp
   *
   * \par Summary
   * \c dbus-cxx-xml2cpp generates proxy and adapter interfaces from modified DBus
   * XML introspection documents.
   *
   * \par Commands
   * - \c -x \c --xml : The xml file to process
   * - \c -p \c --prefix : A prefix to prepend to all output files
   * - \c -f \c --file : Send output to files with naming <i>class</i>_proxy.h and <i>class</i>_adapter.h
   * - \c --proxy : Generate a proxy interface
   * - \c --adapter : Generate an adapter interface
   * - \c --verbose : Generate verbose processing info
   *
   * \par Extended DBus XML attributes
   * \c dbus-cxx-xml2cpp in some cases needs information above and beyond that provided
   * in a dbus introspection document. This information can be supplied by adding
   * additional attributes to key nodes.
   *
   * \par
   * The extended attributes per XML element are:
   * - \b node
   *   - \b cppname - The base name of the c++ class adapter and proxy
   *   - \b gen-namespace - The namespace the c++ adapter and proxy will be generated into
   *   - \b orig-namespace - The namespace of the object being adapted
   *   - \b dest - Used to create a default value for the \c dest parameter of a proxy
   *   - \b path - Used to create a default value for the \c path parameter of both proxies and adapters
   *   - \b cppinclude - A free form string that will follow a \c #include line in the adapter
   *   - \b file-prefix - A per-node filename prefix prepended to the class name on output
   * - \b interface
   *   - \b ignored - If set to "1" this interface will not be built for the proxy or adapter
   * - \b signal
   *   - \b accessor - The accessor method used to obtain the adaptee's signal
   *   - \b ignored - If set to "1" this signal will not be built for the proxy or adapter
   * - \b method
   *   - \b cppname - Renames the DBus method to the provided name in the proxy and adapter
   * - \b arg
   *   - \b cpptype - Defines the C++ type to be used when creating proxies/adapters for this argument. Useful for wrapping enumerations.
   */

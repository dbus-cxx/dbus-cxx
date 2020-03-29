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
#include "connection.h"
#include <dbus-cxx/interfaceproxy.h>
#include <dbus-cxx/signalmessage.h>
#include <dbus-cxx/accumulators.h>
#include <algorithm>
#include <cassert>
#include <memory>
#include <utility>
#include "callmessage.h"
#include "dbus-cxx-private.h"
#include "error.h"
#include "message.h"
#include "messagefilter.h"
#include "messagehandler.h"
#include "object.h"
#include "objectpathhandler.h"
#include "objectproxy.h"
#include "path.h"
#include "pendingcall.h"
#include "returnmessage.h"
#include <sigc++/sigc++.h>
#include "signal_proxy_base.h"
#include "timeout.h"
#include "watch.h"
#include "transport.h"
#include "simpletransport.h"
#include <poll.h>

#include <fcntl.h>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/ioctl.h>

namespace sigc { template <typename T_return, typename ...T_arg> class signal; }
namespace sigc { template <typename T_return, typename ...T_arg> class slot; }

namespace DBus
{

/**
 * There's probably a less stupid way of doing this...
 */
std::string numberToHexString( int num ){
	std::ostringstream out;
	std::ostringstream numString;
	std::string finalNumString;

	numString << num;

	finalNumString = numString.str();
	out << std::hex;
	for( const char& s : finalNumString ){
		out << std::hex << (int)s;
	}

	return out.str();
}

  dbus_int32_t Connection::m_weak_pointer_slot = -1;
  
  Connection::Connection( DBusConnection* cobj, bool is_private ):
      m_cobj( cobj )
  {
    if ( m_cobj ) {
      dbus_connection_ref( m_cobj );
      this->initialize(is_private);
    }
  }

  Connection::Connection( BusType type, bool is_private ): m_cobj( nullptr )
  {
    //std::string dbusAddress = std::string( getenv( "DBUS_SESSION_BUS_ADDRESS" ) );
    std::string dbusAddress("/run/user/1000/bus");
    struct sockaddr_un addr = {0};
    m_currentSerial = 1;

    m_fd = ::socket( AF_UNIX, SOCK_STREAM, 0 );
    if( m_fd < 0 ){
        std::string errmsg = strerror( errno );
        SIMPLELOGGER_DEBUG("dbus.Connection", "Unable to create socket: " + errmsg );
    }

    addr.sun_family = AF_UNIX;
    memcpy( addr.sun_path, dbusAddress.c_str(), dbusAddress.size() );

    if( ::connect( m_fd, (struct sockaddr*)&addr, sizeof( addr ) ) < 0 ){
        std::string errmsg = strerror( errno );
        SIMPLELOGGER_DEBUG("dbus.Connection", "Unable to connect: " + errmsg );
        return;
    }

    SIMPLELOGGER_DEBUG("dbus.Connection", "Opened dbus connection" );

    int passcred = 1;
    if( ::setsockopt( m_fd, SOL_SOCKET, SO_PASSCRED, &passcred, sizeof( int ) ) < 0 ){
        std::string errmsg = strerror( errno );
        SIMPLELOGGER_DEBUG("dbus.Connection", "Unable to set passcred: " + errmsg );
        return;
    }

    m_transport = SimpleTransport::create( m_fd, true );
    if( !m_transport ){
        std::string errmsg = strerror( errno );
        SIMPLELOGGER_DEBUG("dbus.Connection", "Unable to create transport: " + errmsg );
        return;
    }

    int uid = getuid();
    std::ostringstream authCommand;
    authCommand << "AUTH EXTERNAL ";
    authCommand << numberToHexString( uid );
    authCommand << "\r\n";
    std::string fullCmd = authCommand.str();
    SIMPLELOGGER_DEBUG("dbus.Connection", "auth command: " + fullCmd );
    if( ::write( m_fd, fullCmd.c_str(), fullCmd.size() ) < 0 ){
        std::string errmsg = strerror( errno );
        SIMPLELOGGER_DEBUG("dbus.Connection", "Unable to write: " + errmsg );
        return;
    }

    char readData[ 1024 ];
    int bytesRead = ::read( m_fd, &readData, 1024 );
    if( bytesRead < 0 ){
        std::string errmsg = strerror( errno );
        SIMPLELOGGER_DEBUG("dbus.Connection", "Unable to read: " + errmsg );
        return;
    }

    readData[ bytesRead ] = 0;
    SIMPLELOGGER_DEBUG( "dbus.Connection", "Got back data: " + std::string( readData, bytesRead ) );

    std::string beginCmd = "BEGIN\r\n";
    if( ::write( m_fd, beginCmd.c_str(), beginCmd.size() ) < 0 ){
        std::string errmsg = strerror( errno );
        SIMPLELOGGER_DEBUG("dbus.Connection", "Unable to write: " + errmsg );
        return;
    }

    // Turn the FD into non-blocking
    {
        int flags = fcntl(m_fd, F_GETFL, 0);
        fcntl(m_fd, F_SETFL, flags | O_NONBLOCK);
    }

    std::shared_ptr<CallMessage> helloMsg = CallMessage::create( "org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "Hello" );
    // Can't use normal methods here before the constructor is done,
    // so we need to do some bootstrapping here!
    if( m_transport->writeMessage( helloMsg, m_currentSerial++ ) <= 0 ){
        std::string errmsg = strerror( errno );
        SIMPLELOGGER_ERROR("dbus.Connection", "Unable say hello: " + errmsg );
        return;
    }

    // Wait for the bus to respond
    pollfd pollfd;
    pollfd.fd = m_fd;
    pollfd.events = POLLIN;

    std::shared_ptr<Message> retmsg;
    do{
        if( poll( &pollfd, 1, -1 ) < 0 ){
            std::string errmsg = strerror( errno );
            SIMPLELOGGER_ERROR("dbus.Connection", "Unable to poll for response from daemon: " + errmsg );
            return;
        }
        retmsg = m_transport->readMessage();
    }while( !retmsg );

    std::ostringstream logmsg;
    logmsg << "Have retmsg!  serial: " << retmsg->serial();
    SIMPLELOGGER_DEBUG("dbus.Connection", logmsg.str() );

    if( retmsg->type() != MessageType::RETURN ){
        SIMPLELOGGER_ERROR("dbus.Connection", "Did not get a return message for some reason" );
        return;
    }

    std::shared_ptr<ReturnMessage> realRetmsg = std::static_pointer_cast<ReturnMessage>( retmsg );
    std::string value;
    realRetmsg >> value;

    logmsg.str("");
    logmsg << "Return message, our ID on the bus is " << value;
    SIMPLELOGGER_DEBUG("dbus.Connection", logmsg.str() );

    logmsg.str("");
    logmsg << "Real rtmsg : " << realRetmsg.get();
    SIMPLELOGGER_DEBUG("dbus.Connection", logmsg.str() );


do{
    if( poll( &pollfd, 1, -1 ) < 0 ){
        std::string errmsg = strerror( errno );
        SIMPLELOGGER_ERROR("dbus.Connection", "Unable to poll for response from daemon: " + errmsg );
        return;
    }

    int bytesAvail;
    if( ioctl( m_fd, FIONREAD, &bytesAvail ) < 0 ){
            std::string errmsg = strerror( errno );
            SIMPLELOGGER_DEBUG("dbus.Connection", "Unable to ioctl: " + errmsg );
            return;
    }

    std::ostringstream debugstr;
    debugstr << "bytes avail before attempting to read: " << bytesAvail;
    SIMPLELOGGER_DEBUG("dbus.Connection", debugstr.str() );

    retmsg = m_transport->readMessage();

    if( ioctl( m_fd, FIONREAD, &bytesAvail ) < 0 ){
            std::string errmsg = strerror( errno );
            SIMPLELOGGER_DEBUG("dbus.Connection", "Unable to ioctl: " + errmsg );
            return;
    }
    debugstr.str("");
    debugstr << "bytes avail after attempting to read: " << bytesAvail;
    SIMPLELOGGER_DEBUG("dbus.Connection", debugstr.str() );
    SIMPLELOGGER_DEBUG("dbus.Connection", "Did not get valid message back" );
}while( !retmsg );
    logmsg.str("");
    logmsg << "Next message: " << retmsg;
    SIMPLELOGGER_DEBUG("dbus.Connection", logmsg.str() );

sleep( 1 );

/*
    Error error = Error();

    if ( type != BusType::NONE ) {

      if ( is_private ) {
        m_cobj = dbus_bus_get_private(( DBusBusType )type, error.cobj() );
        if ( error.is_set() ) throw error;
        if ( m_cobj == nullptr ) throw ErrorFailed();
        this->initialize(is_private);
      }
      else {
        m_cobj = dbus_bus_get(( DBusBusType )type, error.cobj() );
	if ( error.is_set() ) throw error;
        if ( m_cobj == nullptr ) throw ErrorFailed();
        this->initialize(is_private);
      }
    }
*/
  }

  Connection::Connection( std::string address, bool is_private ): m_cobj( nullptr )
  {
    Error error = Error();

    if ( is_private ) {
      m_cobj = dbus_connection_open_private(address.c_str(), error.cobj() );
      if ( error.is_set() ) throw error;
      if ( m_cobj == nullptr ) throw ErrorFailed();
    }
    else {
      m_cobj = dbus_connection_open(address.c_str(), error.cobj() );
      if ( error.is_set() ) throw error;
      if ( m_cobj == nullptr ) throw ErrorFailed();
    }

    //Make sure the DBus doesn't kick us for not sending the org.freedesktop.DBus.Hello
    if(!is_registered()) {
      if(!bus_register()) {
        throw ErrorFailed(); 
      }
    }

    this->initialize(is_private);    
  }

  Connection::Connection( const Connection& other )
  {
    m_cobj = other.m_cobj;
    if ( m_cobj ) dbus_connection_ref( m_cobj );
  }

  static void conn_wp_deleter( void* v )
  {
    std::weak_ptr<Connection>* wp = static_cast<std::weak_ptr<Connection>*>(v);
    delete wp;
  }

  std::shared_ptr<Connection> Connection::create( DBusConnection* cobj, bool is_private )
  {
    std::shared_ptr<Connection> p( new Connection(cobj, is_private) );

    if ( m_weak_pointer_slot == -1 ) throw ErrorNotInitialized();
    if ( p and p->is_valid() )
    {
      dbus_bool_t result;
      std::weak_ptr<Connection>* wp = new std::weak_ptr<Connection>;
      *wp = p;
      result = dbus_connection_set_data( p->cobj(), m_weak_pointer_slot, wp, conn_wp_deleter );
      if ( not result ) throw -1; // TODO throw something better
    }
    
    return p;
  }

  std::shared_ptr<Connection> Connection::create( BusType type, bool is_private )
  {
    std::shared_ptr<Connection> p( new Connection(type, is_private) );

    if ( m_weak_pointer_slot == -1 ) throw ErrorNotInitialized();
    if ( p and p->is_valid() )
    {
      dbus_bool_t result;
      //TODO WTF is this doing?  Creating a weak pointer with new??
      std::weak_ptr<Connection>* wp = new std::weak_ptr<Connection>;
      *wp = p;
      result = dbus_connection_set_data( p->cobj(), m_weak_pointer_slot, wp, conn_wp_deleter );
      if ( not result ) throw -1; // TODO throw something better
    }
    
    return p;
  }

  std::shared_ptr<Connection> Connection::create( std::string address, bool is_private )
  {
    std::shared_ptr<Connection> p( new Connection(address, is_private) );

    if ( m_weak_pointer_slot == -1 ) throw ErrorNotInitialized();
    if ( p and p->is_valid() )
    {
      dbus_bool_t result;
      std::weak_ptr<Connection>* wp = new std::weak_ptr<Connection>;
      *wp = p;
      result = dbus_connection_set_data( p->cobj(), m_weak_pointer_slot, wp, conn_wp_deleter );
      if ( not result ) throw -1; // TODO throw something better
    }
    
    return p;

  }

  std::shared_ptr<Connection> Connection::create( const Connection& other )
  {
    std::shared_ptr<Connection> p( new Connection(other) );
    
    if ( m_weak_pointer_slot == -1 ) throw  ErrorNotInitialized();
    if ( p and p->is_valid() )
    {
      dbus_bool_t result;
      std::weak_ptr<Connection>* wp = new std::weak_ptr<Connection>;
      *wp = p;
      result = dbus_connection_set_data( p->cobj(), m_weak_pointer_slot, wp, conn_wp_deleter );
      if ( not result ) throw -1; // TODO throw something better
    }
    
    return p;
  }

  Connection::~Connection()
  {
    if ( this->is_valid() and this->is_private() )
      dbus_connection_close( m_cobj );
    if ( m_cobj ) dbus_connection_unref( m_cobj );
  }

  std::shared_ptr<Connection> Connection::self()
  {
    if ( not this->is_valid() or m_weak_pointer_slot == -1 ) return std::shared_ptr<Connection>();
    
    void* v = dbus_connection_get_data(this->cobj(), m_weak_pointer_slot);

    if ( v == nullptr ) return std::shared_ptr<Connection>();

    std::weak_ptr<Connection>* wp = static_cast<std::weak_ptr<Connection>*>(v);

    std::shared_ptr<Connection> p = wp->lock();

    return p;
  }

  std::shared_ptr<Connection> Connection::self(DBusConnection * c)
  {
    if ( c == nullptr or m_weak_pointer_slot == -1 ) return std::shared_ptr<Connection>();
    
    void* v = dbus_connection_get_data(c, m_weak_pointer_slot);

    if ( v == nullptr ) return std::shared_ptr<Connection>();

    std::weak_ptr<Connection>* wp = static_cast<std::weak_ptr<Connection>*>(v);

    std::shared_ptr<Connection> p = wp->lock();

    return p;
  }



  DBusConnection* Connection::cobj()
  {
    return m_cobj;
  }

  Connection::operator bool() const
  {
    return m_cobj;
  }

  bool Connection::is_valid() const
  {
    return m_cobj;
  }

  bool Connection::is_private() const
  {
    return m_private_flag;
  }

  bool Connection::bus_register()
  {
    dbus_bool_t result;
    Error error = Error();
    if ( not this->is_valid() ) return false;
    result = dbus_bus_register( m_cobj, error.cobj() );
    if ( error.is_set() ) throw error;
    return result;
  }

  bool Connection::is_registered() const
  {
    return this->unique_name() != nullptr;
  }

  const char * Connection::unique_name() const
  {
    if ( not this->is_valid() ) return nullptr;
    return dbus_bus_get_unique_name(m_cobj);
  }

  unsigned long Connection::unix_user( const std::string & name ) const
  {
    Error error = Error();
    if ( not this->is_valid() ) return -1;
    return dbus_bus_get_unix_user( m_cobj, name.c_str(), error.cobj() );
  }

  const char* Connection::bus_id() const
  {
    Error error = Error();
    if ( not this->is_valid() ) return nullptr;
    return dbus_bus_get_id( m_cobj, error.cobj() );
  }

  int Connection::request_name( const std::string& name, unsigned int flags )
  {
    int result;
    Error error = Error();

    if ( not this->is_valid() ) return -1;
    result = dbus_bus_request_name( m_cobj, name.c_str(), flags, error.cobj() );
    if ( error.is_set() ) throw error;
    return result;
  }

  int Connection::release_name( const std::string& name )
  {
    int result;
    Error error = Error();

    if ( not this->is_valid() ) return -1;
    result = dbus_bus_release_name( m_cobj, name.c_str(), error.cobj() );
    if ( error.is_set() ) throw error;
    return result;
  }

  bool Connection::name_has_owner( const std::string& name ) const
  {
    dbus_bool_t result;
    Error error = Error();

    if ( not this->is_valid() ) return false;
    result = dbus_bus_name_has_owner( m_cobj, name.c_str(), error.cobj() );
    if ( error.is_set() ) throw error;
    return result;
  }

  StartReply Connection::start_service( const std::string& name, uint32_t flags ) const
  {
    dbus_bool_t succeeded;
    dbus_uint32_t result_code;
    Error error = Error();

    if ( not this->is_valid() ) return StartReply::FAILED;

    succeeded = dbus_bus_start_service_by_name( m_cobj, name.c_str(), flags, &result_code, error.cobj() );

    if ( error.is_set() ) throw error;

    if ( succeeded )
      switch ( result_code ) {
        case DBUS_START_REPLY_SUCCESS: return StartReply::SUCCESS;
        case DBUS_START_REPLY_ALREADY_RUNNING: return StartReply::ALREADY_RUNNING;
      }

    return StartReply::FAILED;
  }

  bool Connection::add_match( const std::string& rule )
  {
    Error error = Error();

    if ( not this->is_valid() ) return false;
    SIMPLELOGGER_DEBUG("dbus.Connection", "Adding the following match: " << rule );
    dbus_bus_add_match( m_cobj, rule.c_str(), error.cobj() );

    if ( error.is_set() ) return false;
    return true;
  }

  void Connection::add_match_nonblocking( const std::string& rule )
  {
    if ( not this->is_valid() ) return;
    dbus_bus_add_match( m_cobj, rule.c_str(), nullptr );
  }

  bool Connection::remove_match( const std::string& rule )
  {
    Error error = Error();

    if ( not this->is_valid() ) return false;
    dbus_bus_remove_match( m_cobj, rule.c_str(), error.cobj() );

    if ( error.is_set() ) return false;
    return true;
  }

  void Connection::remove_match_nonblocking( const std::string& rule )
  {
    if ( not this->is_valid() ) return;
    dbus_bus_remove_match( m_cobj, rule.c_str(), nullptr );
  }

  bool Connection::is_connected() const
  {
    if ( not this->is_valid() ) return false;
    return dbus_connection_get_is_connected( m_cobj );
  }

  bool Connection::is_authenticated() const
  {
    if ( not this->is_valid() ) return false;
    return dbus_connection_get_is_authenticated( m_cobj );
  }

  bool Connection::is_anonymous() const
  {
    if ( not this->is_valid() ) return false;
    return dbus_connection_get_is_anonymous( m_cobj );
  }

  const char* Connection::server_id() const
  {
    if ( not this->is_valid() ) return nullptr;
    return dbus_connection_get_server_id( m_cobj );
  }

  ssize_t Connection::send( std::shared_ptr<const Message> msg )
  {
    if ( not this->is_valid() ) throw ErrorDisconnected();
    if ( !msg || !m_transport) return 0;
    if( m_currentSerial == 0 ) m_currentSerial = 1;

    return m_transport->writeMessage( msg, m_currentSerial++ );


//    errno = EINVAL;
//    return -1;

//    if ( not this->is_valid() ) throw ErrorDisconnected();
//    if ( not msg or not *msg ) return 0;
//    if ( not dbus_connection_send( m_cobj, msg->cobj(), &serial ) ) throw ErrorNoMemory();
//    return serial;
  }

  Connection & Connection::operator <<(std::shared_ptr<const Message> msg)
  {
    if ( msg ) this->send(msg);
    return *this;
  }

  std::shared_ptr<PendingCall> Connection::send_with_reply_async( std::shared_ptr<const Message> message, int timeout_milliseconds ) const
  {
    DBusPendingCall* reply;
    if ( not this->is_valid() ) throw ErrorDisconnected();
    if ( not message or not *message ) return std::shared_ptr<PendingCall>();
    if ( not dbus_connection_send_with_reply( m_cobj, message->cobj(), &reply, timeout_milliseconds ) )
      throw ErrorNoMemory( "Unable to start asynchronous call" );
    return PendingCall::create( reply );
  }

  std::shared_ptr<const ReturnMessage> Connection::send_with_reply_blocking( std::shared_ptr<const Message> message, int timeout_milliseconds ) const
  {
    DBusMessage* reply;
    Error error = Error();

    if ( not this->is_valid() ) throw ErrorDisconnected();

    if ( not message or not *message ) return std::shared_ptr<const ReturnMessage>();

    dbus_message_set_no_reply(message->cobj(),FALSE);

    reply = dbus_connection_send_with_reply_and_block( m_cobj, message->cobj(), timeout_milliseconds, error.cobj() );

    if ( error.is_set() ){ 
/*
      SIMPLELOGGER_ERROR( "dbus.Connection", "Message: [" 
        << error->message() 
        << "] Name: ["
        << error->name()
        << "]" ); 
*/
      throw error; 
    }

    SIMPLELOGGER_DEBUG("dbus.Connection", "Reply signature: " << dbus_message_get_signature(reply) );
    
    std::shared_ptr<ReturnMessage> retmsg = ReturnMessage::create(reply);

    dbus_message_unref(reply);

    //SIMPLELOGGER_DEBUG("dbus.Connection", "Return Signature: " << retmsg->signature() );

    return retmsg;
  }

  void Connection::flush()
  {
    if ( not this->is_valid() ) return;
    dbus_connection_flush( m_cobj );
  }

  bool Connection::read_write_dispatch( int timeout_milliseconds )
  {
    if ( not this->is_valid() ) return false;
    return dbus_connection_read_write_dispatch( m_cobj, timeout_milliseconds );
  }

  bool Connection::read_write( int timeout_milliseconds )
  {
    if ( not this->is_valid() ) return false;
    return dbus_connection_read_write( m_cobj, timeout_milliseconds );
  }

  std::shared_ptr<Message> Connection::borrow_message()
  {
    if ( not this->is_valid() ) return std::shared_ptr<Message>();
    return Message::create( dbus_connection_borrow_message( m_cobj ) );
  }

  void Connection::return_message( std::shared_ptr<Message> message )
  {
    if ( not this->is_valid() or not message or not *message ) return;
    dbus_connection_return_message( m_cobj, message->cobj() );
  }

  void Connection::steal_borrowed_message( std::shared_ptr<Message> message )
  {
    if ( not this->is_valid() or not message or not *message ) return;
    dbus_connection_steal_borrowed_message( m_cobj, message->cobj() );
  }

  std::shared_ptr<Message> Connection::pop_message( )
  {
    DBusMessage* message;
    if ( not this->is_valid() ) return std::shared_ptr<Message>();
    message = dbus_connection_pop_message( m_cobj );
    return Message::create( message );
  }

  DispatchStatus Connection::dispatch_status( ) const
  {
    if ( not this->is_valid() ) return DispatchStatus::COMPLETE;
    return static_cast<DispatchStatus>( dbus_connection_get_dispatch_status( m_cobj ) );
  }

  DispatchStatus Connection::dispatch( )
  {
    if ( not this->is_valid() ) return DispatchStatus::COMPLETE;
    dbus_connection_dispatch( m_cobj );
    return static_cast<DispatchStatus>( dbus_connection_get_dispatch_status( m_cobj ) );
  }

  int Connection::unix_fd() const
  {
    dbus_bool_t result;
    int fd;
    if ( not this->is_valid() ) return -1;
    result = dbus_connection_get_unix_fd( m_cobj, &fd );
    if ( not result ) return -1;
    return fd;
  }

  int Connection::socket() const
  {
    dbus_bool_t result;
    int s;
    if ( not this->is_valid() ) return -1;
    result = dbus_connection_get_socket( m_cobj, &s );
    if ( not result ) return -1;
    return s;
  }

  unsigned long Connection::unix_user() const
  {
    dbus_bool_t result;
    unsigned long uid;
    if ( not this->is_valid() ) return -1;
    result = dbus_connection_get_unix_user( m_cobj, &uid );
    if ( not result ) return -1;
    return uid;
  }

  unsigned long Connection::unix_process_id() const
  {
    dbus_bool_t result;
    unsigned long pid;
    if ( not this->is_valid() ) return -1;
    result = dbus_connection_get_unix_process_id( m_cobj, &pid );
    if ( not result ) return -1;
    return pid;
  }

  void Connection::set_allow_anonymous(bool allow)
  {
    if ( not this->is_valid() ) return;
    dbus_connection_set_allow_anonymous( m_cobj, allow );
  }

  void Connection::set_route_peer_messages(bool route)
  {
    if ( not this->is_valid() ) return;
    dbus_connection_set_route_peer_messages( m_cobj, route );
  }

  void Connection::set_max_message_size(long size)
  {
    if ( not this->is_valid() ) return;
    dbus_connection_set_max_message_size( m_cobj, size );
  }

  long Connection::max_message_size()
  {
    if ( not this->is_valid() ) return -1;
    return dbus_connection_get_max_message_size(m_cobj);
  }

  void Connection::set_max_received_size(long size)
  {
    if ( not this->is_valid() ) return;
    dbus_connection_set_max_received_size( m_cobj, size );
  }

  long Connection::max_received_size()
  {
    if ( not this->is_valid() ) return -1;
    return dbus_connection_get_max_received_size(m_cobj);
  }

  long Connection::outgoing_size()
  {
    if ( not this->is_valid() ) return -1;
    return dbus_connection_get_outgoing_size(m_cobj);
  }

  bool Connection::has_messages_to_send()
  {
    if ( not this->is_valid() ) return false;
    return dbus_connection_has_messages_to_send(m_cobj);
  }

  Connection::AddWatchSignal& Connection::signal_add_watch()
  {
    return m_add_watch_signal;
  }

  sigc::signal<bool(std::shared_ptr<Watch>)>& Connection::signal_remove_watch()
  {
    return m_remove_watch_signal;
  }

  sigc::signal<void(std::shared_ptr<Watch>)>& Connection::signal_watch_toggled()
  {
    return m_watch_toggled_signal;
  }

  Connection::AddTimeoutSignal& Connection::signal_add_timeout()
  {
    return m_add_timeout_signal;
  }

  sigc::signal<bool(std::shared_ptr<Timeout>)>& Connection::signal_remove_timeout()
  {
    return m_remove_timeout_signal;
  }

  sigc::signal<bool(std::shared_ptr<Timeout>)>& Connection::signal_timeout_toggled()
  {
    return m_timeout_toggled_signal;
  }

  sigc::signal< void() > & Connection::signal_wakeup_main()
  {
    return m_wakeup_main_signal;
  }

  sigc::signal< void(DispatchStatus) > & Connection::signal_dispatch_status_changed()
  {
    return m_dispatch_status_signal;
  }

  FilterSignal& Connection::signal_filter()
  {
    return m_filter_signal;
  }

  void Connection::set_global_change_sigpipe(bool will_modify_sigpipe)
  {
    dbus_connection_set_change_sigpipe(will_modify_sigpipe);
  }

  const std::deque<std::shared_ptr<Watch>>& Connection::unhandled_watches() const
  {
    return m_unhandled_watches;
  }
  
  void Connection::remove_unhandled_watch(const std::shared_ptr<Watch> w)
  {
    std::deque<std::shared_ptr<Watch>>::iterator i;
    
    if ( not w ) return;
    
    for (i = m_unhandled_watches.begin(); i != m_unhandled_watches.end(); i++)
    {
      if ( (*i)->cobj() == w->cobj() )
      {
        m_unhandled_watches.erase(i);
        return;
      }
    }
  }

  std::shared_ptr<Object> Connection::create_object(const std::string & path, PrimaryFallback pf)
  {
    std::shared_ptr<Object> object = Object::create( path, pf );
    if (not object) return object;
    object->register_with_connection( this->self() );
    m_created_objects[path] = object;
    return object;
  }

  bool Connection::register_object(std::shared_ptr<Object> object)
  {
    SIMPLELOGGER_DEBUG("dbus.Connection", "Connection::register_object");
    if ( not object ) return false;
    object->register_with_connection( this->self() );
    return true;
  }

  std::shared_ptr<ObjectPathHandler> Connection::create_object(const std::string & path, sigc::slot< HandlerResult(std::shared_ptr<Connection>, std::shared_ptr<const Message>) >& slot, PrimaryFallback pf)
  {
    std::shared_ptr<ObjectPathHandler> handler = ObjectPathHandler::create(path, pf);
    if ( not handler ) return handler;
    handler->register_with_connection( this->self() );
    m_created_objects[path] = handler;
    if ( handler ) handler->signal_message().connect( slot );
    return handler;
  }

  std::shared_ptr<ObjectPathHandler> Connection::create_object( const std::string& path, HandlerResult (*MessageFunction)(std::shared_ptr<Connection>,std::shared_ptr<const Message>), PrimaryFallback pf )
  {
    std::shared_ptr<ObjectPathHandler> handler = ObjectPathHandler::create(path, pf);
    if ( not handler ) return handler;
    handler->register_with_connection( this->self() );
    m_created_objects[path] = handler;
    if ( handler ) handler->signal_message().connect( sigc::ptr_fun(MessageFunction) );
    return handler;
  }

  std::shared_ptr<ObjectProxy> Connection::create_object_proxy(const std::string & path)
  {
    std::shared_ptr<ObjectProxy> object = ObjectProxy::create(this->self(), path);
    return object;
  }

  std::shared_ptr<ObjectProxy> Connection::create_object_proxy(const std::string & destination, const std::string & path)
  {
    std::shared_ptr<ObjectProxy> object = ObjectProxy::create(this->self(), destination, path);
    return object;
  }

  bool Connection::unregister_object(const std::string & path)
  {
    // TODO implement this
    return false;
  }

  std::shared_ptr<signal_proxy_base> Connection::add_signal_proxy(std::shared_ptr<signal_proxy_base> signal)
  {
    if ( not signal ) return std::shared_ptr<signal_proxy_base>();
    
    SIMPLELOGGER_DEBUG( "dbus.Connection", "Adding signal " << signal->interface() << ":" << signal->name() );

    if ( signal->connection() ) signal->connection()->remove_signal_proxy(signal);

    m_proxy_signals.push_back( signal );

    this->add_match( signal->match_rule() );
    signal->set_connection(this->self());

    return signal;
  }

  bool Connection::remove_signal_proxy( std::shared_ptr<signal_proxy_base> signal )
  {
    if ( not signal ) return false;

    SIMPLELOGGER_DEBUG( "dbus.Connection", "remove_signal_proxy" );

    this->remove_match( signal->match_rule() );

    ProxySignals::iterator it = std::find( m_proxy_signals.begin(), m_proxy_signals.end(), signal );
    if( it != m_proxy_signals.end() ){
        m_proxy_signals.erase( it );
        return true;
    }

    return false;
  }

//   bool Connection::register_signal_handler(SignalReceiver::pointer sighandler)
//   {
//     if ( not sighandler or sighandler->interface().empty() or sighandler->member().empty() ) return false;
// 
//     m_proxy_signal_interface_map[sighandler->interface()][sighandler->member()].push_back(sighandler);
// 
//     this->add_match( sighandler->match_rule() );
//     
//     return true;
//   }

//   bool Connection::unregister_signal_handler(SignalReceiver::pointer sighandler)
//   {
//     InterfaceToNameProxySignalMap::iterator i;
//     NameToProxySignalMap::iterator j;
//     std::list<SignalReceiver::pointer>::iterator k;
//     
//     if ( not sighandler or sighandler->interface().empty() or sighandler->member().empty() ) return false;
// 
//     i = m_proxy_signal_interface_map.find(sighandler->interface());
//     if ( i == m_proxy_signal_interface_map.end() ) return false;
// 
//     j = i->second.find(sighandler->member());
//     if ( j == i->second.end() ) return false;
// 
//     for ( k = j->second.begin(); k != j->second.end(); k++ )
//     {
//       if ( *k == sighandler )
//       {
//         j->second.erase(k);
//         return true;
//       }
//     }
//     
//     return false;
//   }

  const Connection::ProxySignals& Connection::get_signal_proxies()
  {
    return m_proxy_signals;
  }

  Connection::ProxySignals Connection::get_signal_proxies(const std::string & interface)
  {
    ProxySignals ret;

    for( std::shared_ptr<signal_proxy_base> base : m_proxy_signals ){
        if( base->interface().compare( interface ) == 0 ){
            ret.push_back( base );
        }
    }

    return ret;
  }

  Connection::ProxySignals Connection::get_signal_proxies(const std::string & interface, const std::string & member)
  {
    ProxySignals ret;

    for( std::shared_ptr<signal_proxy_base> base : m_proxy_signals ){
        if( base->interface().compare( interface ) == 0 &&
            base->name().compare( member ) == 0 ){
            ret.push_back( base );
        }
    }

    return ret;
  }


//   bool Connection::register_object( Object& obj, const std::string & path )
//   {
//     return dbus_connection_register_object_path( m_cobj, path.c_str(), obj.dbus_vtable(), &obj );
//   }
//
//   bool Connection::register_signal( signal_proxy_base & signal )
//   {
//     InterfaceVTable* interface_vtable;
//
//     // We must have a valid interface and signal name to perform the match upon
//     if ( signal.interface().size() == 0 || signal.name().size() == 0 )
//       return false;
//
//     // Let's get the signal vtable, and if we don't have a signal vtable let's make one
//     interface_vtable = static_cast<InterfaceVTable*>( dbus_connection_get_data( m_cobj, m_interface_vtable_slot ) );
//
// //                            DEBUG_OUT( "Connection::register_signal()", "pre-creation interface VTable address is " << (unsigned long) interface_vtable );
//
//     if ( interface_vtable == nullptr ) {
//       interface_vtable = new InterfaceVTable();
//       dbus_connection_set_data( m_cobj, m_interface_vtable_slot, interface_vtable, operator delete );
//     }
//
//     InterfaceVTable* interface_vtable2 = static_cast<InterfaceVTable*>( dbus_connection_get_data( m_cobj, m_interface_vtable_slot ) );
//
// //                            DEBUG_OUT( "Connection::register_signal()", "Interface VTable address is " << (unsigned long) interface_vtable2 );
//
//     this->add_match( "type='signal',interface='" + signal.interface() + "',member='" + signal.name() + "'" );
//
// //                            DEBUG_OUT( "Connection::register_signal()", "registering signal with match string \"" "type='signal',interface='" + signal.interface() + "',member='" + signal.name() + "'\"" );
//
//     ( *interface_vtable )[signal.interface()][signal.name()] = signal;
// //                            DEBUG_OUT( "Connection::register_signal()", "the interface vtable for " << (long unsigned)m_cobj << " has " << interface_vtable->size() << " elements" );
//   }
//

  template<typename T>
  void deleter(void * p)
  {
    T* p2 = static_cast<T*>(p);
    delete p2;
  }

  void Connection::initialize(bool is_private)
  {
    dbus_bool_t result;
    
    m_private_flag = is_private;

    // Install callbacks
    result = dbus_connection_set_watch_functions( m_cobj,
                                                  Connection::on_add_watch_callback,
                                                  Connection::on_remove_watch_callback,
                                                  Connection::on_watch_toggled_callback,
                                                  this,
                                                  nullptr
                                                );
    if ( not result ) throw ErrorNoMemory();
  
    result = dbus_connection_set_timeout_functions( m_cobj,
                                                  Connection::on_add_timeout_callback,
                                                  Connection::on_remove_timeout_callback,
                                                  Connection::on_timeout_toggled_callback,
                                                  this,
                                                  nullptr
                                                );
    if ( not result ) throw ErrorNoMemory();
  
    dbus_connection_set_wakeup_main_function( m_cobj, Connection::on_wakeup_main_callback, this, nullptr );
  
    dbus_connection_set_dispatch_status_function( m_cobj, Connection::on_dispatch_status_callback, this, nullptr );

    result = dbus_connection_add_filter( m_cobj, Connection::on_filter_callback, this, nullptr );
    if ( not result ) throw ErrorNoMemory();
  
  }

  dbus_bool_t Connection::on_add_watch_callback(DBusWatch * cwatch, void * data)
  {
    bool result;
    Connection* conn = static_cast<Connection*>(data);
    std::shared_ptr<Watch> watch = Watch::create(cwatch);
    result = conn->signal_add_watch().emit(watch);
    if ( not result ) conn->m_unhandled_watches.push_back(watch);
    return true;
  }

  void Connection::on_remove_watch_callback(DBusWatch * cwatch, void * data)
  {
    Connection* conn = static_cast<Connection*>(data);
    conn->signal_remove_watch().emit(Watch::create(cwatch));
  }

  void Connection::on_watch_toggled_callback(DBusWatch * cwatch, void * data)
  {
    Connection* conn = static_cast<Connection*>(data);
    conn->signal_watch_toggled().emit(Watch::create(cwatch));
  }

  dbus_bool_t Connection::on_add_timeout_callback(DBusTimeout * ctimeout, void * data)
  {
    assert(ctimeout);
    bool result;
    Connection* conn = static_cast<Connection*>(data);
    std::shared_ptr<Timeout> timeout = Timeout::create(ctimeout);
    SIMPLELOGGER_DEBUG( "dbus.Connection", "Connection::on_add_timeout_callback  enabled:" << timeout->is_enabled() << "  interval: " << timeout->interval() );

    // We'll give a signal callback a chance to handle the timeout
    result = conn->signal_add_timeout().emit(timeout);

    // If not, the connection will have to handle the timeout itself
    if ( not result )
    {
      Timeouts::iterator i;
      // Is this timeout already added?
      i = conn->m_timeouts.find(ctimeout);
      if ( i != conn->m_timeouts.end() )
      {
        // We already have the timeout, so let's just re-arm it
        i->second->arm();
      }
      else
      {
        conn->m_timeouts[ctimeout] = timeout;
        timeout->arm();
      }
    }
    return true;
  }

  void Connection::on_remove_timeout_callback(DBusTimeout * ctimeout, void * data)
  {
    Connection* conn = static_cast<Connection*>(data);
    std::shared_ptr<Timeout> timeout = Timeout::create(ctimeout);
    SIMPLELOGGER_DEBUG( "dbus.Connection", "Remove timeout callback. enabled:" << timeout->is_enabled() << "  interval: " << timeout->interval() );

    // Erase the timeout if this connection handled it
    // Otherwise, this has no effect
    conn->m_timeouts.erase(ctimeout);

    conn->signal_remove_timeout().emit(timeout);
  }

  void Connection::on_timeout_toggled_callback(DBusTimeout * ctimeout, void * data)
  {
    Connection* conn = static_cast<Connection*>(data);
    std::shared_ptr<Timeout> timeout = Timeout::create(ctimeout);
    SIMPLELOGGER_DEBUG( "dbus.Connection", "Timeout toggled.  enabled:" << timeout->is_enabled() << "  interval: " << timeout->interval() );

    // If we handled the timeout we'll handle the enabling/disabling
    Timeouts::iterator i;
    i = conn->m_timeouts.find(ctimeout);
    if ( i != conn->m_timeouts.end() )
    {
      if ( i->second->is_enabled() ) i->second->arm();
      else i->second->arm(false);
    }
    // But, if we didn't handle it will pass it along to the signal
    else
    {
      conn->signal_timeout_toggled().emit(timeout);
    }
  }

  void Connection::on_wakeup_main_callback(void * data)
  {
    Connection* conn = static_cast<Connection*>(data);
    conn->signal_wakeup_main().emit();
  }

  void Connection::on_dispatch_status_callback(DBusConnection * connection, DBusDispatchStatus new_status, void * data)
  {
    Connection* conn = static_cast<Connection*>(data);
    conn->signal_dispatch_status_changed().emit(static_cast<DispatchStatus>(new_status));
  }

  DBusHandlerResult Connection::on_filter_callback(DBusConnection * connection, DBusMessage * message, void * data)
  {
    if ( message == nullptr ) return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    
    std::shared_ptr<Connection> conn = static_cast<Connection*>(data)->self();
    FilterResult filter_result = FilterResult::DONT_FILTER;
    HandlerResult signal_result = HandlerResult::NOT_HANDLED;
    std::shared_ptr<Message> msg = Message::create(message);

    filter_result = conn->signal_filter().emit(conn, msg);

    SIMPLELOGGER_DEBUG( "dbus.Connection", "Filter callback.  filter_result: " << static_cast<int>( filter_result ) );

    // Deliver signals to signal proxies
    if ( filter_result != FilterResult::FILTER and msg->type() == MessageType::SIGNAL )
    {
      std::shared_ptr<SignalMessage> smsg = SignalMessage::create(msg);
      HandlerResult result = HandlerResult::NOT_HANDLED;

      SIMPLELOGGER_DEBUG( "dbus.Connection", "Handling signal " 
          << smsg->path()
          << " "
          << smsg->interface()
          << " "
          << smsg->member() );

      for( std::shared_ptr<signal_proxy_base> sighandler : conn->m_proxy_signals ){
          HandlerResult tmpResult = sighandler->handle_signal( smsg );
          if( tmpResult == HandlerResult::HANDLED ){
              result = HandlerResult::HANDLED;
              // Go on at this point incase there are multiple handles for the signal
          }else if( tmpResult == HandlerResult::NEEDS_MEMORY ){
              result = HandlerResult::NEEDS_MEMORY;
              // Can't do anything else at this point
              break;
          }
      }
    }

    if ( signal_result == HandlerResult::HANDLED or filter_result == FilterResult::FILTER ) return DBUS_HANDLER_RESULT_HANDLED;
    if ( signal_result == HandlerResult::NEEDS_MEMORY or filter_result == FilterResult::NEEDS_MEMORY ) return DBUS_HANDLER_RESULT_NEED_MEMORY;
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  }

  std::string Connection::introspect(const std::string& destination, const std::string& path)
  {
    std::string failed;
    std::ostringstream sout;
    sout << "Introspection of Destination: " << destination << "   Path: " << path << " failed";
    
    failed = sout.str();
    
    if ( destination.empty() or path.empty() ) return failed;
    
    std::shared_ptr<CallMessage> msg = CallMessage::create( destination.c_str(), path.c_str(), DBUS_INTERFACE_INTROSPECTABLE, "Introspect" );
    
    std::shared_ptr<Message> retmsg;
    std::shared_ptr<PendingCall> pending;

    pending = this->send_with_reply_async(msg);
    this->flush();
    pending->block();
    retmsg = pending->steal_reply();

//     retmsg = this->send_with_reply_blocking( msg );

    if (not retmsg) return failed;
    
    std::string retval;
    retmsg >> retval;
    return retval;
  }

}


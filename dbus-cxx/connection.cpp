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
#include <dbus-cxx/errormessage.h>
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
#include "utility.h"
#include "daemon-proxy/DBusDaemonProxy.h"

#include <fcntl.h>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DBUSCXX_REQUEST_NAME_REPLY_PRIMARY_OWNER 0x01
#define DBUSCXX_REQUEST_NAME_REPLY_IN_QUEUE 0x02
#define DBUSCXX_REQUEST_NAME_REPLY_EXISTS 0x03
#define DBUSCXX_REQUEST_NAME_REPLY_ALREADY_OWNER 0x04

namespace sigc { template <typename T_return, typename ...T_arg> class signal; }
namespace sigc { template <typename T_return, typename ...T_arg> class slot; }

static const char* LOGGER_NAME = "DBus.Connection";

namespace DBus
{

struct Connection::ExpectingResponse {
    std::mutex cv_lock;
    std::condition_variable cv;
    std::shared_ptr<Message> reply;
};

  Connection::Connection( BusType type ) :
      m_currentSerial( 1 ),
      m_dispatchingThread( std::this_thread::get_id() ),
      m_dispatchStatus( DispatchStatus::COMPLETE )
  {

    if( type == BusType::SESSION ){
        std::string sessionBusAddr = std::string( getenv( "DBUS_SESSION_BUS_ADDRESS" ) );
        SIMPLELOGGER_DEBUG(LOGGER_NAME, "Going to open session bus: " + sessionBusAddr );
        m_transport = priv::Transport::open_transport( sessionBusAddr );
    }else if( type == BusType::SYSTEM ){
        std::string systemBusAddr = std::string( getenv( "DBUS_SYSTEM_BUS_ADDRESS" ) );
        if( systemBusAddr.empty() ){
            systemBusAddr = "unix:path=/var/run/dbus/system_bus_socket";
        }
        m_transport = priv::Transport::open_transport( systemBusAddr );
    }else if( type == BusType::STARTER ){
         std::string starterBusAddr = std::string( getenv( "DBUS_STARTER_ADDRESS" ) );
         if( starterBusAddr.empty() ){
             SIMPLELOGGER_ERROR("dbus.Connection", "Attempting to connect "
                  "to DBUS_STARTER_ADDRESS, but environment variable not defined or empty" );
         }
         m_transport = priv::Transport::open_transport( starterBusAddr );
    }

    if( !m_transport || !m_transport->is_valid() ){
        SIMPLELOGGER_ERROR("dbus.Connection", "Unable to open transport" );
        return;
    }
  }

  Connection::Connection( std::string address ) :
      m_currentSerial( 1 ),
      m_dispatchingThread( std::this_thread::get_id() ),
      m_dispatchStatus( DispatchStatus::COMPLETE )
  {
      m_transport = priv::Transport::open_transport( address );

     if( !m_transport || !m_transport->is_valid() ){
         SIMPLELOGGER_ERROR(LOGGER_NAME, "Unable to open transport" );
         return;
     }
  }

  std::shared_ptr<Connection> Connection::create( BusType type  )
  {
    std::shared_ptr<Connection> p( new Connection(type) );
    
    return p;
  }

  std::shared_ptr<Connection> Connection::create( std::string address )
  {
    std::shared_ptr<Connection> p( new Connection(address) );

    return p;

  }

  Connection::~Connection()
  {
  }

  Connection::operator bool() const
  {
    return is_valid();
  }

  bool Connection::is_valid() const
  {
    return m_transport.operator bool() && m_transport->is_valid();
  }

  bool Connection::bus_register()
  {
      if( !m_transport || !m_transport->is_valid() ){
          return false;
      }

      if( is_registered() ){
          return true;
      }

      m_daemonProxy = DBus::DBusDaemonProxy::create( shared_from_this() );

      m_uniqueName = m_daemonProxy->Hello();

      return true;
  }

  bool Connection::is_registered() const
  {
    return !m_uniqueName.empty();
  }

  std::string Connection::unique_name() const
  {
    if ( not this->is_valid() ) return std::string("");
    return m_uniqueName;
  }

  unsigned long Connection::unix_user( ) const
  {
//    Error error = Error();
//    if ( not this->is_valid() ) return -1;
//    return dbus_bus_get_unix_user( m_cobj, name.c_str(), error.cobj() );
      return 0;
  }

  const char* Connection::bus_id() const
  {
//    Error error = Error();
//    if ( not this->is_valid() ) return nullptr;
//    return dbus_bus_get_id( m_cobj, error.cobj() );
      return nullptr;
  }

  RequestNameResponse Connection::request_name( const std::string& name, unsigned int flags )
  {
      if( !is_valid() ){
        throw ErrorDisconnected();
      }

      uint32_t retval = m_daemonProxy->RequestName( name, flags );
      switch( retval ){
      case DBUSCXX_REQUEST_NAME_REPLY_PRIMARY_OWNER:
          return RequestNameResponse::PrimaryOwner;
      case DBUSCXX_REQUEST_NAME_REPLY_IN_QUEUE:
          return RequestNameResponse::NameInQueue;
      case DBUSCXX_REQUEST_NAME_REPLY_EXISTS:
          return RequestNameResponse::NameExists;
      case DBUSCXX_REQUEST_NAME_REPLY_ALREADY_OWNER:
          return RequestNameResponse::AlreadyOwner;
      default:
      {
          std::ostringstream msg;
          msg << "Unknown value from request_name:" << retval;
          throw ErrorInvalidReturn( msg.str() );
      }
      }
  }

  int Connection::release_name( const std::string& name )
  {
//    int result;
//    Error error = Error();

//    if ( not this->is_valid() ) return -1;
//    result = dbus_bus_release_name( m_cobj, name.c_str(), error.cobj() );
//    if ( error.is_set() ) throw error;
//    return result;
  }

  bool Connection::name_has_owner( const std::string& name ) const
  {
//    dbus_bool_t result;
//    Error error = Error();

//    if ( not this->is_valid() ) return false;
//    result = dbus_bus_name_has_owner( m_cobj, name.c_str(), error.cobj() );
//    if ( error.is_set() ) throw error;
//    return result;
  }

  StartReply Connection::start_service( const std::string& name, uint32_t flags ) const
  {
//    dbus_bool_t succeeded;
//    dbus_uint32_t result_code;
//    Error error = Error();

//    if ( not this->is_valid() ) return StartReply::FAILED;

//    succeeded = dbus_bus_start_service_by_name( m_cobj, name.c_str(), flags, &result_code, error.cobj() );

//    if ( error.is_set() ) throw error;

//    if ( succeeded )
//      switch ( result_code ) {
//        case DBUS_START_REPLY_SUCCESS: return StartReply::SUCCESS;
//        case DBUS_START_REPLY_ALREADY_RUNNING: return StartReply::ALREADY_RUNNING;
//      }

    return StartReply::FAILED;
  }

  bool Connection::add_match( const std::string& rule )
  {
      if( !is_valid() ){
        throw ErrorDisconnected();
      }

      SIMPLELOGGER_DEBUG(LOGGER_NAME, "Adding the following match: " << rule );

      m_daemonProxy->AddMatch( rule );

    return true;
  }

  void Connection::add_match_nonblocking( const std::string& rule )
  {
//    if ( not this->is_valid() ) return;
//    dbus_bus_add_match( m_cobj, rule.c_str(), nullptr );
  }

  bool Connection::remove_match( const std::string& rule )
  {
//    Error error = Error();

//    if ( not this->is_valid() ) return false;
//    dbus_bus_remove_match( m_cobj, rule.c_str(), error.cobj() );

//    if ( error.is_set() ) return false;
//    return true;
  }

  void Connection::remove_match_nonblocking( const std::string& rule )
  {
//    if ( not this->is_valid() ) return;
//    dbus_bus_remove_match( m_cobj, rule.c_str(), nullptr );
  }

  bool Connection::is_connected() const
  {
//    if ( not this->is_valid() ) return false;
//    return dbus_connection_get_is_connected( m_cobj );
  }

  bool Connection::is_authenticated() const
  {
//    if ( not this->is_valid() ) return false;
//    return dbus_connection_get_is_authenticated( m_cobj );
  }

  bool Connection::is_anonymous() const
  {
//    if ( not this->is_valid() ) return false;
//    return dbus_connection_get_is_anonymous( m_cobj );
  }

  const char* Connection::server_id() const
  {
//    if ( not this->is_valid() ) return nullptr;
//    return dbus_connection_get_server_id( m_cobj );
  }

  uint32_t Connection::send( std::shared_ptr<const Message> msg )
  {
    if ( not this->is_valid() ) throw ErrorDisconnected();
    if ( !msg ) return 0;
    if( m_currentSerial == 0 ) m_currentSerial = 1;

    OutgoingMessage outgoing;
    {
        std::unique_lock<std::mutex> lock( m_outgoingLock );
        outgoing.msg = msg;
        outgoing.serial = m_currentSerial++;
        m_outgoingMessages.push( outgoing );
    }

    notify_dispatcher_or_dispatch();

    return outgoing.serial;
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
    //if ( not dbus_connection_send_with_reply( m_cobj, message->cobj(), &reply, timeout_milliseconds ) )
      throw ErrorNoMemory( "Unable to start asynchronous call" );
    return PendingCall::create( reply );
  }

  std::shared_ptr<ReturnMessage> Connection::send_with_reply_blocking( std::shared_ptr<const CallMessage> message, int timeout_milliseconds )
  {

    if ( not this->is_valid() ) throw ErrorDisconnected();

    if ( not message or not *message ) return std::shared_ptr<ReturnMessage>();

    std::shared_ptr<ReturnMessage> retmsg;
    int msToWait = timeout_milliseconds;

    if( msToWait == -1 ){
        // Use a sane default value
        msToWait = 20000;
    }

    if( m_dispatchingThread == std::this_thread::get_id() ){
        uint32_t replySerialExpceted;
        bool gotReply = false;

        /*
         * We are trying to do a blocking method call in the dispatching thread.
         * Don't queue up this message, just send it.
         */
        {
            std::unique_lock<std::mutex> lock( m_outgoingLock );
            replySerialExpceted = write_single_message( message );
        }

        /*
         * Read messages until we find the one with the serial that we are expecting
         */
        std::vector<int> fds;
        fds.push_back( m_transport->fd() );

        do {
            std::tuple<bool,int,std::vector<int>,std::chrono::milliseconds> fdResponse =
                    DBus::priv::wait_for_fd_activity( fds, msToWait );

            msToWait -= std::get<3>( fdResponse ).count();

            if( msToWait <= 0 ){
                throw ErrorNoReply( "Did not receive a response in the alotted time" );
            }

            if( !m_transport->is_valid() ){
                throw ErrorDisconnected();
            }

            std::shared_ptr<Message> incoming = m_transport->readMessage();
            if( incoming ){
                if( incoming->serial() != replySerialExpceted ){
                    m_incomingMessages.push( incoming );
                    continue;
                }

                // At this point, we definitely have a return to our call.
                // See if it's a returnmessage or an errormessage
                if( incoming->type() == MessageType::ERROR ){
                    std::shared_ptr<ErrorMessage> errmsg = std::static_pointer_cast<ErrorMessage>( incoming );
                    errmsg->throw_error();
                }else if( incoming->type() == MessageType::RETURN ){
                    retmsg = std::static_pointer_cast<ReturnMessage>( incoming );
                    gotReply = true;
                }
            }
        } while( !gotReply );

    }else{
        /*
         * We are trying to do a blocking method call in a thread that is not the dispatcher thread.
         * Queue up the message and notify the dispatcher thread.
         */
        uint32_t serial;
        std::shared_ptr<ExpectingResponse> ex;

        {
            OutgoingMessage outgoing;
            std::scoped_lock<std::mutex,std::mutex> lock( m_outgoingLock, m_expectingResponsesLock );
            outgoing.msg = message;
            outgoing.serial = m_currentSerial++;
            m_outgoingMessages.push( outgoing );
            serial = outgoing.serial;

            // Add this to our expecting responses
            ex = std::make_shared<ExpectingResponse>();
            m_expectingResponses[ serial ] = ex;
        }

        notify_dispatcher_or_dispatch();

        {
            /*
             * Lock on the expecting response; when the response comes in, we will notify from
             * the dispatching thread
             */
            std::unique_lock<std::mutex> lock( ex->cv_lock );
            std::cv_status status = ex->cv.wait_for( lock, std::chrono::milliseconds( msToWait ) );
            std::shared_ptr<ExpectingResponse> resp;

            {
                /*
                 * Now remove our expecting response to free up memory
                 */
                std::unique_lock<std::mutex> lock( m_expectingResponsesLock );
                std::map<uint32_t,std::shared_ptr<ExpectingResponse>>::iterator it =
                        m_expectingResponses.find( serial );
                resp = (*it).second;

                m_expectingResponses.erase( it );
            }

            if( !resp ){
                throw ErrorUnexpectedResponse();
            }

            if( status == std::cv_status::no_timeout ){
                std::shared_ptr<Message> gotMessage = resp->reply;
                if( gotMessage->type() == MessageType::RETURN ){
                    retmsg = std::static_pointer_cast<ReturnMessage>( gotMessage );
                }else if( gotMessage->type() == MessageType::ERROR ){
                    std::shared_ptr<ErrorMessage> errmsg = std::static_pointer_cast<ErrorMessage>( gotMessage );
                    errmsg->throw_error();
                }else if( gotMessage->type() == MessageType::SIGNAL ){
                }else{
                    throw ErrorUnknown( "Why are we here" );
                }
            }else{
                throw ErrorNoReply( "Did not receive a response in the alotted time" );
            }
        }
    }

    return retmsg;
  }

  void Connection::flush()
  {
    if ( not this->is_valid() ) return;
    {
        std::unique_lock lock( m_outgoingLock );
        while( !m_outgoingMessages.empty() ){
            OutgoingMessage outgoing = m_outgoingMessages.front();
            m_outgoingMessages.pop();

            m_transport->writeMessage( outgoing.msg, outgoing.serial );
        }
    }
  }

  uint32_t Connection::write_single_message( std::shared_ptr<const Message> msg ){
      uint32_t retval = m_currentSerial;
      m_transport->writeMessage( msg, m_currentSerial++ );
      return retval;
  }

  DispatchStatus Connection::dispatch_status( ) const
  {
    if ( not this->is_valid() ) return DispatchStatus::COMPLETE;
    return m_dispatchStatus;
  }

  DispatchStatus Connection::dispatch( )
  {
      if( std::this_thread::get_id() != m_dispatchingThread ){
          throw ErrorIncorrectDispatchThread( "Calling Connection::dispatch from non-dispatching thread" );
      }
    if ( not this->is_valid() ){
        m_dispatchStatus = DispatchStatus::COMPLETE;
        return DispatchStatus::COMPLETE;
    }

    // Write out any messages we have waiting to be written
    flush();

    // Try to read a message
    {
        SIMPLELOGGER_DEBUG("DBus.Connection", "Try to read a message" );
        std::shared_ptr<Message> incoming = m_transport->readMessage();
        if( incoming ){
            m_incomingMessages.push( incoming );
        }
    }

    // Process any messages that we need to
    process_single_message();

    if( m_outgoingMessages.empty() &&
            m_incomingMessages.empty() ){
        m_dispatchStatus = DispatchStatus::COMPLETE;
    }else{
        m_dispatchStatus = DispatchStatus::DATA_REMAINS;
    }

    return m_dispatchStatus;
  }

  void Connection::process_single_message(){
      std::shared_ptr<Message> msgToProcess;

      if( m_incomingMessages.empty() ) return;

      msgToProcess = m_incomingMessages.front();
      m_incomingMessages.pop();

      if( msgToProcess->type() == MessageType::RETURN ||
              msgToProcess->type() == MessageType::ERROR ){
          uint32_t reply_serial;

          if( msgToProcess->type() == MessageType::RETURN ){
              reply_serial = std::static_pointer_cast<ReturnMessage>( msgToProcess )->reply_serial();
          }else{
              reply_serial = std::static_pointer_cast<ErrorMessage>( msgToProcess )->reply_serial();
          }

          if( m_expectingResponses.find( reply_serial ) != m_expectingResponses.end() ) {
              // This is a response to something that a different thread is waiting for.
              // Update the data and notify the thread!
              m_expectingResponses[ reply_serial ]->reply = msgToProcess;
              m_expectingResponses[ reply_serial ]->cv.notify_one();
              return;
          }
      }

      std::shared_ptr<CallMessage> callmsg;
      std::string path;
      PathHandlingEntry entry;
      bool error = false;

      if( msgToProcess->type() == MessageType::CALL ) {
          callmsg = std::static_pointer_cast<CallMessage>( msgToProcess );
          path = callmsg->path();
      }else if( msgToProcess->type() == MessageType::SIGNAL ){
          std::shared_ptr<SignalMessage> signalmsg = std::static_pointer_cast<SignalMessage>( msgToProcess );
          path = signalmsg->path();
      }else{
        SIMPLELOGGER_ERROR("DBus.Connection", "Unable to process message: invalid type " << msgToProcess->type() );
        return;
      }

      {
          std::unique_lock<std::mutex> lock( m_pathHandlerLock );
          std::map<std::string,PathHandlingEntry>::iterator it;
          it = m_path_handler.find( path );
          if( it != m_path_handler.end() ){
              entry = it->second;
          }else{
              error = true;
          }
      }

      if( error && callmsg ){
          std::shared_ptr<ErrorMessage> errMsg =
                  ErrorMessage::create( callmsg, DBUSCXX_ERROR_FAILED, "Could not find given path" );
          send( errMsg );
          return;
      }

      // TODO need to figure out how to manage the signals

      if( entry.handlingThread == m_dispatchingThread ){
          // We are in the dispatching thread here, so we can simply call the handle method
          //entry.handler->handle_message();
          if( callmsg ){
            entry.handler->handle_call_message( callmsg );
          }
      }else{
          // A different thread needs to handle this.
          std::shared_ptr<ThreadDispatcher> disp = m_threadDispatchers[ entry.handlingThread ].lock();

          if( !disp ){
              // Remove all invalid thread dispatchers, return an error
              remove_invalid_threaddispatchers_and_associated_objects();

              if( callmsg ){
                  std::shared_ptr<ErrorMessage> errMsg =
                          ErrorMessage::create( callmsg, DBUSCXX_ERROR_FAILED, "Could not find given path" );
                  send( errMsg );
                  return;
              }
          }else{
              disp->add_message( entry.handler, msgToProcess );
          }
      }
  }

  int Connection::unix_fd() const
  {
    if ( not this->is_valid() ) return -1;
    return m_transport->fd();
  }

  int Connection::socket() const
  {
      if ( not this->is_valid() ) return -1;
      return m_transport->fd();
  }

  unsigned long Connection::unix_process_id() const
  {
//    dbus_bool_t result;
//    unsigned long pid;
//    if ( not this->is_valid() ) return -1;
//    result = dbus_connection_get_unix_process_id( m_cobj, &pid );
//    if ( not result ) return -1;
//    return pid;
  }

  void Connection::set_allow_anonymous(bool allow)
  {
//    if ( not this->is_valid() ) return;
//    dbus_connection_set_allow_anonymous( m_cobj, allow );
  }

  void Connection::set_route_peer_messages(bool route)
  {
//    if ( not this->is_valid() ) return;
//    dbus_connection_set_route_peer_messages( m_cobj, route );
  }

  void Connection::set_max_message_size(long size)
  {
//    if ( not this->is_valid() ) return;
//    dbus_connection_set_max_message_size( m_cobj, size );
  }

  long Connection::max_message_size()
  {
//    if ( not this->is_valid() ) return -1;
//    return dbus_connection_get_max_message_size(m_cobj);
  }

  void Connection::set_max_received_size(long size)
  {
//    if ( not this->is_valid() ) return;
//    dbus_connection_set_max_received_size( m_cobj, size );
  }

  long Connection::max_received_size()
  {
//    if ( not this->is_valid() ) return -1;
//    return dbus_connection_get_max_received_size(m_cobj);
  }

  long Connection::outgoing_size()
  {
//    if ( not this->is_valid() ) return -1;
//    return dbus_connection_get_outgoing_size(m_cobj);
  }

  bool Connection::has_messages_to_send()
  {
//    if ( not this->is_valid() ) return false;
//    return dbus_connection_has_messages_to_send(m_cobj);
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

  std::shared_ptr<Object> Connection::create_object(const std::string & path, ThreadForCalling calling)
  {
    std::shared_ptr<Object> object = Object::create( path );
    if (not object) return object;
    if( register_object( object, calling ) != RegistrationStatus::Success ){
        return std::shared_ptr<Object>();
    }
    return object;
  }

  RegistrationStatus Connection::register_object(std::shared_ptr<ObjectPathHandler> object, ThreadForCalling calling)
  {
    if ( not object ) return RegistrationStatus::Failed_Invalid_Object;

    SIMPLELOGGER_DEBUG("dbus.Connection", "Connection::register_object at path " << object->path() );

    std::unique_lock<std::mutex> lock( m_pathHandlerLock );
    if( m_path_handler.find( object->path() ) != m_path_handler.end() ){
        return RegistrationStatus::Failed_Path_in_Use;
    }

    PathHandlingEntry entry;
    entry.handler = object;
    if( calling == ThreadForCalling::DispatcherThread ){
        entry.handlingThread = m_dispatchingThread;
    }else{
        entry.handlingThread = std::this_thread::get_id();
    }
    m_path_handler[ object->path() ] = entry;

    object->set_connection( shared_from_this() );

    return RegistrationStatus::Success;
  }

  std::shared_ptr<ObjectProxy> Connection::create_object_proxy(const std::string & path)
  {
    std::shared_ptr<ObjectProxy> object = ObjectProxy::create(shared_from_this(), path);
    return object;
  }

  std::shared_ptr<ObjectProxy> Connection::create_object_proxy(const std::string & destination, const std::string & path)
  {
    std::shared_ptr<ObjectProxy> object = ObjectProxy::create(shared_from_this(), destination, path);
    return object;
  }

  bool Connection::unregister_object(const std::string & path)
  {
    std::unique_lock<std::mutex> lock( m_pathHandlerLock );
    std::map<std::string,PathHandlingEntry>::iterator it;
    it = m_path_handler.find( path );
    if( it != m_path_handler.end() ){
        m_path_handler.erase( it );
        return true;
    }

    return false;
  }

  std::shared_ptr<signal_proxy_base> Connection::add_signal_proxy(std::shared_ptr<signal_proxy_base> signal)
  {
    if ( not signal ) return std::shared_ptr<signal_proxy_base>();
    
    SIMPLELOGGER_DEBUG( "dbus.Connection", "Adding signal " << signal->interface() << ":" << signal->name() );

    if ( signal->connection() ) signal->connection()->remove_signal_proxy(signal);

    m_proxy_signals.push_back( signal );

    this->add_match( signal->match_rule() );
    signal->set_connection( shared_from_this() );

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
    
    std::shared_ptr<Connection> conn;// = static_cast<Connection*>(data)->self();
    FilterResult filter_result = FilterResult::DONT_FILTER;
    HandlerResult signal_result = HandlerResult::NOT_HANDLED;
    std::shared_ptr<Message> msg;// = Message::create(message);

    filter_result = conn->signal_filter().emit(conn, msg);

    SIMPLELOGGER_DEBUG( "dbus.Connection", "Filter callback.  filter_result: " << static_cast<int>( filter_result ) );

    // Deliver signals to signal proxies
    if ( filter_result != FilterResult::FILTER and msg->type() == MessageType::SIGNAL )
    {
      std::shared_ptr<SignalMessage> smsg;// = SignalMessage::create(msg);
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

  void Connection::set_dispatching_thread( std::thread::id tid ){
      m_dispatchingThread = tid;
  }

  void Connection::set_dispatching_thread_wakeup_func( sigc::slot<void()> func ){
      m_notifyDispatcherFunc = func;
  }

  void Connection::notify_dispatcher_or_dispatch(){
      m_dispatchStatus = DispatchStatus::DATA_REMAINS;

      if( std::this_thread::get_id() == m_dispatchingThread ){
          dispatch();
      }else{
          m_notifyDispatcherFunc();
      }
  }

  void Connection::add_thread_dispatcher( std::weak_ptr<ThreadDispatcher> disp ){
    std::unique_lock<std::mutex> lock( m_threadDispatcherLock );
    m_threadDispatchers[ std::this_thread::get_id() ] = disp;
  }

  void Connection::remove_invalid_threaddispatchers_and_associated_objects(){
      std::vector<std::thread::id> invalidThreadIds;

      {
          std::unique_lock<std::mutex> lock( m_threadDispatcherLock );
          for( auto it = m_threadDispatchers.begin();
               it != m_threadDispatchers.end(); ){
              if( it->second.expired() ){
                  invalidThreadIds.push_back( it->first );
                  it = m_threadDispatchers.erase( it );
              }else{
                  it++;
              }
          }
      }

      if( invalidThreadIds.empty() ) return;

      {
          std::unique_lock<std::mutex> lock( m_pathHandlerLock );
          for( auto it = m_path_handler.begin(); it != m_path_handler.end(); ){
              if( std::find( invalidThreadIds.begin(), invalidThreadIds.end(), it->second.handlingThread ) != invalidThreadIds.end() ){
                  it = m_path_handler.erase( it );
              }else{
                  it++;
              }
          }
      }
  }

}


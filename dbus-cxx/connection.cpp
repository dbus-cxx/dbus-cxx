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
#include "transport.h"
#include "simpletransport.h"
#include <poll.h>
#include "utility.h"
#include "daemon-proxy/DBusDaemonProxy.h"

#include <cstring>
#include <fcntl.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/socket.h>

#define DBUSCXX_REQUEST_NAME_REPLY_PRIMARY_OWNER 0x01
#define DBUSCXX_REQUEST_NAME_REPLY_IN_QUEUE 0x02
#define DBUSCXX_REQUEST_NAME_REPLY_EXISTS 0x03
#define DBUSCXX_REQUEST_NAME_REPLY_ALREADY_OWNER 0x04

#define DBUSCXX_RELEASE_NAME_REPLY_RELEASED 0x01
#define DBUSCXX_RELEASE_NAME_REPLY_NON_EXISTENT 0x02
#define DBUSCXX_RELEASE_NAME_REPLY_NOT_OWNER 0x03

#define DBUSCXX_START_REPLY_SUCCESS 0x01
#define DBUSCXX_START_REPLY_ALREADY_RUNNING 0x02

#if defined( _WIN32 ) && defined( ERROR )
#undef ERROR
#endif

namespace sigc { template <typename T_return, typename ...T_arg> class signal; }
namespace sigc { template <typename T_return, typename ...T_arg> class slot; }

static const char* LOGGER_NAME = "DBus.Connection";

namespace DBus
{

struct ExpectingResponse {
    std::mutex cv_lock;
    std::condition_variable cv;
    std::shared_ptr<Message> reply;
};

struct OutgoingMessage{
    std::shared_ptr<const Message> msg;
    uint32_t serial;
};

struct PathHandlingEntry{
    std::shared_ptr<ObjectPathHandler> handler;
    std::thread::id handlingThread;
};

class Connection::priv_data{
public:
    priv_data() :
        m_currentSerial( 1 ),
        m_dispatchingThread( std::this_thread::get_id() ),
        m_dispatchStatus( DispatchStatus::COMPLETE )
    {}

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
    std::mutex m_proxySignalsLock;
    std::vector<std::shared_ptr<signal_proxy_base>> m_proxySignals;
    std::vector<std::shared_ptr<signal_proxy_base>> m_allProxySignals;


  FilterSignal m_filter_signal;
};

  Connection::Connection( BusType type )
  {
      m_priv = std::make_unique<priv_data>();

    if( type == BusType::SESSION ){
        char* env_address = getenv( "DBUS_SESSION_BUS_ADDRESS" );
        if( env_address == nullptr ){
            return;
        }
        std::string sessionBusAddr = std::string( env_address );
        SIMPLELOGGER_DEBUG(LOGGER_NAME, "Going to open session bus: " + sessionBusAddr );
        m_priv->m_transport = priv::Transport::open_transport( sessionBusAddr );
    }else if( type == BusType::SYSTEM ){
        char* env_address = getenv( "DBUS_SYSTEM_BUS_ADDRESS" );
        std::string systemBusAddr;

        if( env_address != nullptr ){
            systemBusAddr = std::string( env_address );
        }
        if( systemBusAddr.empty() ){
            systemBusAddr = "unix:path=/var/run/dbus/system_bus_socket";
        }
        m_priv->m_transport = priv::Transport::open_transport( systemBusAddr );
    }else if( type == BusType::STARTER ){
        char* env_address = getenv( "DBUS_STARTER_ADDRESS" );
        std::string starterBusAddr;

        if( env_address != nullptr ){
            starterBusAddr = std::string( env_address );
        }
        if( starterBusAddr.empty() ){
             SIMPLELOGGER_ERROR(LOGGER_NAME, "Attempting to connect "
                  "to DBUS_STARTER_ADDRESS, but environment variable not defined or empty" );
        }
        m_priv->m_transport = priv::Transport::open_transport( starterBusAddr );
    }

    if( !m_priv->m_transport || !m_priv->m_transport->is_valid() ){
        SIMPLELOGGER_ERROR("dbus.Connection", "Unable to open transport" );
        return;
    }
  }

  Connection::Connection( std::string address )
  {
      m_priv = std::make_unique<priv_data>();
      m_priv->m_transport = priv::Transport::open_transport( address );

     if( !m_priv->m_transport || !m_priv->m_transport->is_valid() ){
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
    return m_priv->m_transport.operator bool() && m_priv->m_transport->is_valid();
  }

  bool Connection::bus_register()
  {
      if( !m_priv->m_transport || !m_priv->m_transport->is_valid() ){
          return false;
      }

      if( is_registered() ){
          return true;
      }

      m_priv->m_daemonProxy = DBus::DBusDaemonProxy::create( shared_from_this() );

      m_priv->m_uniqueName = m_priv->m_daemonProxy->Hello();

      return true;
  }

  bool Connection::is_registered() const
  {
    return !m_priv->m_uniqueName.empty();
  }

  std::string Connection::unique_name() const
  {
    if ( !this->is_valid() ) return std::string("");
    return m_priv->m_uniqueName;
  }

  RequestNameResponse Connection::request_name( const std::string& name, unsigned int flags )
  {
      if( !is_valid() ){
        throw ErrorDisconnected();
      }

      uint32_t retval = m_priv->m_daemonProxy->RequestName( name, flags );
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

  ReleaseNameResponse Connection::release_name( const std::string& name )
  {
    uint32_t retval = m_priv->m_daemonProxy->ReleaseName( name );
    switch( retval ){
    case DBUSCXX_RELEASE_NAME_REPLY_RELEASED:
        return ReleaseNameResponse::NameReleased;
    case DBUSCXX_RELEASE_NAME_REPLY_NOT_OWNER:
        return ReleaseNameResponse::NotOwner;
    case DBUSCXX_RELEASE_NAME_REPLY_NON_EXISTENT:
        return ReleaseNameResponse::NameNonExistant;
    default:
        std::ostringstream msg;
        msg << "Unknown value from release_name:" << retval;
        throw ErrorInvalidReturn( msg.str() );
    }
  }

  bool Connection::name_has_owner( const std::string& name ) const
  {
    return m_priv->m_daemonProxy->NameHasOwner( name );
  }

  StartReply Connection::start_service( const std::string& name, uint32_t flags ) const
  {
      uint32_t retval = m_priv->m_daemonProxy->StartServiceByName( name, flags );

      switch( retval ){
      case DBUSCXX_START_REPLY_SUCCESS:
          return StartReply::SUCCESS;
      case DBUSCXX_START_REPLY_ALREADY_RUNNING:
          return StartReply::ALREADY_RUNNING;
      default:
          std::ostringstream msg;
          msg << "Unknown value from start_service:" << retval;
          throw ErrorInvalidReturn( msg.str() );
      }
  }

  bool Connection::add_match( const std::string& rule )
  {
      if( !is_valid() ){
        throw ErrorDisconnected();
      }

      SIMPLELOGGER_DEBUG(LOGGER_NAME, "Adding the following match: " << rule );

      if( m_priv->m_daemonProxy ){
        m_priv->m_daemonProxy->AddMatch( rule );
      }

    return true;
  }

  void Connection::add_match_nonblocking( const std::string& rule )
  {
//    if ( not this->is_valid() ) return;
//    dbus_bus_add_match( m_cobj, rule.c_str(), nullptr );
  }

  bool Connection::remove_match( const std::string& rule )
  {
      m_priv->m_daemonProxy->RemoveMatch( rule );
    return true;
  }

  bool Connection::is_connected() const
  {
//    if ( not this->is_valid() ) return false;
//    return dbus_connection_get_is_connected( m_cobj );
      return false;
  }

  bool Connection::is_authenticated() const
  {
//    if ( not this->is_valid() ) return false;
//    return dbus_connection_get_is_authenticated( m_cobj );
      return false;
  }

  bool Connection::is_anonymous() const
  {
//    if ( not this->is_valid() ) return false;
//    return dbus_connection_get_is_anonymous( m_cobj );
      return false;
  }

  const char* Connection::server_id() const
  {
//    if ( not this->is_valid() ) return nullptr;
//    return dbus_connection_get_server_id( m_cobj );
      return "";
  }

  uint32_t Connection::send( std::shared_ptr<const Message> msg )
  {
    if ( !this->is_valid() ) throw ErrorDisconnected();
    if ( !msg ) return 0;
    if( m_priv->m_currentSerial == 0 ) m_priv->m_currentSerial = 1;

    OutgoingMessage outgoing;
    {
        std::unique_lock<std::mutex> lock( m_priv->m_outgoingLock );
        outgoing.msg = msg;
        outgoing.serial = m_priv->m_currentSerial++;
        m_priv->m_outgoingMessages.push( outgoing );
    }

    notify_dispatcher_or_dispatch();

    return outgoing.serial;
  }

  Connection & Connection::operator <<(std::shared_ptr<const Message> msg)
  {
    if ( msg ) this->send(msg);
    return *this;
  }

//  std::shared_ptr<PendingCall> Connection::send_with_reply_async( std::shared_ptr<const Message> message, int timeout_milliseconds )
//  {
//    if ( not this->is_valid() ) throw ErrorDisconnected();
//    if ( not message or not *message ) return std::shared_ptr<PendingCall>();
//    uint32_t reply_serial = send( message );
//    //if ( not dbus_connection_send_with_reply( m_cobj, message->cobj(), &reply, timeout_milliseconds ) )
//      throw ErrorNoMemory( "Unable to start asynchronous call" );
//    return PendingCall::create();
//  }

  std::shared_ptr<ReturnMessage> Connection::send_with_reply_blocking( std::shared_ptr<const CallMessage> message, int timeout_milliseconds )
  {

    if ( !this->is_valid() ) throw ErrorDisconnected();

    if ( !message ) return std::shared_ptr<ReturnMessage>();

    std::shared_ptr<ReturnMessage> retmsg;
    int msToWait = timeout_milliseconds;

    if( msToWait == -1 ){
        // Use a sane default value
        msToWait = 20000;
    }

    if( m_priv->m_dispatchingThread == std::this_thread::get_id() ){
        uint32_t replySerialExpceted;
        bool gotReply = false;

        /*
         * We are trying to do a blocking method call in the dispatching thread.
         * Don't queue up this message, just send it.
         */
        {
            std::unique_lock<std::mutex> lock( m_priv->m_outgoingLock );
            replySerialExpceted = write_single_message( message );
        }

        /*
         * Read messages until we find the one with the serial that we are expecting
         */
        std::vector<int> fds;
        fds.push_back( m_priv->m_transport->fd() );

        do {
            std::tuple<bool,int,std::vector<int>,std::chrono::milliseconds> fdResponse =
                    DBus::priv::wait_for_fd_activity( fds, msToWait );

            msToWait -= std::get<3>( fdResponse ).count();

            if( msToWait <= 0 ){
                throw ErrorNoReply( "Did not receive a response in the alotted time" );
            }

            if( !m_priv->m_transport->is_valid() ){
                throw ErrorDisconnected();
            }

            std::shared_ptr<Message> incoming = m_priv->m_transport->readMessage();
            {
                std::ostringstream str;
                str << incoming.get();
                SIMPLELOGGER_DEBUG( LOGGER_NAME, "Got incoming " << str.str() );
            }
            if( incoming ){
                // Check to see what type of message we have, and if it might be a reply to our
                // method call.
                if( incoming->type() == MessageType::ERROR ){
                    std::shared_ptr<ErrorMessage> errmsg = std::static_pointer_cast<ErrorMessage>( incoming );
                    if( errmsg->reply_serial() == replySerialExpceted ){
                        errmsg->throw_error();
                    }
                }else if( incoming->type() == MessageType::RETURN ){
                    retmsg = std::static_pointer_cast<ReturnMessage>( incoming );
                    if( retmsg->reply_serial() == replySerialExpceted ){
                        gotReply = true;
                    }
                }

                if( !gotReply ){
                    m_priv->m_incomingMessages.push( incoming );
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
            std::scoped_lock<std::mutex,std::mutex> lock( m_priv->m_outgoingLock, m_priv->m_expectingResponsesLock );
            outgoing.msg = message;
            outgoing.serial = m_priv->m_currentSerial++;
            m_priv->m_outgoingMessages.push( outgoing );
            serial = outgoing.serial;

            // Add this to our expecting responses
            ex = std::make_shared<ExpectingResponse>();
            m_priv->m_expectingResponses[ serial ] = ex;
        }

        notify_dispatcher_or_dispatch();

        {
            /*
             * Lock on the expecting response; when the response comes in, we will notify from
             * the dispatching thread
             */
            std::unique_lock<std::mutex> lock( ex->cv_lock );
            bool status = ex->cv.wait_for( lock, std::chrono::milliseconds( msToWait ), [this, serial]{
                std::unique_lock<std::mutex> lock( m_priv->m_expectingResponsesLock );
                std::map<uint32_t,std::shared_ptr<ExpectingResponse>>::iterator it =
                        m_priv->m_expectingResponses.find( serial );

                // return false if the waiting should be continued
                return (*it).second->reply.get() != nullptr;
            } );
            std::shared_ptr<ExpectingResponse> resp;

            {
                /*
                 * Now remove our expecting response to free up memory
                 */
                std::unique_lock<std::mutex> lock( m_priv->m_expectingResponsesLock );
                std::map<uint32_t,std::shared_ptr<ExpectingResponse>>::iterator it =
                        m_priv->m_expectingResponses.find( serial );
                resp = (*it).second;

                m_priv->m_expectingResponses.erase( it );
            }

            if( !resp ){
                throw ErrorUnexpectedResponse();
            }

            if( status ){
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
    if ( !this->is_valid() ) return;
    {
        std::unique_lock lock( m_priv->m_outgoingLock );
        while( !m_priv->m_outgoingMessages.empty() ){
            OutgoingMessage outgoing = m_priv->m_outgoingMessages.front();
            m_priv->m_outgoingMessages.pop();

            m_priv->m_transport->writeMessage( outgoing.msg, outgoing.serial );
        }
    }
  }

  uint32_t Connection::write_single_message( std::shared_ptr<const Message> msg ){
      uint32_t retval = m_priv->m_currentSerial;
      m_priv->m_transport->writeMessage( msg, m_priv->m_currentSerial++ );
      return retval;
  }

  DispatchStatus Connection::dispatch_status( ) const
  {
    if ( !this->is_valid() ) return DispatchStatus::COMPLETE;
    return m_priv->m_dispatchStatus;
  }

  DispatchStatus Connection::dispatch( )
  {
      if( std::this_thread::get_id() != m_priv->m_dispatchingThread ){
          throw ErrorIncorrectDispatchThread( "Calling Connection::dispatch from non-dispatching thread" );
      }
    if ( !this->is_valid() ){
        m_priv->m_dispatchStatus = DispatchStatus::COMPLETE;
        return DispatchStatus::COMPLETE;
    }

    // Write out any messages we have waiting to be written
    flush();

    // Try to read a message
    {
        SIMPLELOGGER_DEBUG("DBus.Connection", "Try to read a message" );
        std::shared_ptr<Message> incoming = m_priv->m_transport->readMessage();
        if( incoming ){
            m_priv->m_incomingMessages.push( incoming );
        }
    }

    // Process any messages that we need to
    process_single_message();

    if( m_priv->m_outgoingMessages.empty() &&
            m_priv->m_incomingMessages.empty() ){
        m_priv->m_dispatchStatus = DispatchStatus::COMPLETE;
    }else{
        m_priv->m_dispatchStatus = DispatchStatus::DATA_REMAINS;
    }

    return m_priv->m_dispatchStatus;
  }

  void Connection::process_single_message(){
      std::shared_ptr<Message> msgToProcess;

      if( m_priv->m_incomingMessages.empty() ) return;

      msgToProcess = m_priv->m_incomingMessages.front();
      m_priv->m_incomingMessages.pop();

      if( msgToProcess->type() == MessageType::RETURN ||
              msgToProcess->type() == MessageType::ERROR ){
          uint32_t reply_serial;

          if( msgToProcess->type() == MessageType::RETURN ){
              reply_serial = std::static_pointer_cast<ReturnMessage>( msgToProcess )->reply_serial();
          }else{
              reply_serial = std::static_pointer_cast<ErrorMessage>( msgToProcess )->reply_serial();
          }

          if( m_priv->m_expectingResponses.find( reply_serial ) != m_priv->m_expectingResponses.end() ) {
              // This is a response to something that a different thread is waiting for.
              // Update the data and notify the thread!
              m_priv->m_expectingResponses[ reply_serial ]->reply = msgToProcess;
              m_priv->m_expectingResponses[ reply_serial ]->cv.notify_one();
              return;
          }
      }

      std::shared_ptr<CallMessage> callmsg;

      if( msgToProcess->type() == MessageType::CALL ) {
          callmsg = std::static_pointer_cast<CallMessage>( msgToProcess );
          process_call_message( callmsg );
      }else if( msgToProcess->type() == MessageType::SIGNAL ){
          std::shared_ptr<SignalMessage> signalmsg = std::static_pointer_cast<SignalMessage>( msgToProcess );
          process_signal_message( signalmsg );
      }else{
        SIMPLELOGGER_ERROR("DBus.Connection", "Unable to process message: invalid type " << msgToProcess->type() );
        return;
      }
  }

  void Connection::process_call_message( std::shared_ptr<const CallMessage> callmsg ){
      std::string path = callmsg->path();
      PathHandlingEntry entry;
      bool error = false;

      {
          std::unique_lock<std::mutex> lock( m_priv->m_pathHandlerLock );
          std::map<std::string,PathHandlingEntry>::iterator it;
          it = m_priv->m_path_handler.find( path );
          if( it != m_priv->m_path_handler.end() ){
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

      if( entry.handlingThread == m_priv->m_dispatchingThread ){
          // We are in the dispatching thread here, so we can simply call the handle method
          HandlerResult res = entry.handler->handle_message( callmsg );
          send_error_on_handler_result( callmsg, res );
      }else{
          // A different thread needs to handle this.
          std::shared_ptr<ThreadDispatcher> disp = m_priv->m_threadDispatchers[ entry.handlingThread ].lock();

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
              disp->add_message( entry.handler, callmsg );
          }
      }
  }

  void Connection::process_signal_message( std::shared_ptr<const SignalMessage> msg ){
      {
          // See if any of our handlers can handle this
          std::unique_lock<std::mutex> lock( m_priv->m_proxySignalsLock );
          for( std::shared_ptr<signal_proxy_base> handler : m_priv->m_proxySignals ){
            handler->handle_signal( msg );
          }
      }

      // Give this signal to all of our ThreadDispatchers as well
      {
          std::unique_lock<std::mutex> lock( m_priv->m_threadDispatcherLock );
          for( auto const& x : m_priv->m_threadDispatchers ){
              std::shared_ptr<ThreadDispatcher> disp = x.second.lock();
              if( disp ){
                  disp->add_signal( msg );
              }
          }
      }
  }


  void Connection::send_error_on_handler_result(std::shared_ptr<const CallMessage> callmsg, HandlerResult result){
      if( result == HandlerResult::Handled ){
          return;
      }

      std::shared_ptr<ErrorMessage> errMsg = callmsg->create_error_reply();
      std::ostringstream strErrMsg;

      switch( result ){
      case HandlerResult::Invalid_Path:
          strErrMsg << "dbus-cxx: could not find path "
                    << callmsg->path();
          errMsg->set_name( DBUSCXX_ERROR_FAILED );
          errMsg->set_message( strErrMsg.str() );
          break;
      case HandlerResult::Invalid_Method:
          strErrMsg << "dbus-cxx: unable to find method named "
                    << callmsg->member()
                    << " on interface "
                    << callmsg->interface_name();
          errMsg->set_name( DBUSCXX_ERROR_UNKNOWN_METHOD );
          errMsg->set_message( strErrMsg.str() );
          break;
      case HandlerResult::Invalid_Interface:
          strErrMsg << "dbus-cxx: unable to find interface named "
                    << callmsg->interface_name();
          errMsg->set_name( DBUSCXX_ERROR_UNKNOWN_INTERFACE );
          errMsg->set_message( strErrMsg.str() );
          break;
      default:
          break;
      }

      send( errMsg );
  }

  int Connection::unix_fd() const
  {
    if ( !this->is_valid() ) return -1;
    return m_priv->m_transport->fd();
  }

  int Connection::socket() const
  {
      if ( !this->is_valid() ) return -1;
      return m_priv->m_transport->fd();
  }

  bool Connection::has_messages_to_send()
  {
    if ( !this->is_valid() ) return false;
    return !m_priv->m_outgoingMessages.empty();
  }

  sigc::signal< void() > & Connection::signal_needs_dispatch()
  {
    return m_priv->m_needsDispatching;
  }

  FilterSignal& Connection::signal_filter()
  {
    return m_priv->m_filter_signal;
  }

  std::shared_ptr<Object> Connection::create_object(const std::string & path, ThreadForCalling calling)
  {
    std::shared_ptr<Object> object = Object::create( path );
    if (!object) return object;
    if( register_object( object, calling ) != RegistrationStatus::Success ){
        return std::shared_ptr<Object>();
    }
    return object;
  }

  RegistrationStatus Connection::register_object(std::shared_ptr<ObjectPathHandler> object, ThreadForCalling calling)
  {
    if ( !object ) return RegistrationStatus::Failed_Invalid_Object;

    SIMPLELOGGER_DEBUG("dbus.Connection", "Connection::register_object at path " << object->path() );

    std::unique_lock<std::mutex> lock( m_priv->m_pathHandlerLock );
    if( m_priv->m_path_handler.find( object->path() ) != m_priv->m_path_handler.end() ){
        return RegistrationStatus::Failed_Path_in_Use;
    }

    PathHandlingEntry entry;
    entry.handler = object;
    if( calling == ThreadForCalling::DispatcherThread ){
        entry.handlingThread = m_priv->m_dispatchingThread;
    }else{
        entry.handlingThread = std::this_thread::get_id();
    }
    m_priv->m_path_handler[ object->path() ] = entry;

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
    std::unique_lock<std::mutex> lock( m_priv->m_pathHandlerLock );
    std::map<std::string,PathHandlingEntry>::iterator it;
    it = m_priv->m_path_handler.find( path );
    if( it != m_priv->m_path_handler.end() ){
        m_priv->m_path_handler.erase( it );
        return true;
    }

    return false;
  }

  std::shared_ptr<signal_proxy_base> Connection::add_signal_proxy(std::shared_ptr<signal_proxy_base> signal, ThreadForCalling calling)
  {
    if ( !signal ) return std::shared_ptr<signal_proxy_base>();
    
    SIMPLELOGGER_DEBUG( LOGGER_NAME, "Adding signal " << signal->interface_name() << ":" << signal->name() );

    if ( signal->connection() ) signal->connection()->remove_signal_proxy(signal);

    m_priv->m_allProxySignals.push_back( signal );

    if( calling == ThreadForCalling::DispatcherThread ||
            (calling == ThreadForCalling::CurrentThread &&
              std::this_thread::get_id() == m_priv->m_dispatchingThread) ){
        // We can call this from the dispatch thread, or we want it to be called from
        // the current thread(which happens to be the dispatch thread)
        std::unique_lock<std::mutex> lock( m_priv->m_proxySignalsLock );
        m_priv->m_proxySignals.push_back( signal );
    }else{
        // We need to give this signal to the appropriate ThreadDispatcher to handle
        std::unique_lock<std::mutex> lock( m_priv->m_threadDispatcherLock );
        std::map<std::thread::id,std::weak_ptr<ThreadDispatcher>>::iterator iter =
                m_priv->m_threadDispatchers.find( std::this_thread::get_id() );

        if( iter == m_priv->m_threadDispatchers.end() ){
            SIMPLELOGGER_ERROR( LOGGER_NAME, "Unable to find a thread dispatcher on current thread, not adding signal proxy" );
            return std::shared_ptr<signal_proxy_base>();
        }

        std::shared_ptr<ThreadDispatcher> thrDispatch = iter->second.lock();
        if( !thrDispatch ){
            SIMPLELOGGER_ERROR( LOGGER_NAME, "Unable to find a valid thread dispatcher on current thread, not adding signal proxy" );
            return std::shared_ptr<signal_proxy_base>();
        }

        thrDispatch->add_signal_proxy( signal );
    }

    this->add_match( signal->match_rule() );
    signal->set_connection( shared_from_this() );

    return signal;
  }

  bool Connection::remove_signal_proxy( std::shared_ptr<signal_proxy_base> signal )
  {
    if ( !signal ) return false;

    SIMPLELOGGER_DEBUG( LOGGER_NAME, "remove_signal_proxy with match rule " << signal->match_rule() );

    this->remove_match( signal->match_rule() );

    bool removed = false;

    {
        std::unique_lock<std::mutex> lock( m_priv->m_proxySignalsLock );
        std::vector<std::shared_ptr<signal_proxy_base>>::iterator it =
                std::find( m_priv->m_proxySignals.begin(), m_priv->m_proxySignals.end(), signal );
        if( it != m_priv->m_proxySignals.end() ){
            m_priv->m_proxySignals.erase( it );
            removed = true;
        }

        it = std::find( m_priv->m_allProxySignals.begin(), m_priv->m_allProxySignals.end(), signal );
        if( it != m_priv->m_allProxySignals.end() ){
            m_priv->m_allProxySignals.erase( it );
        }
    }

    {
        std::unique_lock<std::mutex> lock( m_priv->m_threadDispatcherLock );
        for( auto const& x : m_priv->m_threadDispatchers ){
            std::shared_ptr<ThreadDispatcher> disp = x.second.lock();
            if( disp && disp->remove_signal_proxy( signal ) ){
                removed = true;
            }
        }
    }

    return removed;
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

  const std::vector<std::shared_ptr<signal_proxy_base>>& Connection::get_signal_proxies()
  {
    return m_priv->m_proxySignals;
  }

  std::vector<std::shared_ptr<signal_proxy_base>> Connection::get_signal_proxies(const std::string & interface_name)
  {
    std::vector<std::shared_ptr<signal_proxy_base>> ret;

    for( std::shared_ptr<signal_proxy_base> base : m_priv->m_allProxySignals ){
        if( base->interface_name().compare( interface_name ) == 0 ){
            ret.push_back( base );
        }
    }

    return ret;
  }

  std::vector<std::shared_ptr<signal_proxy_base>> Connection::get_signal_proxies(const std::string & interface_name, const std::string & member)
  {
    std::vector<std::shared_ptr<signal_proxy_base>> ret;

    for( std::shared_ptr<signal_proxy_base> base : m_priv->m_allProxySignals ){
        if( base->interface_name().compare( interface_name ) == 0 &&
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

  std::string Connection::introspect(const std::string& destination, const std::string& path)
  {
    std::string failed;
    std::ostringstream sout;
    sout << "Introspection of Destination: " << destination << "   Path: " << path << " failed";
    
    failed = sout.str();
    
    if ( destination.empty() || path.empty() ) return failed;
    
    std::shared_ptr<CallMessage> msg = CallMessage::create( destination.c_str(), path.c_str(), DBUSCXX_INTERFACE_INTROSPECTABLE, "Introspect" );
    
    std::shared_ptr<Message> retmsg;

    retmsg = this->send_with_reply_blocking( msg );

    if (!retmsg) return failed;
    
    std::string retval;
    retmsg >> retval;
    return retval;
  }

  void Connection::set_dispatching_thread( std::thread::id tid ){
      m_priv->m_dispatchingThread = tid;
  }

  void Connection::notify_dispatcher_or_dispatch(){
      m_priv->m_dispatchStatus = DispatchStatus::DATA_REMAINS;

      if( std::this_thread::get_id() == m_priv->m_dispatchingThread ){
          dispatch();
      }else{
          m_priv->m_needsDispatching();
      }
  }

  void Connection::add_thread_dispatcher( std::weak_ptr<ThreadDispatcher> disp ){
    std::unique_lock<std::mutex> lock( m_priv->m_threadDispatcherLock );
    m_priv->m_threadDispatchers[ std::this_thread::get_id() ] = disp;
  }

  void Connection::remove_invalid_threaddispatchers_and_associated_objects(){
      std::vector<std::thread::id> invalidThreadIds;

      {
          std::unique_lock<std::mutex> lock( m_priv->m_threadDispatcherLock );
          for( auto it = m_priv->m_threadDispatchers.begin();
               it != m_priv->m_threadDispatchers.end(); ){
              if( it->second.expired() ){
                  invalidThreadIds.push_back( it->first );
                  it = m_priv->m_threadDispatchers.erase( it );
              }else{
                  it++;
              }
          }
      }

      if( invalidThreadIds.empty() ) return;

      {
          std::unique_lock<std::mutex> lock( m_priv->m_pathHandlerLock );
          for( auto it = m_priv->m_path_handler.begin(); it != m_priv->m_path_handler.end(); ){
              if( std::find( invalidThreadIds.begin(), invalidThreadIds.end(), it->second.handlingThread ) != invalidThreadIds.end() ){
                  it = m_priv->m_path_handler.erase( it );
              }else{
                  it++;
              }
          }
      }
  }

}


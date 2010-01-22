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
#include "utility.h"
#include "message.h"

#include "connection.h"
#include "callmessage.h"

#include <sstream>

namespace DBus
{
  
  /** mutex to lock when initializing */
  pthread_mutex_t init_mutex = PTHREAD_MUTEX_INITIALIZER;

  bool initialized_var = false;



  void init(bool threadsafe)
  {
    int retval;
    dbus_bool_t result;

    if ( threadsafe )
    {
      retval = pthread_mutex_lock( & init_mutex );
      if ( retval ) throw ErrorDeadlock::create();
    }
    
    if ( not initialized_var ) {
            
      if ( threadsafe ) dbus_threads_init_default();

      result = dbus_connection_allocate_data_slot( & Connection::m_weak_pointer_slot );
      if ( not result ) throw (-1); // TODO throw something better

      initialized_var = true;
    }

    if ( threadsafe )
    {
      retval = pthread_mutex_unlock( & init_mutex );
      if ( retval ) throw ErrorNotOwner::create();
    }
  
  }
  
  bool initialized()
  {
    return initialized_var;
  }
  
  
  std::string introspect(Connection::pointer conn, const std::string& destination, const std::string& path)
  {
    std::string failed;
    std::ostringstream sout;
    sout << "Introspection of Destination: " << destination << "   Path: " << path << " failed";
    
    failed = sout.str();
    
    if ( not conn or destination.empty() or path.empty() ) return failed;
    
    CallMessage::pointer msg = CallMessage::create( destination.c_str(), path.c_str(), DBUS_INTERFACE_INTROSPECTABLE, "Introspect" );
    
    Message::pointer retmsg;
    PendingCall::pointer pending;

    pending = conn->send_with_reply_async(msg);
    conn->flush();
    pending->block();
    retmsg = pending->steal_reply();

//     retmsg = conn->send_with_reply_blocking( msg );

    if (not retmsg) return failed;
    
    std::string retval;
    retmsg >> retval;
    return retval;
  }

  Type checked_type_cast(int n)
  {
    return (Type)(n);
  }

}




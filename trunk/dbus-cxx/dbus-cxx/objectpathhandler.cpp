/***************************************************************************
 *   Copyright (C) 2009 by Rick L. Vinyard, Jr.                            *
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
#include "objectpathhandler.h"
#include "connection.h"
#include "dbus-cxx-config.h"

namespace DBus
{
  struct DBusObjectPathVTable ObjectPathHandler::m_dbus_vtable = {
    path_unregister_callback,
    message_handler_callback
  };
  
  ObjectPathHandler::ObjectPathHandler(const std::string& path, PrimaryFallback pf):
      m_path(path),
      m_primary_fallback(pf)
  {
  }

  ObjectPathHandler::~ObjectPathHandler()
  {
  }

  ObjectPathHandler::pointer ObjectPathHandler::create(const std::string& path, PrimaryFallback pf)
  {
    if ( path.empty() ) return pointer();
    return pointer( new ObjectPathHandler(path, pf) );
  }

  const Path& ObjectPathHandler::path() const
  {
    return m_path;
  }

  PrimaryFallback ObjectPathHandler::is_primary_or_fallback()
  {
    return m_primary_fallback;
  }

  DBusCxxPointer< Connection > ObjectPathHandler::connection() const
  {
    return m_connection;
  }

  bool ObjectPathHandler::register_with_connection(Connection::pointer conn)
  {
    dbus_bool_t result;
    Error::pointer error = Error::create();

    DBUS_CXX_DEBUG("ObjectPathHandler::register_with_connection");

    if ( not conn or not conn->is_valid() ) return false;

    if ( m_connection )
    {
      this->unregister( conn );
    }
    
#ifdef DBUS_CXX_HAVE_DBUS_12
    if ( m_primary_fallback == PRIMARY )
      result = dbus_connection_try_register_object_path( conn->cobj(), m_path.c_str(), &m_dbus_vtable, this, error->cobj() );
    else
      result = dbus_connection_try_register_fallback( conn->cobj(), m_path.c_str(), &m_dbus_vtable, this, error->cobj() );
    if ( error->is_set() ) return false;
#else
    if ( m_primary_fallback == PRIMARY )
      result = dbus_connection_register_object_path( conn->cobj(), m_path.c_str(), &m_dbus_vtable, this );
    else
      result = dbus_connection_register_fallback( conn->cobj(), m_path.c_str(), &m_dbus_vtable, this );
#endif
    
    if ( not result ) return false;

    m_connection = conn;
    
    return true;
  }

  bool ObjectPathHandler::unregister(Connection::pointer conn)
  {
    dbus_bool_t result;
    if ( not conn or not conn->is_valid() ) return false;
    result = dbus_connection_unregister_object_path( conn->cobj(), m_path.c_str() );
    if ( result ) m_connection.reset();
    return result;
  }

  sigc::signal< void, Connection::pointer > & ObjectPathHandler::signal_registered()
  {
    return m_signal_registered;
  }

  sigc::signal< void, Connection::pointer > & ObjectPathHandler::signal_unregistered()
  {
    return m_signal_unregistered;
  }

  DBusHandlerResult ObjectPathHandler::message_handler_callback(DBusConnection * connection, DBusMessage * message, void * user_data)
  {
    bool result;
    if ( user_data == NULL ) return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    ObjectPathHandler* handler = static_cast<ObjectPathHandler*>(user_data);
    result = handler->handle_message(Connection::self(connection), Message::create(message));
    DBUS_CXX_DEBUG("ObjectPathHandler::message_handler_callback: result = " << result );
    if ( result == HANDLED ) return DBUS_HANDLER_RESULT_HANDLED;
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  }

  void ObjectPathHandler::path_unregister_callback(DBusConnection * connection, void * user_data)
  {
    if ( user_data == NULL ) return;
    ObjectPathHandler* handler = static_cast<ObjectPathHandler*>(user_data);
    handler->m_signal_unregistered.emit(Connection::self(connection));
  }

}


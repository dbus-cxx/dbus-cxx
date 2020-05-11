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
#include "dbus-cxx-private.h"
#include "error.h"
#include "message.h"
namespace sigc { template <typename T_return, typename ...T_arg> class signal; }

namespace DBus
{
  
  ObjectPathHandler::ObjectPathHandler(const std::string& path, PrimaryFallback pf):
      m_path(path),
      m_primary_fallback(pf)
  {
  }

  ObjectPathHandler::~ObjectPathHandler()
  {
  }

  const Path& ObjectPathHandler::path() const
  {
    return m_path;
  }

  PrimaryFallback ObjectPathHandler::is_primary_or_fallback()
  {
    return m_primary_fallback;
  }

  std::weak_ptr< Connection > ObjectPathHandler::connection() const
  {
    return m_connection;
  }

  bool ObjectPathHandler::register_with_connection(std::shared_ptr<Connection> conn)
  {
    dbus_bool_t result;
    Error error = Error();

    SIMPLELOGGER_DEBUG("dbus.ObjectPathHandler","Registering path " << m_path << " with connection");

    if ( not conn or not conn->is_valid() ) return false;

//    if ( m_connection )
//    {
//      //this->unregister( conn );
//    }
    
#ifdef DBUS_CXX_HAVE_DBUS_12
//    if ( m_primary_fallback == PrimaryFallback::PRIMARY )
//      result = dbus_connection_try_register_object_path( conn->cobj(), m_path.c_str(), &m_dbus_vtable, this, error.cobj() );
//    else
//      result = dbus_connection_try_register_fallback( conn->cobj(), m_path.c_str(), &m_dbus_vtable, this, error.cobj() );
//    if ( error.is_set() ) return false;
#else
    if ( m_primary_fallback == PrimaryFallback::PRIMARY )
      result = dbus_connection_register_object_path( conn->cobj(), m_path.c_str(), &m_dbus_vtable, this );
    else
      result = dbus_connection_register_fallback( conn->cobj(), m_path.c_str(), &m_dbus_vtable, this );
#endif
    
    if ( not result ) return false;

    m_connection = conn;
    
    return true;
  }

  bool ObjectPathHandler::unregister()
  {
      std::shared_ptr connection = m_connection.lock();
    if( !connection ) return true;
    return connection->unregister_object( m_path );
  }

  sigc::signal< void(std::shared_ptr<Connection>)> & ObjectPathHandler::signal_registered()
  {
    return m_signal_registered;
  }

  sigc::signal< void(std::shared_ptr<Connection>)> & ObjectPathHandler::signal_unregistered()
  {
    return m_signal_unregistered;
  }

  void ObjectPathHandler::set_connection(std::shared_ptr<Connection> conn){
      unregister();
      m_connection = conn;
  }

}


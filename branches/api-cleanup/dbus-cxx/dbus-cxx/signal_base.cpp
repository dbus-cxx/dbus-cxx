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

#include "signal_base.h"

#include "signalmessage.h"
#include "connection.h"

namespace DBus
{

  signal_base::signal_base(const std::string& path, const std::string& interface, const std::string& name):
      m_path(path),
      m_interface(interface),
      m_name(name)
  {
  }

  signal_base::signal_base(const std::string& interface, const std::string& name):
      m_interface(interface),
      m_name(name)
  {
  }

  signal_base::signal_base(Connection::pointer connection, const std::string& path, const std::string& interface, const std::string& name):
      m_connection(connection),
      m_path(path),
      m_interface(interface),
      m_name(name)
  {
  }

  signal_base::signal_base(Connection::pointer connection, const std::string& interface, const std::string& name):
      m_connection(connection),
      m_interface(interface),
      m_name(name)
  {
  }

  signal_base::signal_base(const signal_base& other):
      m_path(other.m_path),
      m_interface(other.m_interface),
      m_name(other.m_name),
      m_destination(other.m_destination)
  {
    // TODO connect to the other's connection
  }

  signal_base::~signal_base()
  {
  }

  DBusCxxPointer< Connection > signal_base::connection()
  {
    return m_connection.lock();
  }

  void DBus::signal_base::set_connection(DBusCxxPointer< Connection > connection)
  {
    m_connection = connection;
  }

  const std::string & signal_base::sender() const
  {
    return m_sender;
  }

  void signal_base::set_sender(const std::string & s)
  {
    m_sender = s;
  }

  const std::string & signal_base::interface() const
  {
    return m_interface;
  }

  void signal_base::set_interface( const std::string& i )
  {
    m_interface = i;
  }

  const std::string & signal_base::name() const
  {
    return m_name;
  }

  void signal_base::set_name( const std::string& n )
  {
    m_name = n;
  }

  const Path& signal_base::path() const
  {
    return m_path;
  }

  void signal_base::set_path(const std::string & s)
  {
    m_path = s;
  }

  const std::string & signal_base::destination() const
  {
    return m_destination;
  }

  void signal_base::set_destination(const std::string & s)
  {
    m_destination = s;
  }

  bool signal_base::handle_dbus_outgoing(Message::const_pointer msg)
  {
    Connection::pointer conn = m_connection.lock();
    if ( not conn or not conn->is_valid() ) return false;
    conn << msg;
    return true;
  }



}


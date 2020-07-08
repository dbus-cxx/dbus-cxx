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
#include "connection.h"
#include "path.h"

namespace DBus
{
  class Message;

  class signal_base::priv_data{
  public:
      priv_data(){}

      std::weak_ptr<Connection> m_connection;
      std::string m_sender;
      Path m_path;
      std::string m_interface;
      std::string m_name;
      std::string m_destination;
      std::string m_match_rule;
  };

  signal_base::signal_base(const std::string& path, const std::string& interface_name, const std::string& name):
      m_priv( std::make_unique<priv_data>() )
  {
      m_priv->m_path = path;
      m_priv->m_interface = interface_name;
      m_priv->m_name = name;
  }

  signal_base::signal_base(const std::string& interface_name, const std::string& name):
      m_priv( std::make_unique<priv_data>() )
  {
      m_priv->m_interface = interface_name;
      m_priv->m_name = name;
  }

  signal_base::signal_base(std::shared_ptr<Connection> connection, const std::string& path, const std::string& interface_name, const std::string& name):
      m_priv( std::make_unique<priv_data>() )
  {
      m_priv->m_connection = connection;
      m_priv->m_path = path;
      m_priv->m_interface = interface_name;
      m_priv->m_name = name;
  }

  signal_base::signal_base(std::shared_ptr<Connection> connection, const std::string& interface_name, const std::string& name):
      m_priv( std::make_unique<priv_data>() )
  {
      m_priv->m_connection = connection;
      m_priv->m_interface = interface_name;
      m_priv->m_name = name;
  }

  signal_base::~signal_base()
  {
  }

  std::shared_ptr< Connection > signal_base::connection()
  {
    return m_priv->m_connection.lock();
  }

  void DBus::signal_base::set_connection(std::shared_ptr< Connection > connection)
  {
    m_priv->m_connection = connection;
  }

  const std::string & signal_base::sender() const
  {
    return m_priv->m_sender;
  }

  void signal_base::set_sender(const std::string & s)
  {
    m_priv->m_sender = s;
  }

  const std::string & signal_base::interface_name() const
  {
    return m_priv->m_interface;
  }

  void signal_base::set_interface( const std::string& i )
  {
    m_priv->m_interface = i;
  }

  const std::string & signal_base::name() const
  {
    return m_priv->m_name;
  }

  void signal_base::set_name( const std::string& n )
  {
    m_priv->m_name = n;
  }

  const Path& signal_base::path() const
  {
    return m_priv->m_path;
  }

  void signal_base::set_path(const std::string & s)
  {
    m_priv->m_path = s;
  }

  const std::string & signal_base::destination() const
  {
    return m_priv->m_destination;
  }

  void signal_base::set_destination(const std::string & s)
  {
    m_priv->m_destination = s;
  }

  bool signal_base::handle_dbus_outgoing(std::shared_ptr<const Message> msg)
  {
    std::shared_ptr<Connection> conn = m_priv->m_connection.lock();
    if ( not conn or not conn->is_valid() ) return false;
    conn << msg;
    return true;
  }



}


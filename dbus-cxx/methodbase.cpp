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

#include "methodbase.h"
#include "dbus-cxx-private.h"
#include "dbus-cxx/connection.h"

namespace DBus
{

  MethodBase::MethodBase(const std::string& name):
      m_name(name)
  {
    SIMPLELOGGER_DEBUG( "dbus.MethodBase", "Creating new method with name " << name );
  }

  MethodBase::MethodBase(const MethodBase& other):
      m_name(other.m_name)
  {
  }

  MethodBase::~MethodBase(){}

  const std::string & MethodBase::name() const
  {
    return m_name;
  }

  void MethodBase::set_name(const std::string & name)
  {
    std::string old_name;
    {
      std::lock_guard<std::mutex> lock( m_name_mutex );
      old_name = m_name;
      m_name = name;
    }
    m_signal_name_changed.emit(old_name, m_name);
  }

  sigc::signal< void( const std::string &, const std::string &) > MethodBase::signal_name_changed()
  {
    return m_signal_name_changed;
  }

  void MethodBase::set_arg_name(size_t i, const std::string& name){
      if( m_arg_names.size() < i ){
          m_arg_names.resize( i );
      }
      m_arg_names[i] = name;
  }

  std::string MethodBase::arg_name(size_t i) const { 
      if( m_arg_names.size() < i && i >= 0 ){
          return m_arg_names[i];
      }
      return std::string();
  }

  uint32_t MethodBase::sendMessage( std::shared_ptr<Connection> connection, const std::shared_ptr<const Message> message ){
      return connection->send( message );
  }

}


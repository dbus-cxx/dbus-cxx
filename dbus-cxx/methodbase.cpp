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

namespace DBus
{

  MethodBase::MethodBase(const std::string& name):
      m_name(name)
  {
    pthread_mutex_init( &m_name_mutex, NULL );
  }

  MethodBase::MethodBase(const MethodBase& other):
      m_name(other.m_name)
  {
    pthread_mutex_init( &m_name_mutex, NULL );
  }

  MethodBase::~MethodBase()
  {
    pthread_mutex_destroy( &m_name_mutex );
  }

  const std::string & MethodBase::name() const
  {
    return m_name;
  }

  void MethodBase::set_name(const std::string & name)
  {
    pthread_mutex_lock( &m_name_mutex );
    std::string old_name = m_name;
    m_name = name;
    pthread_mutex_unlock( &m_name_mutex );
    m_signal_name_changed.emit(old_name, m_name);
  }

  sigc::signal< void, const std::string &, const std::string & > MethodBase::signal_name_changed()
  {
    return m_signal_name_changed;
  }

}


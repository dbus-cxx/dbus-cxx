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
#include "error.h"
#include "message.h"

namespace DBus
{

  Error::Error()
  {
    dbus_error_init( &m_cobj );
  }

  Error::Error( DBusError * cobj )
  {
    dbus_error_init( &m_cobj );
    dbus_move_error( cobj, &m_cobj );
  }

  Error::Error( const char* name, const char* message )
  {
    dbus_error_init( &m_cobj );
    this->set( name, message );
  }

  Error::Error( Message& m )
  {
    dbus_set_error_from_message( &m_cobj, m.cobj() );
  }

  Error::pointer Error::create()
  {
    return pointer( new Error() );
  }

  Error::pointer Error::create( DBusError* cobj )
  {
    return pointer( new Error( cobj ) );
  }

  Error::pointer Error::create( const char* name, const char* message )
  {
    return pointer( new Error( name, message ) );
  }

  Error::pointer Error::create( Message& message )
  {
     return pointer( new Error( message ) );
  }

  Error::~Error( ) throw()
  {
    dbus_error_free( &m_cobj );
  }

  const char* Error::name() const
  {
    return m_cobj.name;
  }

  const char* Error::message() const
  {
    return m_cobj.message;
  }

  bool Error::is_set() const
  {
    return dbus_error_is_set( &m_cobj );
  }

  Error::operator bool() const
  {
    return this->is_set();
  }

//   Error& Error::operator=( Error& other )
//   {
//     dbus_move_error( &m_cobj, &( other.m_cobj ) );
//     return *this;
//   }

  void Error::set( const char* name, const char* message )
  {
    dbus_set_error_const( &m_cobj, name, message );
  }

  const char* Error::what() const throw()
  {
    return m_cobj.message;
  }

  DBusError* Error::cobj( )
  {
    return & m_cobj;
  }

}



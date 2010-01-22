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
#include "watch.h"
#include "error.h"
#include "enums.h"

namespace DBus
{

  Watch::Watch( DBusWatch* cobj ): m_cobj( cobj )
  {
    if ( m_cobj ) dbus_watch_set_data( cobj, this, NULL );
  }

  Watch::pointer Watch::create(DBusWatch * cobj)
  {
    return pointer( new Watch(cobj) );
  }
    
  Watch::~Watch()
  {
  }

  bool Watch::is_valid() const
  {
    return m_cobj != NULL;
  }

  Watch::operator bool() const
  {
    return this->is_valid();
  }

  int Watch::unix_fd( ) const
  {
    if ( not this->is_valid() ) throw ErrorInvalidCObject::create();
    return dbus_watch_get_unix_fd( m_cobj );
  }

  int Watch::socket( ) const
  {
    if ( not this->is_valid() ) throw ErrorInvalidCObject::create();
    return dbus_watch_get_socket( m_cobj );
  }

  unsigned int Watch::flags( ) const
  {
    if ( not this->is_valid() ) return WATCH_ERROR;
    return dbus_watch_get_flags( m_cobj );
  }

  bool Watch::is_readable() const
  {
    return this->flags() & WATCH_READABLE;
  }

  bool Watch::is_writable() const
  {
    return this->flags() & WATCH_WRITABLE;
  }

  bool Watch::is_enabled( ) const
  {
    if ( not this->is_valid() ) throw ErrorInvalidCObject::create();
    return dbus_watch_get_enabled( m_cobj );
  }

  bool Watch::handle( unsigned int flags )
  {
    if ( not this->is_valid() ) throw ErrorInvalidCObject::create();
    return dbus_watch_handle( m_cobj, flags );
  }

  bool Watch::handle_read(bool error, bool hangup)
  {
    int flags = WATCH_READABLE;
    if ( error ) flags |= WATCH_ERROR;
    if ( hangup ) flags |= WATCH_HANGUP;
    return handle(flags);
  }

  bool Watch::handle_write(bool error, bool hangup)
  {
    int flags = WATCH_WRITABLE;
    if ( error ) flags |= WATCH_ERROR;
    if ( hangup ) flags |= WATCH_HANGUP;
    return handle(flags);
  }

  bool Watch::handle_readwrite(bool error, bool hangup)
  {
    int flags = WATCH_READABLE | WATCH_WRITABLE;
    if ( error ) flags |= WATCH_ERROR;
    if ( hangup ) flags |= WATCH_HANGUP;
    return handle(flags);
  }

  DBusWatch * Watch::cobj( )
  {
    return m_cobj;
  }

  const DBusWatch * Watch::cobj( ) const
  {
    return m_cobj;
  }

  Watch::operator DBusWatch*()
  {
    return this->cobj();
  }

}


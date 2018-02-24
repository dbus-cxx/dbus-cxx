/***************************************************************************
 *   Copyright (C) 2007,2008,2009,2010 by Rick L. Vinyard, Jr.             *
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
#include <dbus/dbus.h>
#include <dbus-cxx/pointer.h>

#ifndef DBUSCXX_WATCH_H
#define DBUSCXX_WATCH_H

namespace DBus
{

  /**
   * Provides file descriptor watches
   *
   * Typically used by connections as requested by the underlying
   * dbus library.
   *
   * @ingroup core
   *
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class Watch
  {
    protected:
      Watch( DBusWatch* cobj = NULL );
      
    public:
      
      typedef DBusCxxPointer<Watch> pointer;
      typedef DBusCxxWeakPointer<Watch> weak_pointer;
      
      static pointer create( DBusWatch* cobj = NULL );
      
      ~Watch();

      bool is_valid() const;

      operator bool() const;

      int unix_fd() const;

      int socket() const;

      unsigned int flags() const;

      bool is_readable() const;

      bool is_writable() const;

      bool is_enabled() const;

      bool handle( unsigned int flags );

      bool handle_read( bool error=false, bool hangup=false );
      
      bool handle_write( bool error=false, bool hangup=false );

      bool handle_readwrite( bool error = false, bool hangup = false );

      DBusWatch* cobj();
      
      const DBusWatch* cobj() const;

      operator DBusWatch*();

    protected:
      DBusWatch* m_cobj;
  };

}

#endif

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
#include <sys/time.h>
#include <sys/signal.h>
#include <dbus-cxx/pointer.h>
#include <mutex>

#ifndef DBUSCXX_TIMEOUT_H
#define DBUSCXX_TIMEOUT_H

namespace DBus
{

  /**
   * Provides timeouts that can be armed/disarmed as necessary
   *
   * Typically used by connections as requested by the underlying
   * dbus library.
   * 
   * @ingroup core
   *
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class Timeout
  {
    protected:
      Timeout( DBusTimeout* cobj=NULL );

    public:
      
      typedef DBusCxxPointer<Timeout> pointer;
      typedef DBusCxxWeakPointer<Timeout> weak_pointer;
      
      static pointer create( DBusTimeout* cobj=NULL );
      
      ~Timeout();

      bool is_valid() const;

      operator bool() const;

      int interval() const;

      bool is_enabled() const;

      bool handle();

      bool operator==(const Timeout& other) const;

      bool operator!=(const Timeout& other) const;

      void arm(bool should_arm=true);

      bool is_armed();

      DBusTimeout* cobj();

      operator DBusTimeout*();

    protected:
      DBusTimeout* m_cobj;

      timer_t m_timer_id;

      bool m_is_armed;

      std::mutex m_arming_mutex;

      static void timer_callback_proxy( sigval_t sv );

  };

}

#endif

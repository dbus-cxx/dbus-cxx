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
#include <sigc++/sigc++.h>
#include <dbus-cxx/message.h>
#include <dbus-cxx/pointer.h>

#ifndef DBUSCXX_PENDING_CALL_H
#define DBUSCXX_PENDING_CALL_H

namespace DBus
{

  /**
   * Monitors an asynchronous call, emitting a signal when a response is received
   *
   * @ingroup message
   *
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class PendingCall: public sigc::trackable
  {
    protected:

      PendingCall( DBusPendingCall* cobj = NULL );

      PendingCall( const PendingCall& );

    public:

      typedef DBusCxxPointer<PendingCall> pointer;

      static pointer create( DBusPendingCall* cobj = NULL );

      static pointer create( const PendingCall& );

      virtual ~PendingCall();

      PendingCall& operator=( const PendingCall& other );

      void cancel();

      bool completed();

      Message::pointer steal_reply();

      void block();

      sigc::signal<void> signal_notify();

      DBusPendingCall* cobj();

    private:

      DBusPendingCall* m_cobj;

      sigc::signal<void> m_signal_notify;

      static void notify_callback( DBusPendingCall* pending, void* data );
  };

}

#endif

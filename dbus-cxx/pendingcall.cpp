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
#include "pendingcall.h"
#include "error.h"
#include "message.h"
#include <sigc++/sigc++.h>

namespace DBus
{


  PendingCall::PendingCall() :
      m_canceled( false )
  {
  }

  std::shared_ptr<PendingCall> PendingCall::create()
  {
    return std::shared_ptr<PendingCall>( new PendingCall() );
  }

  PendingCall::~PendingCall()
  {
  }

  void PendingCall::cancel()
  {
      m_canceled = true;
  }

  bool PendingCall::completed() const
  {
    return m_reply->is_valid();
  }

  std::shared_ptr<Message> PendingCall::reply() const
  {
    return m_reply;
  }

  void PendingCall::block() const
  {
      if( m_canceled ){
          return;
      }
  }

  sigc::signal<void()> PendingCall::signal_notify() const
  {
    return m_signal_notify;
  }

  void PendingCall::set_reply( std::shared_ptr<Message> msg ){
      m_reply = msg;
      m_signal_notify.emit();
  }

}

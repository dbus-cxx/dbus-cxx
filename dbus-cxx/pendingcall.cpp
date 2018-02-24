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
#include "utility.h"

namespace DBus
{

  PendingCall::PendingCall( DBusPendingCall* cobj )
      : m_cobj( cobj )
  {
    if ( m_cobj && !dbus_pending_call_set_notify( m_cobj, PendingCall::notify_callback, this, NULL ) ) {
      throw ErrorNoMemory::create( "Unable to initialize pending call" );
    }
  }

  PendingCall::PendingCall( const PendingCall& c ) :
      m_cobj( c.m_cobj ),
      m_signal_notify( c.m_signal_notify )
  {
    if ( m_cobj )
      dbus_pending_call_ref( m_cobj );
  }

  PendingCall::pointer PendingCall::create( DBusPendingCall * cobj )
  {
    return pointer( new PendingCall( cobj ) );
  }

  PendingCall::pointer PendingCall::create( const PendingCall & other )
  {
    return pointer( new PendingCall( other ) );
  }

  PendingCall::~PendingCall()
  {
    if ( m_cobj ){
      //At this point, there are no more references to this PendingCall - which means that when 
      //we get a callback, we will(most likely) segfault because the memory has been deallocated.
      //Unref the pending call and then cancel it.
      //dbus_pending_call_unref( m_cobj );
      dbus_pending_call_cancel( m_cobj );
    }
  }

  PendingCall& PendingCall::operator=( const PendingCall& other )
  {
    if ( m_cobj )
      dbus_pending_call_unref( m_cobj );
    m_cobj = other.m_cobj;
    if ( m_cobj )
      dbus_pending_call_ref( m_cobj );
    return *this;
  }

  void PendingCall::cancel()
  {
    if ( m_cobj )
      dbus_pending_call_cancel( m_cobj );
  }

  bool PendingCall::completed()
  {
    if ( m_cobj )
      return dbus_pending_call_get_completed( m_cobj );
    return true;
  }

  Message::pointer PendingCall::steal_reply()
  {
    if ( m_cobj ) return Message::create( dbus_pending_call_steal_reply( m_cobj ) );
    return Message::pointer();
  }

  void PendingCall::block()
  {
    if ( m_cobj )
      dbus_pending_call_block( m_cobj );
  }

  sigc::signal<void> PendingCall::signal_notify()
  {
    return m_signal_notify;
  }

  void PendingCall::notify_callback( DBusPendingCall* dpc, void* data )
  {
    PendingCall * pc = static_cast<PendingCall*>( data );

    pc->m_signal_notify.emit();
  }

}

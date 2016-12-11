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
#include "timeout.h"
#include "error.h"
#include "utility.h"

namespace DBus
{

  Timeout::Timeout( DBusTimeout* cobj ):
      m_cobj( cobj ),
      m_is_armed(false)
  {
    if ( m_cobj ) dbus_timeout_set_data( cobj, this, NULL );
    pthread_mutex_init( &m_arming_mutex, NULL );
  }

  Timeout::pointer Timeout::create(DBusTimeout * cobj)
  {
    return pointer( new Timeout(cobj) );
  }
    
  Timeout::~Timeout()
  {
    pthread_mutex_lock( &m_arming_mutex );
    if ( m_is_armed ) timer_delete( m_timer_id );
    pthread_mutex_unlock( &m_arming_mutex );
    pthread_mutex_destroy( &m_arming_mutex );
  }

  bool Timeout::is_valid() const
  {
    return m_cobj != NULL;
  }

  Timeout::operator bool() const
  {
    return this->is_valid();
  }

  int Timeout::interval( ) const
  {
    if ( not this->is_valid() ) throw ErrorInvalidCObject::create();
    return dbus_timeout_get_interval( m_cobj );
  }

  bool Timeout::is_enabled( ) const
  {
    if ( not this->is_valid() ) throw ErrorInvalidCObject::create();
    return dbus_timeout_get_enabled( m_cobj );
  }

  bool Timeout::handle( )
  {
    if ( not this->is_valid() ) throw ErrorInvalidCObject::create();
    return dbus_timeout_handle( m_cobj );
  }

  bool Timeout::operator ==(const Timeout & other) const
  {
    return m_cobj == other.m_cobj;
  }

  bool Timeout::operator !=(const Timeout & other) const
  {
    return m_cobj != other.m_cobj;
  }

  void Timeout::arm(bool should_arm)
  {
    pthread_mutex_lock( &m_arming_mutex );
    if ( should_arm )
    {
      if ( not m_is_armed )
      {
        struct sigevent sigevent = {{0},0};

        sigevent.sigev_notify = SIGEV_THREAD;
        sigevent.sigev_value.sival_ptr = this;
        sigevent.sigev_notify_function = Timeout::timer_callback_proxy;

        timer_create( CLOCK_REALTIME, &sigevent, &m_timer_id);
      }
      
      int intv = this->interval();
      time_t sec;
      long int nsec;
      sec = intv / 1000;
      nsec = (intv % 1000) * 1000000;
      struct itimerspec its = { {sec, nsec}, {sec, nsec} };

      timer_settime( &m_timer_id, 0, &its, NULL );

      m_is_armed = true;
    }
    else
    {
      if ( m_is_armed )
      {
          m_is_armed = false;
          timer_delete( m_timer_id );
      }
    }
    pthread_mutex_unlock( &m_arming_mutex );
  }

  bool Timeout::is_armed()
  {
    return m_is_armed;
  }

  DBusTimeout* Timeout::cobj( )
  {
    return m_cobj;
  }

  Timeout::operator DBusTimeout*()
  {
    return m_cobj;
  }

  void Timeout::timer_callback_proxy( sigval_t sv ) {
    DBUS_CXX_DEBUG( "Timeout::timer_callback_proxy" );
    Timeout* t;
    t = ( Timeout* ) sv.sival_ptr;

    if ( t != NULL and t->is_valid() ) t->handle();
  }

}




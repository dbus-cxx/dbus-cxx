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
#include "utility.h"
#include "error.h"

namespace DBus
{
  
  /** mutex to lock when initializing */
  pthread_mutex_t init_mutex = PTHREAD_MUTEX_INITIALIZER;

  bool initialized_var = false;

  dbus_int32_t connection_weak_pointer_slot = -1;


  void init(bool threadsafe)
  {
    int retval;
    dbus_bool_t result;

    if ( threadsafe )
    {
      retval = pthread_mutex_lock( & init_mutex );
      if ( retval ) throw ErrorDeadlock::create();
    }
    
    if ( not initialized_var ) {
            
      if ( threadsafe ) dbus_threads_init_default();

      result = dbus_connection_allocate_data_slot( & connection_weak_pointer_slot );
      if ( not result ) throw (-1); // TODO throw something better

      initialized_var = true;
    }

    if ( threadsafe )
    {
      retval = pthread_mutex_unlock( & init_mutex );
      if ( retval ) throw ErrorNotOwner::create();
    }
  
  }
  
  bool initialized()
  {
    return initialized_var;
  }

  DBusCxxPointer<Connection> connection(DBusConnection * c)
  {
    if ( c == NULL or connection_weak_pointer_slot == -1 ) return DBusCxxPointer<Connection>();
    
    void* v = dbus_connection_get_data(c, connection_weak_pointer_slot);

    if ( v == NULL ) return DBusCxxPointer<Connection>();

    DBusCxxWeakPointer<Connection>* wp = static_cast<DBusCxxWeakPointer<Connection>*>(v);

    DBusCxxPointer<Connection> p = wp->lock();

    return p;
  }

}




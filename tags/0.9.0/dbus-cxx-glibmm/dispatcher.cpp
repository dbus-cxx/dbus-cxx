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
#include "dispatcher.h"
#include <iostream>
#include <dbus/dbus.h>

#include <dbus-cxx/signalmessage.h>

#include <sys/select.h>
#include <errno.h>

namespace DBus
{
  namespace Glib {

    Dispatcher::Dispatcher(bool is_running):
        DBus::Dispatcher(false)
    {
      m_glibmm_dispatcher.connect( sigc::mem_fun(*this, &DBus::Glib::Dispatcher::on_glibmm_dispatch) );
      if ( is_running ) this->start();
    }
    
    Dispatcher::Dispatcher(bool is_running, const ::Glib::RefPtr< ::Glib::MainContext >& context):
        DBus::Dispatcher(false),
        m_glibmm_dispatcher(context)
    {
      m_glibmm_dispatcher.connect( sigc::mem_fun(*this, &DBus::Glib::Dispatcher::on_glibmm_dispatch) );
      if ( is_running ) this->start();
    }
    
    Dispatcher::pointer Dispatcher::create( bool is_running )
    {
      return pointer( new Dispatcher(is_running) );
    }

    Dispatcher::pointer Dispatcher::create(bool is_running, const ::Glib::RefPtr< ::Glib::MainContext > & context)
    {
      return pointer( new Dispatcher(is_running, context) );
    }

    Dispatcher::~Dispatcher()
    {
      if ( this->is_running() ) this->stop();
    }

    void Dispatcher::dispatch_thread_main()
    {
      struct pollfd thread_wakeup;
      std::vector<struct pollfd> fds;
      int selresult;

      thread_wakeup.fd = process_fd[ 1 ];
      thread_wakeup.events = POLLIN;
      
      while ( m_running ) {
        // wait forever until some file descriptor has events
        selresult = poll( fds.data(), fds.size(), -1 );

        m_glibmm_dispatcher.emit();
      }
    }

    void Dispatcher::on_glibmm_dispatch()
    {
      dispatch_connections();
    }
  
  }
}


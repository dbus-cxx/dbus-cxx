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
      pthread_cond_init( &m_cond_glibmm_processing, NULL );
      pthread_mutex_init( &m_mutex_glibmm_processing, NULL );
      m_glibmm_dispatcher.connect( sigc::mem_fun(*this, &DBus::Glib::Dispatcher::on_glibmm_dispatch) );
      if ( is_running ) this->start();
    }
    
    Dispatcher::Dispatcher(bool is_running, const ::Glib::RefPtr< ::Glib::MainContext >& context):
        DBus::Dispatcher(false),
        m_glibmm_dispatcher(context)
    {
      pthread_cond_init( &m_cond_glibmm_processing, NULL );
      pthread_mutex_init( &m_mutex_glibmm_processing, NULL );
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
      // Setting this guarantees that we will not stop on the condition
      m_initiate_processing = true;

      while ( m_running ) {

        // Here, we lock the mutex before checking m_initiate_processing
        // If something has changed m_initiate_processing there is no reason to wait on the condition
        // since we have some work to do
        pthread_mutex_lock( &m_mutex_initiate_processing );

        if ( not m_initiate_processing )
          // If we don't have any work to do we will wait here for someone to wake us up
          pthread_cond_wait( &m_cond_initiate_processing, &m_mutex_initiate_processing );

        // At this point we have one of two situations: either there is work for us to do, or we were
        // sleeping waiting for the condition to wake us up.
        // It's possible that we were woken up because we are shutting down processing.
        // If that's the case we need out of the while loop, but we also need to unlock the mutex
        if ( not m_running ) {
          pthread_mutex_unlock( &m_mutex_initiate_processing );
          break;
        }

        m_initiate_processing = false;
        pthread_mutex_unlock( &m_mutex_initiate_processing );

        // We will lock the glib processing mutex, emit the glibmm dispatch
        // signal and wait in the condition until the glibmm main loop
        // triggers the callback and performs the dispatching
        pthread_mutex_lock( &m_mutex_glibmm_processing );
        m_glibmm_dispatcher.emit();
        pthread_cond_wait( &m_cond_glibmm_processing, &m_mutex_glibmm_processing );

        // And, if we made it here we are unblocked and need to release the mutex
        pthread_mutex_unlock( &m_mutex_glibmm_processing );
      }
    }

    void Dispatcher::on_glibmm_dispatch()
    {
      Connections::iterator ci;
      unsigned int loop_count;

      for ( ci = m_connections.begin(); ci != m_connections.end(); ci++ ) {
          // If the dispatch loop limit is zero we will loop as long as status is DISPATCH_DATA_REMAINS
        if ( m_dispatch_loop_limit == 0 ) {
          DBUS_CXX_DEBUG( "Dispatch Status: " << ( *ci )->dispatch_status() << "   Hints: DISPATCH_DATA_REMAINS=" << DISPATCH_DATA_REMAINS );
          while (( *ci )->dispatch_status() == DISPATCH_DATA_REMAINS )
            ( *ci )->dispatch();
        }
          // Otherwise, we will only perform a number of dispatches up to the loop limit
        else {
          for ( loop_count = 0; loop_count < m_dispatch_loop_limit; loop_count++ ) {
              // Make sure we need to dispatch before calling it
            if (( *ci )->dispatch_status() != DISPATCH_COMPLETE )( *ci )->dispatch();

              // Are we done? If so, let's break out of the loop.
            if (( *ci )->dispatch_status() != DISPATCH_DATA_REMAINS ) break;
          }

            // If we still have more to process let's set the processing flag to true
          if (( *ci )->dispatch_status() != DISPATCH_DATA_REMAINS ) {
            pthread_mutex_lock( &m_mutex_initiate_processing );
            m_initiate_processing = true;
            pthread_mutex_unlock( &m_mutex_initiate_processing );
          }
        }

      }

      // We'll lock the glibmm mutex to ensure that there are no race
      // conditions with the dispatcher thread and it entering the
      // pthread condition
      pthread_mutex_lock( &m_mutex_glibmm_processing );

      // Let the DBus dispatch thread know that we are finished dispatching
      pthread_cond_broadcast( &m_cond_glibmm_processing );

      // Now that we have sent the broadcast we can unlock the mutex
      pthread_mutex_unlock( &m_mutex_glibmm_processing );

    }
  
  }
}


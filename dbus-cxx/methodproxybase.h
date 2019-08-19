/***************************************************************************
 *   Copyright (C) 2009,2010 by Rick L. Vinyard, Jr.                       *
 *   rvinyard@cs.nmsu.edu                                                  *
 *   Copyright (C) 2019 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
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
#include <string>

#include <sigc++/sigc++.h>

#include <dbus-cxx/enums.h>
#include <dbus-cxx/pointer.h>
#include <dbus-cxx/forward_decls.h>
#include <dbus-cxx/accumulators.h>
#include <dbus-cxx/callmessage.h>
#include <dbus-cxx/returnmessage.h>
#include <dbus-cxx/pendingcall.h>

#ifndef DBUSCXX_METHODPROXYBASE_H
#define DBUSCXX_METHODPROXYBASE_H

namespace DBus
{
  /**
   * @ingroup objects
   * @ingroup proxy
   * 
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class MethodProxyBase
  {
    protected:

      MethodProxyBase( const std::string& name );
      
      MethodProxyBase(const MethodProxyBase& other);

    public:

      typedef DBusCxxPointer<MethodProxyBase> pointer;

      static pointer create( const std::string& name );

      ~MethodProxyBase();

      InterfaceProxy* interface() const;

      const std::string& name() const;

      void set_name( const std::string& name );

      CallMessage::pointer create_call_message( ) const;

      ReturnMessage::const_pointer call( CallMessage::const_pointer, int timeout_milliseconds=-1 ) const;

      //TODO make a call() function that takes in a builder(for the milliseconds)
      template <class T_return,T_arg...>
      T_return call( T_arg... args )
      {
        CallMessage::pointer _callmsg = this->create_call_message();
        std::tuple<T_arg...> msg_args = std::tuple<T_arg...>(args...);
        //_callmsg.setArgs(msg_args)
        ReturnMessage::const_pointer retmsg = this->call( _callmsg, -1 );
        T_return _retval;
        retmsg >> _retval;
        return _retval;
      }

      template <void,T_arg...>
      void call( T_arg... args )
      {
        CallMessage::pointer _callmsg = this->create_call_message();
        std::tuple<T_arg...> msg_args = std::tuple<T_arg...>(args...);
        //_callmsg.setArgs(msg_args)
        this->call( _callmsg, -1 );
      }

      PendingCall::pointer call_async( CallMessage::const_pointer, int timeout_milliseconds=-1 ) const;

      sigc::signal<void,const std::string&/*old name*/, const std::string&/*new name*/> signal_name_changed();

    protected:

      // Declare InterfaceProxy as a friend so that it can set the interface
      friend class InterfaceProxy;

      InterfaceProxy* m_interface;

      std::string m_name;

      /** Ensures that the name doesn't change while the name changed signal is emitting */
      pthread_mutex_t m_name_mutex;

      sigc::signal<void,const std::string&, const std::string&> m_signal_name_changed;

  };

}

#endif

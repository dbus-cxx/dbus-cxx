dnl ***************************************************************************
dnl *   Copyright (C) 2009,2010 by Rick L. Vinyard, Jr.                       *
dnl *   rvinyard@cs.nmsu.edu                                                  *
dnl *                                                                         *
dnl *   This file is part of the dbus-cxx library.                            *
dnl *                                                                         *
dnl *   The dbus-cxx library is free software; you can redistribute it and/or *
dnl *   modify it under the terms of the GNU General Public License           *
dnl *   version 3 as published by the Free Software Foundation.               *
dnl *                                                                         *
dnl *   The dbus-cxx library is distributed in the hope that it will be       *
dnl *   useful, but WITHOUT ANY WARRANTY; without even the implied warranty   *
dnl *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   *
dnl *   General Public License for more details.                              *
dnl *                                                                         *
dnl *   You should have received a copy of the GNU General Public License     *
dnl *   along with this software. If not see <http://www.gnu.org/licenses/>.  *
dnl ***************************************************************************

divert(-1)

include(template.macros.m4)

define([BLOCKING_CALL],[dnl
      dnl
template <LIST(class T_return, LOOP(class T_arg%1, [$1]))>
      T_return call( LIST(LOOP(T_arg%1 _val_%1, $1), int _timeout_milliseconds = -1) )
      {
        CallMessage::pointer _callmsg = this->create_call_message();
        _callmsg FOR(1, $1,[ << _val_%1]);
        ReturnMessage::const_pointer retmsg = this->call( _callmsg, _timeout_milliseconds );
        T_return _retval;
        retmsg >> _retval;
        return _retval;
      }
      dnl
])
    
define([BLOCKING_CALL_VOID_RETURN],[dnl
      dnl
template <LIST(LOOP(class T_arg%1, [$1]))>
      void call<LIST(void, LOOP(T_arg%1, [$1]))>( LIST(LOOP(T_arg%1 _val_%1, $1), int _timeout_milliseconds = -1) )
      {
        CallMessage::pointer _callmsg = this->create_call_message();
        _callmsg FOR(1, $1,[ << _val_%1]);
        this->call( _callmsg, _timeout_milliseconds );
      }
      dnl
])
    


divert(0)
dnl
[/***************************************************************************
 *   Copyright (C) 2009,2010 by Rick L. Vinyard, Jr.                       *
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
 ***************************************************************************/]
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

dnl FOR(0, eval(CALL_SIZE),[[
dnl       BLOCKING_CALL(%1)
dnl           ]])
      
dnl FOR(0, eval(CALL_SIZE),[[
dnl       BLOCKING_CALL_VOID_RETURN(%1)
dnl           ]])
      
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

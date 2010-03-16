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

define([METHODPROXY],[dnl
ifelse($1, $2,[dnl

  /**
   * @ingroup objects
   * @ingroup proxy
   * 
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  template <LIST(class T_return, LOOP(class T_arg%1, $1))>],[dnl

  /**
   * @ingroup objects
   * @ingroup proxy
   * 
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  template <LIST(class T_return, LOOP(class T_arg%1, $1))>])
  class MethodProxy ifelse($1, $2,,[<LIST(T_return, LOOP(T_arg%1,$1), LOOP(nil, CALL_SIZE - $1))>])
    : public MethodProxyBase
  {
  public:

    typedef DBusCxxPointer<MethodProxy> pointer;

    MethodProxy(const std::string& name):
      MethodProxyBase(name)
    {  }

    T_return operator()(LIST(LOOP(T_arg%1 _val_%1, $1)))
    {
      DBUS_CXX_DEBUG("Method<LIST(T_return, LOOP(T_arg%1, $1))>::operator()   method=" << m_name );
      CallMessage::pointer _callmsg = this->create_call_message();
      ifelse(eval($1>0),1,[*_callmsg FOR(1, $1,[ << _val_%1]);],[])
      ReturnMessage::const_pointer retmsg = this->call( _callmsg );
      T_return _retval;
      retmsg >> _retval;
      return _retval;
    }

    static pointer create(const std::string& name)
    { return pointer( new MethodProxy(name) ); }

  };

])

define([METHODPROXY_VOID],[dnl
  /**
   */
  template <LIST(LOOP(class T_arg%1, $1))>
  class MethodProxy <LIST(void, LOOP(T_arg%1,$1), LOOP(nil, CALL_SIZE - $1))>
    : public MethodProxyBase
  {
  public:

    typedef DBusCxxPointer<MethodProxy> pointer;

    MethodProxy(const std::string& name):
      MethodProxyBase(name)
    {  }

    void operator()(LIST(LOOP(T_arg%1 _val_%1, $1)))
    {
      DBUS_CXX_DEBUG("Method<LIST(T_return, LOOP(T_arg%1, $1))>::operator()   method=" << m_name );
      CallMessage::pointer _callmsg = this->create_call_message();
      *_callmsg FOR(1, $1,[ << _val_%1]);
      _callmsg->set_no_reply();
      this->call( _callmsg );
    }

    static pointer create(const std::string& name)
    { return pointer( new MethodProxy(name) ); }

  };

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
#include <dbus-cxx/forward_decls.h>
#include <dbus-cxx/methodproxybase.h>
#include <dbus-cxx/utility.h>

#ifndef DBUSCXX_METHODPROXY_H
#define DBUSCXX_METHODPROXY_H

namespace DBus {

METHODPROXY(CALL_SIZE,CALL_SIZE)
FOR(0,eval(CALL_SIZE-1),[[METHODPROXY(%1)]])
FOR(0,eval(CALL_SIZE-1),[[METHODPROXY_VOID(%1)]])

}

#endif

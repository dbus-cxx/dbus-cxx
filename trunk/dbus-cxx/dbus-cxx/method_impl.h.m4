dnl ***************************************************************************
dnl *   Copyright (C) 2009 by Rick L. Vinyard, Jr.                            *
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

define([METHOD_VOID],[dnl
  /**
   */
  template <LIST(LOOP(class T_arg%1, $1))>
  class Method <LIST(void, LOOP(T_arg%1,$1), LOOP(nil, CALL_SIZE - $1))>
    : public MethodBase
  {
  public:

    typedef DBusCxxPointer<Method> pointer;

    Method(const std::string& name):
      MethodBase(name)
    {  }

    virtual HandlerResult handle_call_message( DBusCxxPointer<Connection> connection, CallMessage::const_pointer message )
    {
      DBUS_CXX_DEBUG("Method<LIST(void, LOOP(T_arg%1, $1))>::handle_call_message   method=" << m_name );
      
      if ( not connection or not message ) return NOT_HANDLED;

      dnl
      FOR(1, $1,[
      T_arg%1 _val_%1;])

      ifelse(eval($1>0),1,[
      try {
        Message::iterator i = message->begin();
        i FOR(1, $1,[ >> _val_%1]);
      }
      catch ( ErrorInvalidTypecast& e ) {
          return NOT_HANDLED;
      }
      ],[])

      m_slot(LIST(LOOP(_val_%1, $1)));

      ReturnMessage::const_pointer retmsg;

      if ( message->expects_reply() )
      {
        DBUS_CXX_DEBUG( "REPLY EXPECTED" );
        retmsg = message->create_reply();
        if ( not retmsg ) return NOT_HANDLED;
        connection << retmsg;
      }

      return HANDLED;
    }

    void set_method( sigc::slot$1<LIST(void, LOOP(T_arg%1, $1))> slot )
    { m_slot = slot; }

    static pointer create(const std::string& name)
    { return pointer( new Method(name) ); }

    virtual MethodBase::pointer clone()
    { return MethodBase::pointer( new Method(this->name()) ); }

  protected:

    sigc::slot$1<LIST(void, LOOP(T_arg%1, $1))> m_slot;

  };

])

divert(0)
#ifndef DBUS_CXX_METHOD_IMPL_H
#define DBUS_CXX_METHOD_IMPL_H

#include <dbus-cxx/method.h>
#include <dbus-cxx/connection.h>
    
namespace DBus {

FOR(0,eval(CALL_SIZE-1),[[METHOD_VOID(%1)]])

}

#endif

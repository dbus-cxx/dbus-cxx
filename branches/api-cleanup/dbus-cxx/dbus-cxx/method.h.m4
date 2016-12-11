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

define([TEMPLATE_PREFIX],[dnl
define([VAR1],ifelse($3,[void],,[class T_return]))dnl
template <LIST(VAR1, LOOP(class T_arg%1, $1))>])

define([TEMPLATE_SPECIALIZATION],[dnl
define([VAR1],ifelse($3,[void],[void],[T_return]))dnl
ifelse($1, $2,,[<LIST(VAR1, LOOP(T_arg%1,$1), LOOP(nil, CALL_SIZE - $1))>])])

define([MESSAGE_HANDLER_BODY],[dnl
define([RETURN_TYPE],ifelse($3,[void],[void],[T_return]))dnl
  {
    ifelse($3,[void],
    [DBUS_CXX_DEBUG("Method<void, LOOP(T_arg%1, $1)>::handle_call_message method=" << m_name);],
    [DBUS_CXX_DEBUG("Method<LIST(T_return, LOOP(T_arg%1, $1))>::handle_call_message   method=" << m_name );])
      
    if ( not connection or not message ) return NOT_HANDLED;

ifelse(RETURN_TYPE,[void],,[dnl
    T_return _retval;
])dnl
FOR(1, $1,[dnl
    T_arg%1 _val_%1;
])dnl

ifelse(eval($1>0),1,[dnl
    try {
      Message::iterator i = message->begin();
      i FOR(1, $1,[ >> _val_%1]);
    }
    catch ( ErrorInvalidTypecast& e ) {
        return NOT_HANDLED;
    }
],[])dnl

    try {
      ifelse(RETURN_TYPE,[void],,[_retval = ])m_slot(LIST(LOOP(_val_%1, $1)));
      ReturnMessage::pointer retmsg = message->create_reply();

      if ( not retmsg ) return NOT_HANDLED;

ifelse(RETURN_TYPE,[void],,[dnl
      *retmsg << _retval;
])dnl

      connection->send(retmsg);
    }
    catch ( const std::exception &e ) {
      ErrorMessage::pointer errmsg = ErrorMessage::create( message, DBUS_ERROR_FAILED, e.what() );

      if ( not errmsg ) return NOT_HANDLED;

      connection->send(errmsg);
    }
    catch ( ... ) {
      std::ostringstream stream;
      stream << "DBus-cxx " << DBUS_CXX_PACKAGE_MAJOR_VERSION << "." << 
           DBUS_CXX_PACKAGE_MINOR_VERSION << "." << DBUS_CXX_PACKAGE_MICRO_VERSION << " unknown error.";
      ErrorMessage::pointer errmsg = ErrorMessage::create( message, DBUS_ERROR_FAILED, stream.str() );

      if ( not errmsg ) return NOT_HANDLED;

      connection->send(errmsg);
    }

    return HANDLED;
  }

])

define([DECLARE_METHOD],[dnl
define([RETURN_TYPE],ifelse($3,[void],[void],[T_return]))dnl
  /**
   * @ingroup local
   * @ingroup objects
   * 
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  TEMPLATE_PREFIX($1,$2,$3)
  class Method TEMPLATE_SPECIALIZATION($1,$2,$3)
    : public MethodBase
  {
  public:

    typedef DBusCxxPointer<Method> pointer;

    Method(const std::string& name): MethodBase(name) {  }
    
    virtual ~Method() { }

ifelse(RETURN_TYPE,[void],[dnl
    virtual HandlerResult handle_call_message( DBusCxxPointer<Connection> connection,
					       CallMessage::const_pointer message )
 MESSAGE_HANDLER_BODY($1,$2,$3)
],[dnl
    virtual HandlerResult handle_call_message( DBusCxxPointer<Connection> connection,
					       CallMessage::const_pointer message );
])dnl

    void set_method( sigc::slot$1<LIST(RETURN_TYPE, LOOP(T_arg%1, $1))> slot )
    { m_slot = slot; }

    static pointer create(const std::string& name)
    { return pointer( new Method(name) ); }

    virtual MethodBase::pointer clone()
    { return MethodBase::pointer( new Method(this->name()) ); }

    /** Returns a DBus XML description of this interface */
    virtual std::string introspect(int space_depth=0) const
    {
      std::ostringstream sout;
      std::string spaces;
      for (int i=0; i < space_depth; i++ ) spaces += " ";
      sout << spaces << "<method name=\"" << name() << "\">\n";
ifelse(RETURN_TYPE,[void],,[dnl
      T_return type;
      sout << spaces << "  <arg name=\"" << m_arg_names[[0]]
           << "\" type=\"" << signature(type)
           << "\" direction=\"out\"/>\n";
])dnl
FOR(1,$1,[dnl
      T_arg%1 arg%1;
      sout << spaces << "  <arg name=\"" << m_arg_names[[[%1]]]
           << "\" type=\"" << signature(arg%1)
           << "\" direction=\"in\"/>\n";
],[])dnl
      sout << spaces << "</method>\n";
      return sout.str();
    }

    virtual std::string arg_name(size_t i) {
      if ( i < $1+1 ) return m_arg_names[[i]];
      return std::string();
    }

    virtual void set_arg_name(size_t i, const std::string& name) {
      if ( i < $1+1 ) m_arg_names[[i]] = name;
    }

  protected:

    std::string m_arg_names[[$1+1]];

    sigc::slot$1<LIST(RETURN_TYPE, LOOP(T_arg%1, $1))> m_slot;

  };

])

define([DEFINE_METHOD],[dnl
define([RETURN_TYPE],ifelse($3,[void],[void],[T_return]))dnl
  TEMPLATE_PREFIX($1,$2,$3)
  HandlerResult Method TEMPLATE_SPECIALIZATION($1,$3,$3)::handle_call_message( DBusCxxPointer<Connection> connection, CallMessage::const_pointer message )
MESSAGE_HANDLER_BODY($1,$2,$3)
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
#include <sstream>
#include <dbus-cxx/forward_decls.h>
#include <dbus-cxx/methodbase.h>
#include <dbus-cxx/errormessage.h>
#include <dbus-cxx/utility.h>
#include <exception>
#include <ostream>
    
#ifndef DBUSCXX_METHOD_H
#define DBUSCXX_METHOD_H

namespace DBus {

DECLARE_METHOD(CALL_SIZE,CALL_SIZE)
FOR(0,eval(CALL_SIZE-1),[[DECLARE_METHOD(%1)]])

} /* namespace DBus */

#include <dbus-cxx/connection.h>

namespace DBus {

DEFINE_METHOD(CALL_SIZE,CALL_SIZE)
FOR(0,eval(CALL_SIZE-1),[[DEFINE_METHOD(%1)]])
FOR(0,eval(CALL_SIZE),[[DECLARE_METHOD(%1,[void],[void])]])

} /* namespace DBus */

#endif /* DBUS_CXX_METHOD_H */


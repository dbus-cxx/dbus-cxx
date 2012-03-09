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

define([SIGNAL],[dnl
ifelse($1, $2,[dnl
/**
 * Subclass of the numbered sigc::signal templates.
 *
 * This class defines a subclass of the sigc::signal types and can be used
 * as its parent. The key difference is that when the signal is emitted
 * a DBus signal message is created using the parameters of the signal
 * emission and transmitted via the dbus in addition to any local
 * signal propagation that may take place.
 *
 * @ingroup signals
 *
 * @ingroup local
 *
 * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
 */
template <LIST(class T_return, LOOP(class T_arg%1 = nil, $1))>],[dnl

/**
 * Subclass of the numbered sigc::signal templates.
 *
 * This class defines a subclass of the sigc::signal types and can be used
 * as its parent. The key difference is that when the signal is emitted
 * a DBus signal message is created using the parameters of the signal
 * emission and transmitted via the dbus in addition to any local
 * signal propagation that may take place.
 *
 * @ingroup signals
 *
 * @ingroup local
 *
 * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
 */
template <LIST(class T_return, LOOP(class T_arg%1, $1))>])
class signal ifelse($1, $2,,[<LIST(T_return, LOOP(T_arg%1,$1), LOOP(nil, CALL_SIZE - $1))>])
  : public sigc::signal<LIST(T_return, LOOP(T_arg%1, $1))>, public signal_base
{
public:

  //template <class T_accumulator>
  //class accumulated
  //: public sigc::signal$1<LIST(T_return, LOOP(T_arg%1, $1),T_accumulator)>, public signal_base
  //{
  //public:
  //  typedef DBusCxxPointer<accumulated> pointer;
  //
  //  accumulated(const std::string& interface, const std::string& name):
  //    signal_base(interface, name)
  //  {
  //    m_internal_callback_connection =
  //      this->connect( sigc::mem_fun(*this, &accumulated::internal_callback) );
  //      
  //    m_signal_dbus_incoming.connect( sigc::mem_fun(*this, &accumulated::on_dbus_incoming) );
  //  }

  //  accumulated(const std::string& path, const std::string& interface, const std::string& name):
  //    signal_base(path, interface, name)
  //  {
  //    m_internal_callback_connection =
  //      this->connect( sigc::mem_fun(*this, &accumulated::internal_callback) );
  //      
  //    m_signal_dbus_incoming.connect( sigc::mem_fun(*this, &accumulated::on_dbus_incoming) );
  //  }

  //  accumulated(const std::string& interface, const std::string& name, const accumulated& src):
  //    sigc::signal$1<LIST(T_return, LOOP(T_arg%1, $1),T_accumulator)>(src),
  //    signal_base(interface, name)
  //  {}

  //  accumulated(const std::string& path, const std::string& interface, const std::string& name, const accumulated& src):
  //    sigc::signal$1<LIST(T_return, LOOP(T_arg%1, $1),T_accumulator)>(src),
  //    signal_base(path, interface, name)
  //  {}

  //  static pointer create(const std::string& interface, const std::string& name)
  //  {
  //    return pointer( new accumulated(interface, name) );
  //  }
    
  //  static pointer create(const std::string& path, const std::string& interface, const std::string& name)
  //  {
  //    return pointer( new accumulated(path, interface, name) );
  //  }
    
  //  static pointer create(const std::string& interface, const std::string& name, const accumulated& src)
  //  {
  //    return pointer( new accumulated(interface, name, src) );
  //  }
    
  //  static pointer create(const std::string& path, const std::string& interface, const std::string& name, const accumulated& src)
  //  {
  //    return pointer( new accumulated(path, interface, name, src) );
  //  }
    
  //  virtual signal_base::pointer clone()
  //  {
  //    return signal_base::pointer( new accumulated(*this) );
  //  }

  //protected:

  //  sigc::connection m_internal_callback_connection;

  //  virtual HandlerResult on_dbus_incoming( SignalMessage::const_pointer msg )
  //  {
  //    // T_return _retval;
  //    dnl
  //    FOR(1, $1,[
  //    T_arg%1 _val_%1;])

  //    m_internal_callback_connection.block();
    
  //    try {
  //      Message::iterator i = msg->begin();
  //      i FOR(1, $1,[ >> _val_%1]);
  //      this->emit(LIST(LOOP(_val_%1, $1)));
  //    }
  //    catch ( ErrorInvalidTypecast& e ) {
  //      m_internal_callback_connection.unblock();
  //      return NOT_HANDLED;
  //    }
    
  //    m_internal_callback_connection.unblock();

  //    return HANDLED;
  //  }


  //  T_return internal_callback(LIST(LOOP(T_arg%1 arg%1, $1)))
  //  {
    
  //  }

  //};

  typedef DBusCxxPointer<signal> pointer;
  
  signal(const std::string& interface, const std::string& name):
    signal_base(interface, name)
  {
    m_internal_callback_connection =
      this->connect( sigc::mem_fun(*this, &signal::internal_callback) );
  }
  
  signal(const std::string& path, const std::string& interface, const std::string& name):
    signal_base(path, interface, name)
  {
    m_internal_callback_connection =
      this->connect( sigc::mem_fun(*this, &signal::internal_callback) );
  }
  
  signal(const std::string& interface, const std::string& name, const signal& src) :
    sigc::signal<LIST(T_return, LOOP(T_arg%1, $1))>(src),
    signal_base(interface, name)
  { }

  signal(const std::string& path, const std::string& interface, const std::string& name, const signal& src) :
    sigc::signal<LIST(T_return, LOOP(T_arg%1, $1))>(src),
    signal_base(path, interface, name)
  { }

  static pointer create(const std::string& interface, const std::string& name)
  {
    return pointer( new signal(interface, name) );
  }

  static pointer create(const std::string& path, const std::string& interface, const std::string& name)
  {
    return pointer( new signal(path, interface, name) );
  }

  static pointer create(const std::string& interface, const std::string& name, const signal& src)
  {
    return pointer( new signal(interface, name, src) );
  }

  static pointer create(const std::string& path, const std::string& interface, const std::string& name, const signal& src)
  {
    return pointer( new signal(path, interface, name, src) );
  }

  virtual signal_base::pointer clone()
  {
    return signal_base::pointer( new signal(*this) );
  }

  /** Returns a DBus XML description of this interface */
  virtual std::string introspect(int space_depth=0) const
  {
    std::ostringstream sout;
    std::string spaces;
    for (int i=0; i < space_depth; i++ ) spaces += " ";
    sout << spaces << "<signal name=\"" << name() << "\">\n";
    FOR(1,$1,[
    T_arg%1 arg%1;
    sout << spaces << "  <arg name=\"" << m_arg_names[[[%1-1]]] << "\" type=\"" << signature(arg%1) << "\"/>\n";],[])
    sout << spaces << "</signal>\n";
    return sout.str();
  }

  virtual std::string arg_name(size_t i) {
    if ( i < $1 ) return m_arg_names[[i]];
    return std::string();
  }

  virtual void set_arg_name(size_t i, const std::string& name) {
    if ( i < $1 ) m_arg_names[[i]] = name;
  }

  protected:

  friend class Interface;

  std::string m_arg_names[[$1]];

  sigc::connection m_internal_callback_connection;

  T_return internal_callback(LIST(LOOP(T_arg%1 arg%1, $1)))
  {
    // DBUS_CXX_DEBUG( "signal::internal_callback: " FOR(1,$1,[ << arg%1]) );
    SignalMessage::pointer __msg = SignalMessage::create(m_path, m_interface, m_name);
    if ( not m_destination.empty() ) __msg->set_destination(m_destination);
    ifelse(eval($1>0),1,[*__msg FOR(1, $1,[ << arg%1]);],[])
    #ifdef DBUS_CXX_DEBUG_ENABLED
      bool result = this->handle_dbus_outgoing(__msg);
      DBUS_CXX_DEBUG( "signal::internal_callback: result=" << result );
    #else
      this->handle_dbus_outgoing(__msg);
    #endif
  }

};

])

divert(0)
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
#include <dbus-cxx/utility.h>
#include <dbus-cxx/signal_base.h>

#ifndef DBUSCXX_DBUS_SIGNAL_H_
#define DBUSCXX_DBUS_SIGNAL_H_

namespace DBus {

SIGNAL(CALL_SIZE,CALL_SIZE)
FOR(0,eval(CALL_SIZE-1),[[SIGNAL(%1)]])

} /* namespace DBus */

#endif /* _DBUS_CXX_SIGNAL_H_ */

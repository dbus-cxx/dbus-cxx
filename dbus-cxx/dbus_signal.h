/***************************************************************************
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
#include <sstream>
#include <dbus-cxx/forward_decls.h>
#include <dbus-cxx/utility.h>
#include <dbus-cxx/signal_base.h>
#include <dbus-cxx/headerlog.h>

#ifndef DBUSCXX_DBUS_SIGNAL_H_
#define DBUSCXX_DBUS_SIGNAL_H_

namespace DBus {

/**
 * Subclass of sigc::signal
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
 */
template <typename T_return, typename... T_arg>
class signal 
  : public sigc::signal<T_return(T_arg...)>, public signal_base
{
  typedef DBusCxxPointer<signal> pointer;
  
  signal(const std::string& interface, const std::string& name):
    signal_base(interface, name),
    m_arg_names(sizeof...(T_arg))
  {
    m_internal_callback_connection =
      this->connect( sigc::mem_fun(*this, &signal::internal_callback) );
  }
  
  signal(const std::string& path, const std::string& interface, const std::string& name):
    signal_base(path, interface, name),
    m_arg_names(sizeof...(T_arg))
  {
    m_internal_callback_connection =
      this->connect( sigc::mem_fun(*this, &signal::internal_callback) );
  }
  
  signal(const std::string& interface, const std::string& name, const signal& src) :
    sigc::signal<T_return(T_arg...)>(src),
    signal_base(interface, name),
    m_arg_names(sizeof...(T_arg))
  { }

  signal(const std::string& path, const std::string& interface, const std::string& name, const signal& src) :
    sigc::signal<T_return(T_arg...)>(src),
    signal_base(path, interface, name),
    m_arg_names(sizeof...(T_arg))
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
    for (std::string arg_name : m_arg_names ){
      sout << spaces << "  <arg name=\"" << arg_name << "\" type=\"" << sig<T_arg...>().sig() << "\"/>\n";
    }
    sout << spaces << "</signal>\n";
    return sout.str();
  }

  virtual std::string arg_name(size_t i) {
    if ( i < m_arg_names.size() ) return m_arg_names[i];
    return std::string();
  }

  virtual void set_arg_name(size_t i, const std::string& name) {
    if ( i < m_arg_names.size() )
        m_arg_names[i] = name;
  }

  protected:

  friend class Interface;

  std::vector<std::string> m_arg_names;

  sigc::connection m_internal_callback_connection;

  //template<class ... T_arg>
  T_return internal_callback(T_arg ... args)
  {
    // DBUS_CXX_DEBUG( "signal::internal_callback: " FOR(1,$1,[ << arg%1]) );
    SignalMessage::pointer __msg = SignalMessage::create(m_path, m_interface, m_name);
    if ( not m_destination.empty() ) __msg->set_destination(m_destination);
    *__msg << args;
    bool result = this->handle_dbus_outgoing(__msg);
    DBUSCXX_DEBUG_STDSTR( "dbus.signal", "signal::internal_callback: result=" << result );
  }
};

} /* namespace DBus */

#endif /* _DBUS_CXX_SIGNAL_H_ */

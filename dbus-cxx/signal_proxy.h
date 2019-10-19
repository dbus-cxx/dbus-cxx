/***************************************************************************
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
 ***************************************************************************/
#include <dbus-cxx/signal_proxy_base.h>

#ifndef DBUSCXX_SIGNALPROXY_H_
#define DBUSCXX_SIGNALPROXY_H_

namespace DBus {

/**
 * Subclass of the sigc::signal templates.
 *
 * @ingroup signals
 * @ingroup proxy
 *
 * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
 */
template <class... T_arg>
class signal_proxy
  : public sigc::signal<void(T_arg...)>, public signal_proxy_base
{
  public:

    typedef std::shared_ptr<signal_proxy> pointer;
  
    signal_proxy(const std::string& interface, const std::string& name):
      signal_proxy_base(interface, name)
    { m_signal_dbus_incoming.connect( sigc::mem_fun(*this, &signal_proxy::on_dbus_incoming) ); }

    signal_proxy(const std::string& path, const std::string& interface, const std::string& name):
      signal_proxy_base(path, interface, name)
    { m_signal_dbus_incoming.connect( sigc::mem_fun(*this, &signal_proxy::on_dbus_incoming) ); }

    signal_proxy(const std::string& interface, const std::string& name, const signal_proxy& src) :
      sigc::signal<void(T_arg...)>(src),
      signal_proxy_base(interface, name)
    { m_signal_dbus_incoming.connect( sigc::mem_fun(*this, &signal_proxy::on_dbus_incoming) ); }

    signal_proxy(const std::string& path, const std::string& interface, const std::string& name, const signal_proxy& src) :
      sigc::signal<void(T_arg...)>(src),
      signal_proxy_base(path, interface, name)
    { m_signal_dbus_incoming.connect( sigc::mem_fun(*this, &signal_proxy::on_dbus_incoming) ); }

    static pointer create(const std::string& interface, const std::string& name)
    { return pointer( new signal_proxy(interface, name) ); }

    static pointer create(const std::string& path, const std::string& interface, const std::string& name)
    { return pointer( new signal_proxy(path, interface, name) ); }

    static pointer create(const std::string& interface, const std::string& name, const signal_proxy& src)
    { return pointer( new signal_proxy(interface, name, src) ); }

    static pointer create(const std::string& path, const std::string& interface, const std::string& name, const signal_proxy& src)
    { return pointer( new signal_proxy(path, interface, name, src) ); }

    virtual signal_base::pointer clone()
    { return signal_base::pointer( new signal_proxy(*this) ); }

  protected:
    virtual HandlerResult on_dbus_incoming( SignalMessage::const_pointer msg )
    {
      std::tuple<T_arg...> tup_args;

      try {
        Message::iterator i = msg->begin();
        std::apply( [i](auto ...arg) mutable {
               (i >> ... >> arg);
              },
        tup_args );
        std::apply(&signal_proxy::emit, std::tuple_cat(std::make_tuple(this), tup_args) );
      }
      catch ( ErrorInvalidTypecast& e ) {
          return NOT_HANDLED;
      }
    
      return HANDLED;
    }

};
  

} /* namespace DBus */

#endif /* _DBUS_CXX_SIGNALPROXY_H_ */

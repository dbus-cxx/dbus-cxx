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
#include <dbus-cxx/signal_base.h>
#include <dbus-cxx/accumulators.h>

#ifndef DBUSCXX_SIGNALPROXYBASE_H
#define DBUSCXX_SIGNALPROXYBASE_H

namespace DBus
{
  /**
   * @ingroup proxy
   * @ingroup signals
   * 
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */

  // TODO fix signals that expect a return value and partially specialize for void returns
  
  class signal_proxy_base: public signal_base
  {
    public:
      signal_proxy_base(const std::string& path, const std::string& interface, const std::string& name);

      signal_proxy_base(const std::string& interface, const std::string& name);

      signal_proxy_base(std::shared_ptr<Connection> connection, const std::string& path, const std::string& interface, const std::string& name);

      signal_proxy_base(std::shared_ptr<Connection> connection, const std::string& interface, const std::string& name);

      signal_proxy_base(const signal_proxy_base& other);

      virtual ~signal_proxy_base();

      virtual HandlerResult handle_signal( std::shared_ptr<const SignalMessage> );

      sigc::signal<HandlerResult(std::shared_ptr<const SignalMessage>)>::accumulated<MessageHandlerAccumulator> signal_dbus_incoming();

      const std::string& match_rule();

      bool matches(std::shared_ptr<const Message> msg);

      /**
       * This method is needed to be able to create a duplicate of a child
       * capable of parsing their specific template type message.
       */
      virtual std::shared_ptr<signal_base> clone() = 0;

    protected:

      std::string m_match_rule;

      sigc::signal<HandlerResult(std::shared_ptr<const SignalMessage>)>::accumulated<MessageHandlerAccumulator> m_signal_dbus_incoming;
  };

  class signal_proxy_simple: public signal_proxy_base, public sigc::trackable
  {
    public:
      signal_proxy_simple(const std::string& path, const std::string& interface, const std::string& name);

      signal_proxy_simple(const std::string& interface, const std::string& name);

      signal_proxy_simple(std::shared_ptr<Connection> connection, const std::string& path, const std::string& interface, const std::string& name);

      signal_proxy_simple(std::shared_ptr<Connection> connection, const std::string& interface, const std::string& name);

      signal_proxy_simple(const signal_proxy_simple& other);

      static std::shared_ptr<signal_proxy_simple> create(const std::string& path, const std::string& interface, const std::string& name);

      static std::shared_ptr<signal_proxy_simple> create(const std::string& interface, const std::string& name);

      static std::shared_ptr<signal_proxy_simple> create(std::shared_ptr<Connection> connection, const std::string& path, const std::string& interface, const std::string& name);

      static std::shared_ptr<signal_proxy_simple> create(std::shared_ptr<Connection> connection, const std::string& interface, const std::string& name);

      static std::shared_ptr<signal_proxy_simple> create(const signal_proxy_simple& other);

      virtual ~signal_proxy_simple();

      /**
       * This method is needed to be able to create a duplicate of a child
       * capable of parsing their specific template type message.
       */
      virtual std::shared_ptr<signal_base> clone();
  };

}

#endif

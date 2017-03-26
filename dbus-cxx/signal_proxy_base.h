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

      typedef DBusCxxPointer<signal_proxy_base> pointer;

      signal_proxy_base(const std::string& path, const std::string& interface, const std::string& name);

      signal_proxy_base(const std::string& interface, const std::string& name);

      signal_proxy_base(DBusCxxPointer<Connection> connection, const std::string& path, const std::string& interface, const std::string& name);

      signal_proxy_base(DBusCxxPointer<Connection> connection, const std::string& interface, const std::string& name);

      signal_proxy_base(const signal_proxy_base& other);

      virtual ~signal_proxy_base();

      virtual HandlerResult handle_signal( SignalMessage::const_pointer );

      sigc::signal<HandlerResult,SignalMessage::const_pointer>::accumulated<MessageHandlerAccumulator> signal_dbus_incoming();

      const std::string& match_rule();

      bool matches(Message::const_pointer msg);

      /**
       * This method is needed to be able to create a duplicate of a child
       * capable of parsing their specific template type message.
       */
      virtual signal_base::pointer clone() = 0;

    protected:

      std::string m_match_rule;

      sigc::signal<HandlerResult,SignalMessage::const_pointer>::accumulated<MessageHandlerAccumulator> m_signal_dbus_incoming;
  };

  class signal_proxy_simple: public signal_proxy_base, public sigc::trackable
  {
    public:

      typedef DBusCxxPointer<signal_proxy_base> pointer;

      signal_proxy_simple(const std::string& path, const std::string& interface, const std::string& name);

      signal_proxy_simple(const std::string& interface, const std::string& name);

      signal_proxy_simple(DBusCxxPointer<Connection> connection, const std::string& path, const std::string& interface, const std::string& name);

      signal_proxy_simple(DBusCxxPointer<Connection> connection, const std::string& interface, const std::string& name);

      signal_proxy_simple(const signal_proxy_simple& other);

      static pointer create(const std::string& path, const std::string& interface, const std::string& name);

      static pointer create(const std::string& interface, const std::string& name);

      static pointer create(DBusCxxPointer<Connection> connection, const std::string& path, const std::string& interface, const std::string& name);

      static pointer create(DBusCxxPointer<Connection> connection, const std::string& interface, const std::string& name);

      static pointer create(const signal_proxy_simple& other);

      virtual ~signal_proxy_simple();

      /**
       * This method is needed to be able to create a duplicate of a child
       * capable of parsing their specific template type message.
       */
      virtual signal_base::pointer clone();
  };

}

#endif

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
#include <dbus-cxx/messagehandler.h>

#ifndef DBUSCXX_SIGNALRECEIVER_H
#define DBUSCXX_SIGNALRECEIVER_H

namespace DBus
{

  /**
   * This class serves as a receiver of @e incoming signal messages.
   *
   * The messages this receiver will process are determined by the
   * sender, interface, member, path and destination values.
   *
   * A value that is not set will not be used in determining processing.
   * Thus, if the only two values set a interface ('org.freedesktop.Hal.Manager') and
   * member ('NameOwnerChanged') then the signal will be processed
   * for all senders.
   *
   * @ingroup signals
   * @ingroup proxy
   *
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class SignalReceiver : public MessageHandler
  {
    protected:
      SignalReceiver(const std::string& path, const std::string& interface, const std::string& member);

    public:

      typedef DBusCxxPointer<SignalReceiver> pointer;

      typedef DBusCxxWeakPointer<SignalReceiver> weak_pointer;

      static pointer create(const std::string& interface, const std::string& member);

      static pointer create(const std::string& path, const std::string& interface, const std::string& member);

      ~SignalReceiver();

      virtual HandlerResult handle_message( DBusCxxPointer<Connection>, Message::const_pointer );

      const std::string& sender();

      void set_sender(const std::string& s);

      const std::string& interface();

      void set_interface(const std::string& s);

      const std::string& member();

      void set_member(const std::string& s);

      const Path& path();

      void set_path(const std::string& s);

      const std::string& destination();

      void set_destination(const std::string& s);

      sigc::signal<void>& signal_sender_changed();

      sigc::signal<void>& signal_interface_changed();

      sigc::signal<void>& signal_member_changed();

      sigc::signal<void>& signal_path_changed();

      sigc::signal<void>& signal_destination_changed();

      const std::string& match_rule();

      bool matches(Message::const_pointer msg);

    protected:

      std::string m_sender;

      Path m_path;

      std::string m_interface;

      std::string m_member;

      std::string m_destination;

      sigc::signal<void> m_signal_sender_changed;

      sigc::signal<void> m_signal_interface_changed;

      sigc::signal<void> m_signal_member_changed;

      sigc::signal<void> m_signal_path_changed;

      sigc::signal<void> m_signal_destination_changed;

      std::string m_match_rule;

  };

}

#endif

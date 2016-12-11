/***************************************************************************
 *   Copyright (C) 2009 by Rick L. Vinyard, Jr.                            *
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

#include "signalreceiver.h"

#include "signalmessage.h"

#include "utility.h"

namespace DBus
{

  SignalReceiver::SignalReceiver(const std::string& path, const std::string& interface, const std::string& member):
      MessageHandler(),
      m_path(path),
      m_interface(interface),
      m_member(member)
  {
  }

  SignalReceiver::pointer SignalReceiver::create(const std::string & interface, const std::string & member)
  {
    return pointer( new SignalReceiver(std::string(), interface, member) );
  }

  SignalReceiver::pointer SignalReceiver::create(const std::string & path, const std::string & interface, const std::string & member)
  {
    return pointer( new SignalReceiver(path, interface, member) );
  }

  SignalReceiver::~SignalReceiver()
  {
  }

  HandlerResult SignalReceiver::handle_message(DBusCxxPointer< Connection > conn, Message::const_pointer msg)
  {
    DBUS_CXX_DEBUG( "SignalReceiver::handle_message" );
    
    if ( not this->matches(msg) ) return NOT_HANDLED;

    return m_signal_message.emit(conn, msg);
  }

  const std::string & SignalReceiver::sender()
  {
    return m_sender;
  }

  void SignalReceiver::set_sender(const std::string & s)
  {
    m_sender = s;
    m_signal_sender_changed.emit();
  }

  const std::string & SignalReceiver::interface()
  {
    return m_interface;
  }

  void SignalReceiver::set_interface(const std::string & s)
  {
    m_interface = s;
    m_signal_interface_changed.emit();
  }

  const std::string & SignalReceiver::member()
  {
    return m_member;
  }

  void SignalReceiver::set_member(const std::string & s)
  {
    m_member = s;
    m_signal_member_changed.emit();
  }

  const Path& SignalReceiver::path()
  {
    return m_path;
  }

  void SignalReceiver::set_path(const std::string & s)
  {
    m_path = s;
    m_signal_path_changed.emit();
  }

  const std::string & SignalReceiver::destination()
  {
    return m_destination;
  }

  void SignalReceiver::set_destination(const std::string & s)
  {
    m_destination = s;
    m_signal_destination_changed.emit();
  }

  sigc::signal< void > & SignalReceiver::signal_sender_changed()
  {
    return m_signal_sender_changed;
  }

  sigc::signal< void > & SignalReceiver::signal_interface_changed()
  {
    return m_signal_interface_changed;
  }

  sigc::signal< void > & SignalReceiver::signal_member_changed()
  {
    return m_signal_member_changed;
  }

  sigc::signal< void > & SignalReceiver::signal_path_changed()
  {
    return m_signal_path_changed;
  }

  sigc::signal< void > & SignalReceiver::signal_destination_changed()
  {
    return m_signal_destination_changed;
  }

  const std::string & SignalReceiver::match_rule()
  {
    if ( m_interface.empty() or m_member.empty() )
    {
      m_match_rule = std::string();
      return m_match_rule;
    }
    m_match_rule = "type='signal'";
    m_match_rule += ",interface='"   + m_interface   + "'";
    m_match_rule += ",member='"      + m_member      + "'";
    if ( not m_sender.empty() )      m_match_rule += ",sender='"      + m_sender      + "'";
    if ( not m_path.empty() )        m_match_rule += ",path='"        + m_path        + "'";
    if ( not m_destination.empty() ) m_match_rule += ",destination='" + m_destination + "'";
    return m_match_rule;
  }

  bool SignalReceiver::matches(Message::const_pointer msg)
  {
    if (not msg or not msg->is_valid()) return false;
    if ( msg->type() != SIGNAL_MESSAGE ) return false;
    if ( m_interface.empty() or m_member.empty() ) return false;

    SignalMessage::const_pointer smsg;
    smsg = dbus_cxx_dynamic_pointer_cast<const SignalMessage>(msg);

    if ( not smsg ) smsg = SignalMessage::create(msg);

    if ( m_interface != smsg->interface() ) return false;
    
    if ( m_member != smsg->member() ) return false;

    if ( not m_sender.empty() and m_sender != smsg->sender() ) return false;
    
    if ( not m_destination.empty() and m_destination != smsg->destination() ) return false;
    
    if ( not m_path.empty() and m_path != smsg->path() ) return false;
    
    return true;
  }

}


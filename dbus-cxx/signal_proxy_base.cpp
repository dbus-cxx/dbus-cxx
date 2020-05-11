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
#include "signal_proxy_base.h"
#include <dbus-cxx/signalmessage.h>
#include "path.h"
#include "signal_base.h"

namespace DBus
{

SignalMatchRule::SignalMatchRule(){}

SignalMatchRule& SignalMatchRule::setPath( const std::string& path ){
    m_path = path;
    return *this;
}

SignalMatchRule& SignalMatchRule::setInterface( const std::string& interface ){
    m_interface = interface;
    return *this;
}

SignalMatchRule& SignalMatchRule::setMember( const std::string& member ){
    m_member = member;
    return *this;
}

SignalMatchRule& SignalMatchRule::setSender( const std::string& sender ){
    m_sender = sender;
    return *this;
}

SignalMatchRule& SignalMatchRule::setDestination( const std::string& destination ){
    m_destination = destination;
    return *this;
}

std::string SignalMatchRule::getPath() const {
    return m_path;
}

std::string SignalMatchRule::getInterface() const {
    return m_interface;
}

std::string SignalMatchRule::getMember() const {
    return m_member;
}


std::string SignalMatchRule::getMatchRule() const {
    std::string match_rule = "type='signal'";
    if( !m_interface.empty() )   match_rule += ",interface='"   + m_interface   + "'";
    if( !m_member.empty() )      match_rule += ",member='"      + m_member      + "'";
    if( !m_sender.empty() )      match_rule += ",sender='"      + m_sender      + "'";
    if( !m_path.empty() )        match_rule += ",path='"        + m_path        + "'";
    if( !m_destination.empty() ) match_rule += ",destination='" + m_destination + "'";

    return match_rule;
}

SignalMatchRule SignalMatchRule::create(){
    return SignalMatchRule();
}

  signal_proxy_base::signal_proxy_base( const SignalMatchRule& matchRule ):
      signal_base( matchRule.getPath(), matchRule.getInterface(), matchRule.getMember() ),
      m_match_rule( matchRule.getMatchRule() )
  {
  }

  signal_proxy_base::signal_proxy_base( const signal_proxy_base& other ):
      signal_base( other )
  {
    // TODO connect to the other's connection
  }

  signal_proxy_base::~signal_proxy_base()
  {
  }

  HandlerResult signal_proxy_base::handle_signal( std::shared_ptr<const SignalMessage> msg )
  {
    if ( not this->matches( msg ) ) return HandlerResult::Not_Handled;

    return on_dbus_incoming( msg );
  }

  const std::string & signal_proxy_base::match_rule() const
  {
    return m_match_rule;
  }

  bool signal_proxy_base::matches( std::shared_ptr<const SignalMessage> msg )
  {
    if ( not msg or not msg->is_valid() ) return false;

    if ( not m_interface.empty() && m_interface != msg->interface() ) return false;

    if ( not m_name.empty() && m_name != msg->member() ) return false;

    if ( not m_sender.empty() and m_sender != msg->sender() ) return false;

    if ( not m_destination.empty() and m_destination != msg->destination() ) return false;

    if ( not m_path.empty() and m_path != msg->path() ) return false;

    return true;
  }

}


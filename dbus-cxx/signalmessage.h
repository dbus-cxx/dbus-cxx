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
#include <dbus-cxx/message.h>

#ifndef DBUSCXX_SIGNALMESSAGE_H
#define DBUSCXX_SIGNALMESSAGE_H

namespace DBus
{

  /**
   * Represents a DBus signal message
   *
   * This class inherits from Message and uses the dbus_message_* methods to
   * create an interface for a signal message that is received from a signal
   * or transmitted on signal emission.
   *
   * @ingroup message
   *
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class SignalMessage : public Message
  {
    protected:

      SignalMessage( DBusMessage* cobj=NULL, CreateMethod m = CREATE_ALIAS );
      
      SignalMessage( Message::pointer msg );

      SignalMessage( Message::const_pointer msg );

      SignalMessage( const std::string& name );

      SignalMessage( const std::string& path, const std::string& interface, const std::string& name );

    public:

      typedef DBusCxxPointer<SignalMessage> pointer;

      typedef DBusCxxPointer<const SignalMessage> const_pointer;

      typedef DBusCxxWeakPointer<SignalMessage> weak_pointer;

      static pointer create( DBusMessage* cobj=NULL, CreateMethod m = CREATE_ALIAS );
      
      static pointer create( Message::pointer msg );

      static const_pointer create( Message::const_pointer msg );

      static pointer create( const std::string& name );

      static pointer create( const std::string& path, const std::string& interface, const std::string& name );

      bool set_path( const std::string& p );

      Path path() const;

      bool has_path( const std::string& p ) const;

      std::vector<std::string> path_decomposed() const;

      bool set_interface( const std::string& i );

      const char* interface() const;

      bool has_interface( const std::string& i ) const;

      bool set_member( const std::string& m );

      const char* member() const;

      bool has_member( const std::string& m ) const;

      bool operator == ( const SignalMessage& ) const;
  
  };

}

#endif

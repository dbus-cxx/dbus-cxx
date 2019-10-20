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
#include <dbus-cxx/path.h>
#include <dbus-cxx/message.h>

#ifndef DBUSCXX_CALLMESSAGE_H
#define DBUSCXX_CALLMESSAGE_H

namespace DBus
{

  /**
   * Represents a DBus call message
   *
   * This class inherits from Message and uses the dbus_message_* methods to
   * create an interface for a call message.
   *
   * @ingroup message
   *
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class CallMessage : public Message
  {
    protected:
      
      CallMessage();

      CallMessage( DBusMessage* cobj );

      CallMessage( std::shared_ptr<Message> msg );

      CallMessage( std::shared_ptr<const Message> msg );

      CallMessage( const std::string& dest, const std::string& path, const std::string& iface, const std::string& method );

      CallMessage( const std::string& path, const std::string& iface, const std::string& method );

      CallMessage( const std::string& path, const std::string& method );

    public:

      static std::shared_ptr<CallMessage> create();

      static std::shared_ptr<CallMessage> create( DBusMessage* cobj );

      static std::shared_ptr<CallMessage> create( std::shared_ptr<Message> msg );

      static std::shared_ptr<const CallMessage> create( std::shared_ptr<const Message> msg);

      static std::shared_ptr<CallMessage> create( const std::string& dest, const std::string& path, const std::string& iface, const std::string& method );

      static std::shared_ptr<CallMessage> create( const std::string& path, const std::string& iface, const std::string& method );

      static std::shared_ptr<CallMessage> create( const std::string& path, const std::string& method );

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

      const char* signature() const;

      bool has_signature( const std::string& signature ) const;

      bool operator == ( const CallMessage& ) const;

      void set_no_reply( bool no_reply=true );

      bool expects_reply() const;

  };

}

#endif

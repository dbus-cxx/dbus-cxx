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
#include <string>

#include <sigc++/sigc++.h>

#include <dbus-cxx/enums.h>
#include <dbus-cxx/pointer.h>
#include <dbus-cxx/accumulators.h>
#include <dbus-cxx/signalmessage.h>

#ifndef DBUSCXX_SIGNALBASE_H
#define DBUSCXX_SIGNALBASE_H

namespace DBus
{
  class Connection;

  /**
   * @defgroup signals Signals
   */

  /**
   * @ingroup signals
   * @ingroup local
   * 
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  
  // TODO fix signals that expect a return value and partially specialize for void returns
  
  class signal_base
  {
    protected:
      
      signal_base(const std::string& path, const std::string& interface, const std::string& name);

      signal_base(const std::string& interface, const std::string& name);

      signal_base(DBusCxxPointer<Connection> connection, const std::string& path, const std::string& interface, const std::string& name);

      signal_base(DBusCxxPointer<Connection> connection, const std::string& interface, const std::string& name);

      signal_base(const signal_base& other);

    public:

      typedef DBusCxxPointer<signal_base> pointer;

      virtual ~signal_base();

      DBusCxxPointer<Connection> connection();

      void set_connection(DBusCxxPointer<Connection> connection);

      const std::string& sender() const;

      void set_sender(const std::string& s);

      const std::string& interface() const;

      void set_interface(const std::string& i);

      const std::string& name() const;

      void set_name( const std::string& n );

      const Path& path() const;

      void set_path(const std::string& s);

      const std::string& destination() const;

      void set_destination(const std::string& s);

      /**
       * This method is needed to be able to create a duplicate of a child
       * capable of parsing their specific template type message.
       */
      virtual pointer clone() = 0;

      /** Returns a DBus XML description of this interface */
      virtual std::string introspect(int space_depth=0) const; 

      virtual std::string arg_name(size_t i);

      virtual void set_arg_name(size_t i, const std::string& name);

    protected:

      DBusCxxWeakPointer<Connection> m_connection;

      std::string m_sender;

      Path m_path;

      std::string m_interface;

      std::string m_name;

      std::string m_destination;

      std::string m_match_rule;

      bool handle_dbus_outgoing( Message::const_pointer );
  };

}

#endif

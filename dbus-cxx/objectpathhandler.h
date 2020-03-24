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
#include <dbus/dbus.h>
#include <memory>
#include <string>
#include "enums.h"
#include "path.h"
#include <sigc++/sigc++.h>

#ifndef DBUSCXX_OBJECTPATHHANDLER_H
#define DBUSCXX_OBJECTPATHHANDLER_H

namespace DBus
{
  class Connection;

  /**
   * Provides an object that handles messages for a given object path on a connection.
   *
   * This class doesn't know anything of methods or signals. It simply knows how
   * to register with a connection as an object on a path.
   *
   * @ingroup local
   * @ingroup objects
   * 
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class ObjectPathHandler: public MessageHandler
  {
    protected:
      ObjectPathHandler(const std::string& path, PrimaryFallback pf);

    public:
      static std::shared_ptr<ObjectPathHandler> create(const std::string& path, PrimaryFallback pf=PrimaryFallback::PRIMARY);
      
      virtual ~ObjectPathHandler();

      /** Returns the path this handler is associated with */
      const Path& path() const;

      /** Returns the mechanism this handler will use to register with a connection */
      PrimaryFallback is_primary_or_fallback();

      /** Returns the connection this handler is registered with */
      std::shared_ptr<Connection> connection() const;

      /** Tries to register the handler using the provided connection and with the currently set path and primary/fallback setting */
      virtual bool register_with_connection(std::shared_ptr<Connection> conn);

      /** Unregisters the handler */
      bool unregister(std::shared_ptr<Connection> conn);

      /** Emitted when this object is registered with a connection */
      sigc::signal<void(std::shared_ptr<Connection>) >& signal_registered();

      /**
       * Emitted when this object is unregistered from a connection
       */
      sigc::signal<void(std::shared_ptr<Connection>) >& signal_unregistered();

    protected:

      std::shared_ptr<Connection> m_connection;

      Path m_path;

      PrimaryFallback m_primary_fallback;

      sigc::signal<void(std::shared_ptr<Connection>) > m_signal_registered;

      sigc::signal<void(std::shared_ptr<Connection>) > m_signal_unregistered;

      static struct DBusObjectPathVTable m_dbus_vtable;

      static DBusHandlerResult message_handler_callback(DBusConnection* connection, DBusMessage* message, void* user_data);

      static void path_unregister_callback(DBusConnection* connection, void* user_data);

  };

}

#endif

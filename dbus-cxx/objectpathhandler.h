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
#include <dbus-cxx/dbus-cxx-config.h>
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
  class CallMessage;

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
      virtual ~ObjectPathHandler();

      /** Returns the path this handler is associated with */
      const Path& path() const;

      /** Returns the mechanism this handler will use to register with a connection */
      PrimaryFallback is_primary_or_fallback();

      /** Returns the connection this handler is registered with */
      std::weak_ptr<Connection> connection() const;

      /**
       * Sets the connection that this ObjectPathHandler is on.
       * Generally, you should not call this, as calling Connection::register_object
       * will do this for you automatically.
       */
      virtual void set_connection( std::shared_ptr<Connection> conn );

      /** Tries to register the handler using the provided connection and with the currently set path and primary/fallback setting */
      virtual bool register_with_connection(std::shared_ptr<Connection> conn);

      /** Unregisters the handler */
      bool unregister();

      /** Emitted when this object is registered with a connection */
      sigc::signal<void(std::shared_ptr<Connection>) >& signal_registered();

      /**
       * Emitted when this object is unregistered from a connection
       */
      sigc::signal<void(std::shared_ptr<Connection>) >& signal_unregistered();

    private:
      class priv_data;

      DBUS_CXX_PROPAGATE_CONST(std::unique_ptr<priv_data>) m_priv;

  };

}

#endif

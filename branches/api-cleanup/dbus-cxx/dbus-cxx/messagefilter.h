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

#ifndef DBUSCXX_MESSAGEFILTER_H
#define DBUSCXX_MESSAGEFILTER_H

namespace DBus
{
  
  /**
   * @ingroup objects
   * @ingroup local
   * 
   * This class provides a common base class for all message filters
   *
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class MessageFilter : public MessageHandler
  {
    protected:
      MessageFilter();

    public:
      
      typedef DBusCxxPointer<MessageFilter> pointer;

      typedef DBusCxxWeakPointer<MessageFilter> weak_pointer;

      virtual ~MessageFilter();

      /** The default implementation simply emits the message signal and returns the result */
//       virtual HandlerResult handle_message( DBusCxxPointer<Connection>, Message::const_pointer );

    protected:

//       virtual FilterResult on_filter_message( DBusCxxPointer<Connection>, Message::const_pointer );

  };

  typedef sigc::signal<FilterResult,DBusCxxPointer<Connection>,Message::pointer>::accumulated<FilterAccumulator> FilterSignal;

}

#endif

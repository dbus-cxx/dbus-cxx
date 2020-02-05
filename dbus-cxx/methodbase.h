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
#include <dbus-cxx/callmessage.h>

#ifndef DBUSCXX_METHODBASE_H
#define DBUSCXX_METHODBASE_H

namespace DBus
{
  class Connection;

  class Interface;

  /**
   * @ingroup local
   * @ingroup objects
   * 
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  
   // TODO fix signals that expect a return value and partially specialize for void returns
  
  class MethodBase
  {
    protected:
      
      MethodBase(const std::string& name);

      MethodBase(const MethodBase& other);

    public:

      typedef DBusCxxPointer<MethodBase> pointer;

      virtual ~MethodBase();

      const std::string& name() const;

      void set_name( const std::string& name );

      virtual HandlerResult handle_call_message( DBusCxxPointer<Connection> connection, CallMessage::const_pointer message ) = 0;

      /**
       * This method is needed to be able to create a duplicate of a child
       * capable of parsing their specific template type message.
       */
      virtual pointer clone() = 0;

      sigc::signal<void,const std::string&/*old name*/, const std::string&/*new name*/> signal_name_changed();

      /** Returns a DBus XML description of this interface */
      virtual std::string introspect(int space_depth=0) const;

      virtual std::string arg_name(size_t i);

      virtual void set_arg_name(size_t i, const std::string& name);

    protected:

      std::string m_name;

      /** Ensures that the name doesn't change while the name changed signal is emitting */
      pthread_mutex_t m_name_mutex;

      sigc::signal<void,const std::string&, const std::string&> m_signal_name_changed;

  };

  /**
   * @example callee_c.cpp
   *
   * This example is part of a series of examples that demonstrate method
   * proxy calls and adapter stubs.
   *
   * This group of examples are:
   * <ul>
   *   <li> Method Adapters
   *   <ul>
   *     <li> @c callee_c.cpp - A dbus object interface created with the dbus C API
   *     <li> @c callee_raw.cpp - Manually handling the loop and callbacks similar to the C API example
   *     <li> @c callee_dispatched.cpp - Dispatched callbacks with manual demarshalling
   *     <li> @c callee.cpp - Dispatched callbacks with automatic demarshalling to a connected slot
   *     <li> @c callee_2.cpp - Similar to callee.cpp but with a void(void) method in the interface
   *     <li> @c callee_object.cpp - Dispatched callbacks with automatic demarshalling to connected slots that are members of a class instance (class knows nothing of DBus)
   *     <li> @c callee_object_inherited.cpp - Dispatched callbacks with automatic demarshalling to a class derived from Object
   *   </ul>
   *   <li> Method Proxies
   *   <ul>
   *     <li> @c caller_c.cpp - Calling a proxy method with the dbus C API
   *     <li> @c caller_raw.cpp - Calling a proxy method with a manual loop similar to the C API example
   *     <li> @c caller_dispatched.cpp - Calling a proxy method using a dispatcher
   *     <li> @c caller_object.cpp - Calling a proxy method of a class derived from ObjectProxy
   *   </ul>
   * </ul>
   *
   * Here is this example:
   *
   */

  /**
   * @example callee_raw.cpp
   *
   * This example is part of a series of examples that demonstrate method
   * proxy calls and adapter stubs.
   *
   * This group of examples are:
   * <ul>
   *   <li> Method Adapters
   *   <ul>
   *     <li> @c callee_c.cpp - A dbus object interface created with the dbus C API
   *     <li> @c callee_raw.cpp - Manually handling the loop and callbacks similar to the C API example
   *     <li> @c callee_dispatched.cpp - Dispatched callbacks with manual demarshalling
   *     <li> @c callee.cpp - Dispatched callbacks with automatic demarshalling to a connected slot
   *     <li> @c callee_2.cpp - Similar to callee.cpp but with a void(void) method in the interface
   *     <li> @c callee_object.cpp - Dispatched callbacks with automatic demarshalling to connected slots that are members of a class instance (class knows nothing of DBus)
   *     <li> @c callee_object_inherited.cpp - Dispatched callbacks with automatic demarshalling to a class derived from Object
   *   </ul>
   *   <li> Method Proxies
   *   <ul>
   *     <li> @c caller_c.cpp - Calling a proxy method with the dbus C API
   *     <li> @c caller_raw.cpp - Calling a proxy method with a manual loop similar to the C API example
   *     <li> @c caller_dispatched.cpp - Calling a proxy method using a dispatcher
   *     <li> @c caller_object.cpp - Calling a proxy method of a class derived from ObjectProxy
   *   </ul>
   * </ul>
   *
   * Here is this example:
   *
   */

  /**
   * @example callee_dispatched.cpp
   *
   * This example is part of a series of examples that demonstrate method
   * proxy calls and adapter stubs.
   *
   * This group of examples are:
   * <ul>
   *   <li> Method Adapters
   *   <ul>
   *     <li> @c callee_c.cpp - A dbus object interface created with the dbus C API
   *     <li> @c callee_raw.cpp - Manually handling the loop and callbacks similar to the C API example
   *     <li> @c callee_dispatched.cpp - Dispatched callbacks with manual demarshalling
   *     <li> @c callee.cpp - Dispatched callbacks with automatic demarshalling to a connected slot
   *     <li> @c callee_2.cpp - Similar to callee.cpp but with a void(void) method in the interface
   *     <li> @c callee_object.cpp - Dispatched callbacks with automatic demarshalling to connected slots that are members of a class instance (class knows nothing of DBus)
   *     <li> @c callee_object_inherited.cpp - Dispatched callbacks with automatic demarshalling to a class derived from Object
   *   </ul>
   *   <li> Method Proxies
   *   <ul>
   *     <li> @c caller_c.cpp - Calling a proxy method with the dbus C API
   *     <li> @c caller_raw.cpp - Calling a proxy method with a manual loop similar to the C API example
   *     <li> @c caller_dispatched.cpp - Calling a proxy method using a dispatcher
   *     <li> @c caller_object.cpp - Calling a proxy method of a class derived from ObjectProxy
   *   </ul>
   * </ul>
   *
   * Here is this example:
   *
   */

  /**
   * @example callee.cpp
   *
   * This example is part of a series of examples that demonstrate method
   * proxy calls and adapter stubs.
   *
   * This group of examples are:
   * <ul>
   *   <li> Method Adapters
   *   <ul>
   *     <li> @c callee_c.cpp - A dbus object interface created with the dbus C API
   *     <li> @c callee_raw.cpp - Manually handling the loop and callbacks similar to the C API example
   *     <li> @c callee_dispatched.cpp - Dispatched callbacks with manual demarshalling
   *     <li> @c callee.cpp - Dispatched callbacks with automatic demarshalling to a connected slot
   *     <li> @c callee_2.cpp - Similar to callee.cpp but with a void(void) method in the interface
   *     <li> @c callee_object.cpp - Dispatched callbacks with automatic demarshalling to connected slots that are members of a class instance (class knows nothing of DBus)
   *     <li> @c callee_object_inherited.cpp - Dispatched callbacks with automatic demarshalling to a class derived from Object
   *   </ul>
   *   <li> Method Proxies
   *   <ul>
   *     <li> @c caller_c.cpp - Calling a proxy method with the dbus C API
   *     <li> @c caller_raw.cpp - Calling a proxy method with a manual loop similar to the C API example
   *     <li> @c caller_dispatched.cpp - Calling a proxy method using a dispatcher
   *     <li> @c caller_object.cpp - Calling a proxy method of a class derived from ObjectProxy
   *   </ul>
   * </ul>
   *
   * Here is this example:
   *
   */

  /**
   * @example callee_2.cpp
   *
   * This example is part of a series of examples that demonstrate method
   * proxy calls and adapter stubs.
   *
   * This group of examples are:
   * <ul>
   *   <li> Method Adapters
   *   <ul>
   *     <li> @c callee_c.cpp - A dbus object interface created with the dbus C API
   *     <li> @c callee_raw.cpp - Manually handling the loop and callbacks similar to the C API example
   *     <li> @c callee_dispatched.cpp - Dispatched callbacks with manual demarshalling
   *     <li> @c callee.cpp - Dispatched callbacks with automatic demarshalling to a connected slot
   *     <li> @c callee_2.cpp - Similar to callee.cpp but with a void(void) method in the interface
   *     <li> @c callee_object.cpp - Dispatched callbacks with automatic demarshalling to connected slots that are members of a class instance (class knows nothing of DBus)
   *     <li> @c callee_object_inherited.cpp - Dispatched callbacks with automatic demarshalling to a class derived from Object
   *   </ul>
   *   <li> Method Proxies
   *   <ul>
   *     <li> @c caller_c.cpp - Calling a proxy method with the dbus C API
   *     <li> @c caller_raw.cpp - Calling a proxy method with a manual loop similar to the C API example
   *     <li> @c caller_dispatched.cpp - Calling a proxy method using a dispatcher
   *     <li> @c caller_object.cpp - Calling a proxy method of a class derived from ObjectProxy
   *   </ul>
   * </ul>
   *
   * Here is this example:
   *
   */

  /**
   * @example callee_object.cpp
   *
   * This example is part of a series of examples that demonstrate method
   * proxy calls and adapter stubs.
   *
   * This group of examples are:
   * <ul>
   *   <li> Method Adapters
   *   <ul>
   *     <li> @c callee_c.cpp - A dbus object interface created with the dbus C API
   *     <li> @c callee_raw.cpp - Manually handling the loop and callbacks similar to the C API example
   *     <li> @c callee_dispatched.cpp - Dispatched callbacks with manual demarshalling
   *     <li> @c callee.cpp - Dispatched callbacks with automatic demarshalling to a connected slot
   *     <li> @c callee_2.cpp - Similar to callee.cpp but with a void(void) method in the interface
   *     <li> @c callee_object.cpp - Dispatched callbacks with automatic demarshalling to connected slots that are members of a class instance (class knows nothing of DBus)
   *     <li> @c callee_object_inherited.cpp - Dispatched callbacks with automatic demarshalling to a class derived from Object
   *   </ul>
   *   <li> Method Proxies
   *   <ul>
   *     <li> @c caller_c.cpp - Calling a proxy method with the dbus C API
   *     <li> @c caller_raw.cpp - Calling a proxy method with a manual loop similar to the C API example
   *     <li> @c caller_dispatched.cpp - Calling a proxy method using a dispatcher
   *     <li> @c caller_object.cpp - Calling a proxy method of a class derived from ObjectProxy
   *   </ul>
   * </ul>
   *
   * Here is this example:
   *
   */

  /**
   * @example callee_object_inherited.cpp
   *
   * This example is part of a series of examples that demonstrate method
   * proxy calls and adapter stubs.
   *
   * This group of examples are:
   * <ul>
   *   <li> Method Adapters
   *   <ul>
   *     <li> @c callee_c.cpp - A dbus object interface created with the dbus C API
   *     <li> @c callee_raw.cpp - Manually handling the loop and callbacks similar to the C API example
   *     <li> @c callee_dispatched.cpp - Dispatched callbacks with manual demarshalling
   *     <li> @c callee.cpp - Dispatched callbacks with automatic demarshalling to a connected slot
   *     <li> @c callee_2.cpp - Similar to callee.cpp but with a void(void) method in the interface
   *     <li> @c callee_object.cpp - Dispatched callbacks with automatic demarshalling to connected slots that are members of a class instance (class knows nothing of DBus)
   *     <li> @c callee_object_inherited.cpp - Dispatched callbacks with automatic demarshalling to a class derived from Object
   *   </ul>
   *   <li> Method Proxies
   *   <ul>
   *     <li> @c caller_c.cpp - Calling a proxy method with the dbus C API
   *     <li> @c caller_raw.cpp - Calling a proxy method with a manual loop similar to the C API example
   *     <li> @c caller_dispatched.cpp - Calling a proxy method using a dispatcher
   *     <li> @c caller_object.cpp - Calling a proxy method of a class derived from ObjectProxy
   *   </ul>
   * </ul>
   *
   * Here is this example:
   *
   */

  /**
   * @example caller_c.cpp
   *
   * This example is part of a series of examples that demonstrate method
   * proxy calls and adapter stubs.
   *
   * This group of examples are:
   * <ul>
   *   <li> Method Adapters
   *   <ul>
   *     <li> @c callee_c.cpp - A dbus object interface created with the dbus C API
   *     <li> @c callee_raw.cpp - Manually handling the loop and callbacks similar to the C API example
   *     <li> @c callee_dispatched.cpp - Dispatched callbacks with manual demarshalling
   *     <li> @c callee.cpp - Dispatched callbacks with automatic demarshalling to a connected slot
   *     <li> @c callee_2.cpp - Similar to callee.cpp but with a void(void) method in the interface
   *     <li> @c callee_object.cpp - Dispatched callbacks with automatic demarshalling to connected slots that are members of a class instance (class knows nothing of DBus)
   *     <li> @c callee_object_inherited.cpp - Dispatched callbacks with automatic demarshalling to a class derived from Object
   *   </ul>
   *   <li> Method Proxies
   *   <ul>
   *     <li> @c caller_c.cpp - Calling a proxy method with the dbus C API
   *     <li> @c caller_raw.cpp - Calling a proxy method with a manual loop similar to the C API example
   *     <li> @c caller_dispatched.cpp - Calling a proxy method using a dispatcher
   *     <li> @c caller_object.cpp - Calling a proxy method of a class derived from ObjectProxy
   *   </ul>
   * </ul>
   *
   * Here is this example:
   *
   */

  /**
   * @example caller_raw.cpp
   *
   * This example is part of a series of examples that demonstrate method
   * proxy calls and adapter stubs.
   *
   * This group of examples are:
   * <ul>
   *   <li> Method Adapters
   *   <ul>
   *     <li> @c callee_c.cpp - A dbus object interface created with the dbus C API
   *     <li> @c callee_raw.cpp - Manually handling the loop and callbacks similar to the C API example
   *     <li> @c callee_dispatched.cpp - Dispatched callbacks with manual demarshalling
   *     <li> @c callee.cpp - Dispatched callbacks with automatic demarshalling to a connected slot
   *     <li> @c callee_2.cpp - Similar to callee.cpp but with a void(void) method in the interface
   *     <li> @c callee_object.cpp - Dispatched callbacks with automatic demarshalling to connected slots that are members of a class instance (class knows nothing of DBus)
   *     <li> @c callee_object_inherited.cpp - Dispatched callbacks with automatic demarshalling to a class derived from Object
   *   </ul>
   *   <li> Method Proxies
   *   <ul>
   *     <li> @c caller_c.cpp - Calling a proxy method with the dbus C API
   *     <li> @c caller_raw.cpp - Calling a proxy method with a manual loop similar to the C API example
   *     <li> @c caller_dispatched.cpp - Calling a proxy method using a dispatcher
   *     <li> @c caller_object.cpp - Calling a proxy method of a class derived from ObjectProxy
   *   </ul>
   * </ul>
   *
   * Here is this example:
   *
   */

  /**
   * @example caller_dispatched.cpp
   *
   * This example is part of a series of examples that demonstrate method
   * proxy calls and adapter stubs.
   *
   * This group of examples are:
   * <ul>
   *   <li> Method Adapters
   *   <ul>
   *     <li> @c callee_c.cpp - A dbus object interface created with the dbus C API
   *     <li> @c callee_raw.cpp - Manually handling the loop and callbacks similar to the C API example
   *     <li> @c callee_dispatched.cpp - Dispatched callbacks with manual demarshalling
   *     <li> @c callee.cpp - Dispatched callbacks with automatic demarshalling to a connected slot
   *     <li> @c callee_2.cpp - Similar to callee.cpp but with a void(void) method in the interface
   *     <li> @c callee_object.cpp - Dispatched callbacks with automatic demarshalling to connected slots that are members of a class instance (class knows nothing of DBus)
   *     <li> @c callee_object_inherited.cpp - Dispatched callbacks with automatic demarshalling to a class derived from Object
   *   </ul>
   *   <li> Method Proxies
   *   <ul>
   *     <li> @c caller_c.cpp - Calling a proxy method with the dbus C API
   *     <li> @c caller_raw.cpp - Calling a proxy method with a manual loop similar to the C API example
   *     <li> @c caller_dispatched.cpp - Calling a proxy method using a dispatcher
   *     <li> @c caller_object.cpp - Calling a proxy method of a class derived from ObjectProxy
   *   </ul>
   * </ul>
   *
   * Here is this example:
   *
   */

  /**
   * @example caller_object.cpp
   *
   * This example is part of a series of examples that demonstrate method
   * proxy calls and adapter stubs.
   *
   * This group of examples are:
   * <ul>
   *   <li> Method Adapters
   *   <ul>
   *     <li> @c callee_c.cpp - A dbus object interface created with the dbus C API
   *     <li> @c callee_raw.cpp - Manually handling the loop and callbacks similar to the C API example
   *     <li> @c callee_dispatched.cpp - Dispatched callbacks with manual demarshalling
   *     <li> @c callee.cpp - Dispatched callbacks with automatic demarshalling to a connected slot
   *     <li> @c callee_2.cpp - Similar to callee.cpp but with a void(void) method in the interface
   *     <li> @c callee_object.cpp - Dispatched callbacks with automatic demarshalling to connected slots that are members of a class instance (class knows nothing of DBus)
   *     <li> @c callee_object_inherited.cpp - Dispatched callbacks with automatic demarshalling to a class derived from Object
   *   </ul>
   *   <li> Method Proxies
   *   <ul>
   *     <li> @c caller_c.cpp - Calling a proxy method with the dbus C API
   *     <li> @c caller_raw.cpp - Calling a proxy method with a manual loop similar to the C API example
   *     <li> @c caller_dispatched.cpp - Calling a proxy method using a dispatcher
   *     <li> @c caller_object.cpp - Calling a proxy method of a class derived from ObjectProxy
   *   </ul>
   * </ul>
   *
   * Here is this example:
   *
   */


}

#endif

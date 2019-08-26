/***************************************************************************
 *   Copyright (C) 2009,2010 by Rick L. Vinyard, Jr.                       *
 *   rvinyard@cs.nmsu.edu                                                  *
 *   Copyright (C) 2019 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
 *                                                                         *
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
#include <dbus-cxx/errormessage.h>
#include <dbus-cxx/dbus-cxx-config.h>
#include <dbus-cxx/headerlog.h>
#include <dbus-cxx/returnmessage.h>
#include <type_traits>
#include <mutex>
#include <tuple>
#include <sstream>

#ifndef DBUSCXX_METHODBASE_H
#define DBUSCXX_METHODBASE_H

namespace DBus
{

namespace priv{
   template<typename... argn> class method_sig;

   template<> class method_sig<>{
   public:
   std::string sigg(const std::vector<std::string>&, int idx, const std::string& spaces ) const {
     return "";
   }
   };

   template<typename arg1, typename... argn> 
   class method_sig<arg1, argn...> : public method_sig<argn...> {
   public:
   std::string sigg(const std::vector<std::string>& names, int idx, const std::string& spaces ) const{
     arg1 arg;
     std::ostringstream output;
     std::string name = names.size() < idx ? names[idx] : "";
     output << spaces;
     output << "<arg name=\"";
     output << name << "\" type=\"";
     output << signature(arg);
     output << "\" ";
     output << "direction=\"in\"/>\n";
     output << method_sig<argn...>::sigg(names, idx + 1, spaces);
     return output.str();
   }
  };

   template<typename... template_argn> class template_sig;

   template<> class template_sig<>{
   public:
   std::string sigg() const {
     return "";
   }
   };

   template<typename template_arg1, typename... template_argn> 
   class template_sig<template_arg1, template_argn...> : public template_sig<template_argn...> {
   public:
   std::string sigg() const{
     std::string arg1_name = typeid(template_arg1).name();
     std::string remaining_args = template_sig<template_argn...>::sigg();
     if( remaining_args.size() > 1 ){
         arg1_name += ",";
     }
     return arg1_name + remaining_args;
   }
   };

} /* namespace priv */

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

    private:
      uint32_t sendMessage( std::shared_ptr<Connection> connection, const Message::const_pointer );

    public:

      typedef std::shared_ptr<MethodBase> pointer;

      ~MethodBase();

      const std::string& name() const;

      void set_name( const std::string& name );


      virtual HandlerResult handle_call_message( DBusCxxPointer<Connection> connection, CallMessage::const_pointer message ) = 0;

      /**
       * This method is needed to be able to create a duplicate of a child
       * capable of parsing their specific template type message.
       */
      //TODO is this needed anymore?
      //virtual pointer clone() = 0;

      sigc::signal<void(const std::string&/*old name*/, const std::string&/*new name*/)> signal_name_changed();

      /** Returns a DBus XML description of this interface */
      virtual std::string introspect(int space_depth=0) const { return std::string(); };

      std::string arg_name(size_t i) const;

      void set_arg_name(size_t i, const std::string& name);

    protected:

      std::string m_name;

      /** Ensures that the name doesn't change while the name changed signal is emitting */
      std::mutex m_name_mutex;

      sigc::signal<void(const std::string&, const std::string&)> m_signal_name_changed;

      std::vector<std::string> m_arg_names;

  };

  template <class T_return, class... T_arg>
  class Method : public MethodBase {
  private:
      template <class Tuple, size_t... Is>
      void call_slot(Tuple t, std::index_sequence<Is...> ){
          m_slot( std::get<Is>(t)... );
      }

  public:
      void set_method( sigc::signal<T_return(T_arg...)> slot ){ m_slot = slot; }

      virtual std::string introspect(int space_depth=0) const {
          std::ostringstream sout;
          std::string spaces;
          DBus::priv::method_sig<T_arg...> method_sig_gen;
          for(int i = 0; i < space_depth; i++ ) spaces += " ";
          sout << spaces << "<method name=\"" << name() << "\">\n";
          if( std::is_same<void,T_return>::value == false && std::is_same<std::any,T_return>::value == false ){
              T_return ret_type;
              sout << spaces << "<arg name=\"" << arg_name(0) << "\" "
                   << "type=\"" << signature(ret_type) << "\" "
                   << "direction=\"out\"/>\n";
          }
          sout << method_sig_gen.sigg( m_arg_names, 1, spaces );
          return sout.str();
      }

      virtual HandlerResult handle_call_message( DBusCxxPointer<Connection> connection, CallMessage::const_pointer message ){
          std::ostringstream debug_str;
          std::tuple<T_arg...> args;

          debug_str << "DBus::Method<";
          debug_str << DBus::priv::template_sig<T_return>().sigg();
          debug_str << ",";
          debug_str << DBus::priv::template_sig<T_arg...>().sigg();
          debug_str << ">::handle_call_message method=";
          debug_str << name();
          DBUSCXX_DEBUG_STDSTR( "dbus.Method", debug_str.str() );

          if( !connection || !message ) return NOT_HANDLED;

          try{
              Message::iterator i = message->begin();
              std::apply( [i](auto ...arg){
                     (i >> ... >> arg);
                 },
                 args );
              if( !i.is_valid() ){
                  return NOT_HANDLED;
              }
          }catch( ErrorInvalidTypecast ){
              return NOT_HANDLED;
          }

          try{
            ReturnMessage::pointer retmsg = message->create_reply();
            if( !retmsg ) return NOT_HANDLED;

            if( std::is_same<void,T_return>::value == false ){
                T_return retval;
                //TODO call slot here and get the return value
                //retval = m_slot(val1....);
                *retmsg << retval;
            }else{
                call_slot( args, std::index_sequence_for<T_arg...>{} );
            }

            sendMessage( connection, retmsg );
         }catch( const std::exception &e ){
            ErrorMessage::pointer errmsg = ErrorMessage::create( message, DBUS_ERROR_FAILED, e.what() );

            if( !errmsg ) return NOT_HANDLED;

            sendMessage( connection, errmsg );
         }catch( ... ){
            std::ostringstream stream;
            stream << "DBus-cxx " << DBUS_CXX_PACKAGE_MAJOR_VERSION << "."
                   << DBUS_CXX_PACKAGE_MINOR_VERSION << "."
                   << DBUS_CXX_PACKAGE_MICRO_VERSION
                   << ": unknown error(uncaught exception)";
            ErrorMessage::pointer errmsg = ErrorMessage::create( message, DBUS_ERROR_FAILED, stream.str() );

            if( !errmsg ) return NOT_HANDLED;

            sendMessage( connection, errmsg );
         }

         return HANDLED;
      }


    protected:
      sigc::signal<T_return(T_arg...)> m_slot;
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

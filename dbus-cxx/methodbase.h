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
#include <exception>
#include <stdint.h>
#include <dbus-cxx/callmessage.h>
#include <dbus-cxx/dbus-cxx-config.h>
#include <dbus-cxx/errormessage.h>
#include <dbus-cxx/headerlog.h>
#include <dbus-cxx/utility.h>
#include <stddef.h>
#include <functional>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>
#include <dbus/dbus.h>
#include "enums.h"
#include "error.h"
#include <sigc++/sigc++.h>

#ifndef DBUSCXX_METHODBASE_H
#define DBUSCXX_METHODBASE_H

namespace DBus
{

  class Connection;
  class Message;
  class ReturnMessage;

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

    protected:
      uint32_t sendMessage( std::shared_ptr<Connection> connection, const std::shared_ptr<const Message> );

    public:
      ~MethodBase();

      const std::string& name() const;

      void set_name( const std::string& name );


      virtual HandlerResult handle_call_message( std::shared_ptr<Connection> connection, std::shared_ptr<const CallMessage> message ) = 0;

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

  template <typename T_type>
  class Method : public MethodBase {
  private:
/*
      template <class Tuple, size_t... Is>
      void call_slot(Tuple t, std::index_sequence<Is...> ){
          m_slot( std::get<Is>(t)... );
      }
*/
      Method(const std::string& name) : MethodBase(name){}

  public:
      static std::shared_ptr<Method<T_type>> create(const std::string& name){
          return std::shared_ptr<Method<T_type>>( new Method<T_type>(name) );
      }

      void set_method( sigc::slot<T_type> slot ){ m_slot = slot; }

      virtual std::string introspect(int space_depth=0) const {
          std::ostringstream sout;
          std::string spaces;
          DBus::priv::dbus_function_traits<std::function<T_type>> method_sig_gen;
          for(int i = 0; i < space_depth; i++ ) spaces += " ";
          sout << spaces << "<method name=\"" << name() << "\">\n";
          sout << method_sig_gen.introspect(m_arg_names, 0, spaces + "  " );
          sout << spaces << "</method>\n";
          return sout.str();
      }

      virtual HandlerResult handle_call_message( std::shared_ptr<Connection> connection, std::shared_ptr<const CallMessage> message ){
          std::ostringstream debug_str;
          DBus::priv::dbus_function_traits<std::function<T_type>> method_sig_gen;

          debug_str << "DBus::Method<";
          debug_str << method_sig_gen.debug_string();
          debug_str << ">::handle_call_message method=";
          debug_str << name();
          DBUSCXX_DEBUG_STDSTR( "dbus.Method", debug_str.str() );

          if( !connection || !message ) return HandlerResult::NOT_HANDLED;

          try{
              std::shared_ptr<ReturnMessage> retmsg = message->create_reply();
              if( !retmsg ) return HandlerResult::NOT_HANDLED;

              method_sig_gen.extractAndCall(message, retmsg, m_slot );

              sendMessage( connection, retmsg );
          }catch( ErrorInvalidTypecast ){
              return HandlerResult::NOT_HANDLED;
         }catch( const std::exception &e ){
            std::shared_ptr<ErrorMessage> errmsg = ErrorMessage::create( message, DBUS_ERROR_FAILED, e.what() );

            if( !errmsg ) return HandlerResult::NOT_HANDLED;

            sendMessage( connection, errmsg );
         }catch( ... ){
            std::ostringstream stream;
            stream << "DBus-cxx " << DBUS_CXX_PACKAGE_MAJOR_VERSION << "."
                   << DBUS_CXX_PACKAGE_MINOR_VERSION << "."
                   << DBUS_CXX_PACKAGE_MICRO_VERSION
                   << ": unknown error(uncaught exception)";
            std::shared_ptr<ErrorMessage> errmsg = ErrorMessage::create( message, DBUS_ERROR_FAILED, stream.str() );

            if( !errmsg ) return HandlerResult::NOT_HANDLED;

            sendMessage( connection, errmsg );
         }

         return HandlerResult::HANDLED;
      }


    protected:
      sigc::slot<T_type> m_slot;
  };

}

#endif

/***************************************************************************
 *   Copyright (C) 2009,2010 by Rick L. Vinyard, Jr.                       *
 *   rvinyard@cs.nmsu.edu                                                  *
 *   Copyright (C) 2019 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
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
#include <dbus-cxx/headerlog.h>
#include <dbus-cxx/utility.h>
#include <memory>
#include <mutex>
#include <string>
#include <sstream>
#include "messageappenditerator.h"
#include <sigc++/sigc++.h>

#ifndef DBUSCXX_METHODPROXYBASE_H
#define DBUSCXX_METHODPROXYBASE_H

namespace DBus
{

  class InterfaceProxy;
  class PendingCall;
  class ReturnMessage;
  template <typename signature> class MethodProxy;

  /**
   * Base class for all proxy(remote) methods.  This class allows remote methods
   * to be used in a generic manner, as the MethodProxy class is templated and
   * thus cannot be used in a generic manner(e.g. stored in a vector).
   *
   * @ingroup objects
   * @ingroup proxy
   * 
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class MethodProxyBase
  {
    protected:

      MethodProxyBase( const std::string& name );
      
      MethodProxyBase(const MethodProxyBase& other);

    public:
      static std::shared_ptr<MethodProxyBase> create( const std::string& name );

      ~MethodProxyBase();

      InterfaceProxy* interface() const;

      const std::string& name() const;

      void set_name( const std::string& name );

      std::shared_ptr<CallMessage> create_call_message( ) const;

      std::shared_ptr<const ReturnMessage> call( std::shared_ptr<const CallMessage>, int timeout_milliseconds=-1 ) const;

      //TODO make a call() function that takes in a builder(for the milliseconds)

      std::shared_ptr<PendingCall> call_async( std::shared_ptr<const CallMessage>, int timeout_milliseconds=-1 ) const;

      sigc::signal<void(const std::string&/*old name*/, const std::string&/*new name*/)> signal_name_changed();

    protected:

      // Declare InterfaceProxy as a friend so that it can set the interface
      friend class InterfaceProxy;

      InterfaceProxy* m_interface;

      std::string m_name;

      /** Ensures that the name doesn't change while the name changed signal is emitting */
      std::mutex m_name_mutex;

      sigc::signal<void(const std::string&, const std::string&)> m_signal_name_changed;

  };


  /**
   * MethodProxy specialization for void return type.
   *
   * @param T_arg Method arguments
   */
  template<typename... T_arg>
  class MethodProxy<void(T_arg...)> : public MethodProxyBase
  {
  protected:
      MethodProxy(const std::string& name) :
        MethodProxyBase(name){}

  public:
    void operator()(T_arg... args){
        std::ostringstream debug_str;
        DBus::priv::dbus_function_traits<std::function<void(T_arg...)>> method_sig_gen;

        debug_str << "DBus::MethodProxy<";
        debug_str << method_sig_gen.debug_string();
        debug_str << "> calling method=";
        debug_str << name();
        DBUSCXX_DEBUG_STDSTR( "dbus.MethodProxy", debug_str.str() );

        std::shared_ptr<CallMessage> _callmsg = this->create_call_message();
        (*_callmsg << ... << args);
        std::shared_ptr<const ReturnMessage> retmsg = this->call( _callmsg, -1 );
    }

    static std::shared_ptr<MethodProxy> create(const std::string& name){
      return std::shared_ptr<MethodProxy>( new MethodProxy(name) );
    }
  };

  /**
   * MethodProxy specialization for non-void return type.
   *
   * @param T_return Return type(not void)
   * @param T_arg Method arguments
   */
  template<typename T_return, typename... T_arg>
  class MethodProxy<T_return(T_arg...)> : public MethodProxyBase
  {
  protected:
      MethodProxy(const std::string& name) :
        MethodProxyBase(name){}

  public:
    T_return operator()(T_arg... args){
        std::ostringstream debug_str;
        DBus::priv::dbus_function_traits<std::function<T_return(T_arg...)>> method_sig_gen;

        debug_str << "DBus::MethodProxy<";
        debug_str << method_sig_gen.debug_string();
        debug_str << "> calling method=";
        debug_str << name();
        DBUSCXX_DEBUG_STDSTR( "dbus.MethodProxy", debug_str.str() );

        std::shared_ptr<CallMessage> _callmsg = this->create_call_message();
        MessageAppendIterator iter = _callmsg->append();
        (iter << ... << args);
        std::shared_ptr<const ReturnMessage> retmsg = this->call( _callmsg, -1 );
        T_return _retval;
        retmsg >> _retval;
        return _retval;
    }

    static std::shared_ptr<MethodProxy> create(const std::string& name){
      return std::shared_ptr<MethodProxy>( new MethodProxy(name) );
    }
  };

}

#endif

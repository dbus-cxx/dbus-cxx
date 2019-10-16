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
#include <string>

#include <sigc++/sigc++.h>

#include <mutex>
#include <memory>

#include <dbus-cxx/enums.h>
#include <dbus-cxx/forward_decls.h>
#include <dbus-cxx/accumulators.h>
#include <dbus-cxx/callmessage.h>
#include <dbus-cxx/returnmessage.h>
#include <dbus-cxx/pendingcall.h>

#ifndef DBUSCXX_METHODPROXYBASE_H
#define DBUSCXX_METHODPROXYBASE_H

namespace DBus
{
  /**
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

      typedef std::shared_ptr<MethodProxyBase> pointer;

      static pointer create( const std::string& name );

      ~MethodProxyBase();

      InterfaceProxy* interface() const;

      const std::string& name() const;

      void set_name( const std::string& name );

      CallMessage::pointer create_call_message( ) const;

      ReturnMessage::const_pointer call( CallMessage::const_pointer, int timeout_milliseconds=-1 ) const;

      //TODO make a call() function that takes in a builder(for the milliseconds)
/*
      T_return call( T_arg... args )
      {
        CallMessage::pointer _callmsg = this->create_call_message();
        (*_callmsg << ... << args);
        ReturnMessage::const_pointer retmsg = this->call( _callmsg, -1 );
        T_return _retval;
        retmsg >> _retval;
        return _retval;
      }
*/

/*
      template<>
      void call( T_arg... args )
      {
        CallMessage::pointer _callmsg = this->create_call_message();
        (*_callmsg << ... << args);
        this->call( _callmsg, -1 );
      }
*/

      PendingCall::pointer call_async( CallMessage::const_pointer, int timeout_milliseconds=-1 ) const;

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

  template <class T_return, class... T_arg>
  class MethodProxy : public MethodProxyBase
  {
  protected:
      MethodProxy(const std::string& name) :
        MethodProxyBase(name){}

  public:
    typedef std::shared_ptr<MethodProxy> pointer;

    T_return operator()(T_arg... args){
        //TODO add debug here(see methodbase.h)
        //DBUSCXX_DEBUG_STDSTR( "dbus.MethodProxy", 
        CallMessage::pointer _callmsg = this->create_call_message();
        (*_callmsg << ... << args);
        ReturnMessage::const_pointer retmsg = this->call( _callmsg, -1 );
        T_return _retval;
        retmsg >> _retval;
        return _retval;
    }

    static pointer create(const std::string& name){
      return pointer( new MethodProxy(name) );
    }
  };

}

#endif

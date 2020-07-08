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
#include <dbus-cxx/signal_base.h>
#include <dbus-cxx/signalmessage.h>
#include <dbus-cxx/utility.h>
#include <functional>
#include <memory>
#include <string>
#include <tuple>
#include "enums.h"
#include "error.h"
#include "headerlog.h"
#include "messageiterator.h"
#include <sigc++/sigc++.h>

#ifndef DBUSCXX_SIGNALPROXYBASE_H
#define DBUSCXX_SIGNALPROXYBASE_H

namespace DBus
{
  struct MessageHandlerAccumulator;

/**
 * A builder class to build up a match rule for a signal.  Define
 * the particular parts of the signal that you want to listen for,
 * and then pass it into the signal_proxy.
 */
class SignalMatchRule {
  public:
    SignalMatchRule();

    SignalMatchRule& setPath( const std::string& path );

    SignalMatchRule& setInterface( const std::string& interface_name );

    SignalMatchRule& setMember( const std::string& member );

    SignalMatchRule& setSender( const std::string& sender );

    SignalMatchRule& setDestination( const std::string& destination );

    std::string getMatchRule() const;

    std::string getPath() const;

    std::string getInterface() const;

    std::string getMember() const;

    static SignalMatchRule create();

  private:
    std::string m_path;
    std::string m_interface;
    std::string m_member;
    std::string m_sender;
    std::string m_destination;
};

  /**
   * Base class for a signal proxy that allows you to listen for signals.  This is required
   * to use signals in a generic manner, as the subclasses are all templated.
   *
   * @ingroup proxy
   * @ingroup signals
   * 
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class signal_proxy_base: public signal_base
  {
    public:
      virtual HandlerResult handle_signal( std::shared_ptr<const SignalMessage> );

      const std::string& match_rule() const;

    protected:
      signal_proxy_base(const SignalMatchRule& matchRule);

      virtual ~signal_proxy_base();

      bool matches(std::shared_ptr<const SignalMessage> msg);

      /**
       * This method is needed to be able to create a duplicate of a child
       * capable of parsing their specific template type message.
       */
//      virtual std::shared_ptr<signal_base> clone() = 0;

      virtual HandlerResult on_dbus_incoming( std::shared_ptr<const SignalMessage> msg ) = 0;

  private:
      class priv_data;

      DBUS_CXX_PROPAGATE_CONST(std::unique_ptr<priv_data>) m_priv;
  };

/**
 * Subclass of the sigc::signal templates.
 * A signal_proxy allows you to listen for signals that are emitted by other applications on the DBus.
 * Note that because DBus signals never have a return type, neither can the handling function.
 *
 * @ingroup signals
 * @ingroup proxy
 *
 * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
 */
template <class... T_arg>
class signal_proxy
  : public sigc::signal<void(T_arg...)>, public signal_proxy_base
{
  public:
    signal_proxy(const SignalMatchRule& matchRule):
      signal_proxy_base(matchRule)
    {  }

    static std::shared_ptr<signal_proxy> create(const SignalMatchRule& matchRule)
    { return std::shared_ptr<signal_proxy>( new signal_proxy(matchRule) ); }

//    virtual std::shared_ptr<signal_base> clone()
//    { return std::shared_ptr<signal_base>( new signal_proxy(*this) ); }

  protected:
    HandlerResult on_dbus_incoming( std::shared_ptr<const SignalMessage> msg )
    {
      std::tuple<T_arg...> tup_args;
      std::ostringstream debug_str;
      DBus::priv::dbus_function_traits<std::function<void(T_arg...)>> method_sig_gen;

      debug_str << "DBus::signal_proxy<";
      debug_str << method_sig_gen.debug_string();
      debug_str << ">::on_dbus_incoming method=";
      debug_str << msg->member();
      DBUSCXX_DEBUG_STDSTR( "DBus.signal_proxy", debug_str.str() );

      try {
        MessageIterator i = msg->begin();
        std::apply( [i](auto&& ...arg) mutable {
               (void)(i >> ... >> arg);
              },
        tup_args );
        std::apply(&signal_proxy::emit, std::tuple_cat(std::make_tuple(this), tup_args) );
      }catch ( ErrorInvalidTypecast& e ) {
          DBUSCXX_DEBUG_STDSTR( "DBus.signal_proxy", "Caught error invalid typecast" );
          return HandlerResult::Not_Handled;
      }catch( ... ){
          DBUSCXX_DEBUG_STDSTR( "DBus.signal_proxy", "Unknown exception" );
          return HandlerResult::Not_Handled;
      }
    
      return HandlerResult::Handled;
    }

};

}

#endif

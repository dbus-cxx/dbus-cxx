/***************************************************************************
 *   Copyright (C) 2007,2008,2009,2010 by Rick L. Vinyard, Jr.             *
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

#include <memory>
#include <dbus/dbus.h>
#include <dbus-cxx/simplelogger_defs.h>
#include <dbus-cxx/message.h>
#include <dbus-cxx/returnmessage.h>
#include <dbus-cxx/messageiterator.h>
#include <functional>
#include <sstream>
#include <cxxabi.h>

#ifndef DBUSCXX_UTILITY_H
#define DBUSCXX_UTILITY_H

#define DBUS_CXX_INTROSPECTABLE_INTERFACE "org.freedesktop.DBus.Introspectable"

/**
 * \def DBUS_CXX_ITERATOR_SUPPORT(CppType,DBusType)
 * Provides support for unsupported types that can be cast to DBus supported types
 *
 * This macro provides message iterator and introspection support for types that are
 * not supported by the DBus native types, but can be \c static_cast<> to a DBus
 * native type. Typically, this is an enum.
 *
 * @param CppType The unsupported type. Must be able to static_cast<> to DBusType
 *
 * @param DBusType One of the types inherently supported by dbus-cxx
 *
 * Example supporting an enum as a 32-bit int:
 * \code
 * typedef MyEnum { ZERO, ONE, TWO, THREE } MyEnum;
 * 
 * DBUS_CXX_ITERATOR_SUPPORT( MyEnum, uint32_t )
 * \endcode
 */
#define DBUS_CXX_ITERATOR_SUPPORT( CppType, DBusType )                                                \
  inline                                                                                              \
  DBus::MessageIterator& operator>>(DBus::MessageIterator& __msgiter, CppType& __cpptype)             \
  {                                                                                                   \
    DBusType __dbustype;                                                                              \
    __msgiter >> __dbustype;                                                                          \
    __cpptype = static_cast< CppType >( __dbustype );                                                 \
    return __msgiter;                                                                                 \
  }                                                                                                   \
                                                                                                      \
  inline                                                                                              \
  DBus::MessageAppendIterator& operator<<(DBus::MessageAppendIterator& __msgiter, CppType& __cpptype) \
  {                                                                                                   \
    __msgiter << static_cast< DBusType >( __cpptype );                                                \
    return __msgiter;                                                                                 \
  }                                                                                                   \
                                                                                                      \
  namespace DBus {                                                                                    \
    inline std::string signature( CppType ) { DBusType d; return signature( d ); }          \
  }


namespace DBus
{

  /**
   * Initializes the dbus-cxx library
   *
   * @param threadsafe If \c true the library's threadsafe structures, along with the underlying dbus library will be initialized to support threadsafe operations. This makes the library threadsafe at the cost of efficiency.
   */
  void init(bool threadsafe=true);

  /**
   * Get the initialization state of the dbus-cxx library
   *
   * @return \c true if the library is initialized, \c false otherwise
   */
  bool initialized();

  /**
   * Set the callback function that is used for printing log messages.
   * Set this to either your provided function, or use the built-in function DBus::logStdErr.
   */
  void setLoggingFunction( ::simplelogger_log_function function );

  /**
   * Log messages to stderr(std::cerr).
   *
   * Format is: [thread-id] [logger-name] [level] - message(file:line)
   */
  void logStdErr( const char* logger_name, const struct ::SL_LogLocation* location,
      const enum ::SL_LogLevel level,
      const char* log_string );

  /**
   * When used in conjunction with DBus::logStdErr, will only print out log messages above the set level.
   * By default, this is set to SL_INFO
   */
  void setLogLevel( const enum ::SL_LogLevel level );

namespace priv {
/*
 * dbus_signature class - signature of a given type
 */
template<typename... argn>
class dbus_signature;
 
template<> class dbus_signature<>{
public:
  std::string dbus_sig() const {
    return "";
  }
};
 
template<typename arg1, typename... argn>
class dbus_signature<arg1, argn...> : public dbus_signature<argn...> {
public:
  std::string dbus_sig() const {
    arg1 arg;
    return signature(arg) + dbus_signature<argn...>::dbus_sig();
  }
};

/*
 * method_signature class - like dbus_signature, but outputs the args of the signature
 * in a C++-like manner
 */
template<typename... argn>
class method_signature;

template<> class method_signature<>{
public:
  std::string method_sig() const {
    return "";
  }

  std::string introspect(const std::vector<std::string>&, int, const std::string&) const {
    return "";
  }
};

template<typename arg1, typename... argn>
class method_signature<arg1, argn...> : public method_signature<argn...> {
public:
  std::string method_sig() const {
#ifdef DBUS_CXX_CXA_DEMANGLE
    int status;
    char* demangled = abi::__cxa_demangle( typeid(arg1).name(), nullptr, nullptr, &status );
    std::string arg1_name( demangled );
    free( demangled );
    if( status < 0 ){
        arg1_name = typeid(arg1).name();
    }
#else
    std::string arg1_name = typeid(arg1).name();
#endif
    std::string remaining_args = method_signature<argn...>::method_sig();
    if( remaining_args.size() > 1 ){
        arg1_name += ",";
    }
    return arg1_name + remaining_args;
  }

  std::string introspect(const std::vector<std::string>& names, int idx, const std::string& spaces) const {
     arg1 arg;
     std::ostringstream output;
     std::string name = names.size() < idx ? names[idx] : "";
     output << spaces;
     output << "<arg name=\"";
     output << name << "\" type=\"";
     output << signature(arg);
     output << "\" ";
     output << "direction=\"in\"/>\n";
     output << method_signature<argn...>().introspect(names, idx + 1, spaces);
     return output.str();
  }
};


/*
 * dbus_function_traits - given a function, get information about it needed for dbus operations.
 */
template<typename T> 
struct dbus_function_traits;  

template<typename ...Args> 
struct dbus_function_traits<std::function<void(Args...)>>
{
  std::string dbus_sig() const {
    return dbus_signature<Args...>().dbus_sig();
  }

  std::string debug_string() const {
    return "void (" + method_signature<Args...>().method_sig() + ")";
  }

  std::string introspect(const std::vector<std::string>& names, int idx, const std::string& spaces) const {
    std::ostringstream sout;
    sout << method_signature<Args...>().introspect(names, idx + 1, spaces);
    return sout.str();
  }

  void extractAndCall(CallMessage::const_pointer callmsg, ReturnMessage::pointer retmsg, sigc::slot<void(Args...)> slot ){
    Message::iterator i = callmsg->begin();
    std::tuple<Args...> tup_args;
    std::apply( [i](auto ...arg) mutable {
               (i >> ... >> arg);
              },
    tup_args );

    std::apply(slot, tup_args);
  }
};

template<typename T_ret, typename ...Args> 
struct dbus_function_traits<std::function<T_ret(Args...)>>
{
  std::string dbus_sig() const {
    return dbus_signature<Args...>().dbus_sig();
  }

  std::string debug_string() const {
    std::ostringstream ret;
    ret << typeid(T_ret).name();
    ret << "(";
    ret << method_signature<Args...>().method_sig();
    ret << ")";
    return ret.str();
  }

  std::string introspect(const std::vector<std::string>& names, int idx, const std::string& spaces) const {
    std::ostringstream sout;
    T_ret ret_type;
    std::string name = "";
    if( names.size() > 0 ){
      name = names[0];
    }
    sout << spaces << "<arg name=\"" << name << "\" "
         << "type=\"" << signature(ret_type) << "\" "
         << "direction=\"out\"/>\n";
    sout << method_signature<Args...>().introspect(names, idx + 1, spaces);
    return sout.str();
  }

/*
  std::tuple<Args...> extract(Message::iterator i){
    std::tuple<Args...> tup_args;
    std::apply( [i](auto ...arg){
               (i >> ... >> arg);
              },
    tup_args );
    return tup_args;
  }
*/
  void extractAndCall(CallMessage::const_pointer callmsg, ReturnMessage::pointer retmsg, sigc::slot<T_ret(Args...)> slot ){
    Message::iterator i = callmsg->begin();
    std::tuple<Args...> tup_args;
    std::apply( [i](auto ...arg) mutable {
               (i >> ... >> arg);
              },
    tup_args );
    T_ret retval;

    retval = std::apply(slot, tup_args);
  }
};

} /* namespace priv */

} /* namespace DBus */

#endif

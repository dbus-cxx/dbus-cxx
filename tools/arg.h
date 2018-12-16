/***************************************************************************
 *   Copyright (C) 2009 by Rick L. Vinyard, Jr.                            *
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
#ifndef ARG_H
#define ARG_H

#include <string>
#include <dbus-cxx/types.h>

typedef enum Direction {
  DIRECTION_NONE,
  DIRECTION_IN,
  DIRECTION_OUT
} Direction;

typedef enum ProxyAdapter { 
  PROXY_RET, 
  PROXY_PARAM, 
  ADAPTER_RET, 
  ADAPTER_PARAM 
} ProxyAdapter;

/**
 * Represents an argument to a DBus method.
 */
class Arg {

public:
  Arg(): mis_const(false), mis_ref(false) { }

  /** The name of this argument */
  std::string name();
  /** What direction this argument goes in */
  Direction direction();
  std::string cpp_type(ProxyAdapter pa);
//   std::string cpp_cast(const std::string& var);
//   std::string dbus_cast(const std::string& var);
  std::string cpp_dbus_type();
  std::string stubsignature();
  DBus::Type type();
  std::string strfmt(int depth=0);

  bool need_iterator_support() { return not cpp_type_override.empty(); }
  std::pair<std::string,std::string> iterator_support() { return std::make_pair(cpp_type(ADAPTER_RET), cpp_dbus_type()); }

  void set_dbus_name( std::string );
  void set_direction_string( std::string );
  void set_signature( std::string );
  void set_cpp_type_override( std::string );
  void set_const( bool );
  void set_ref( bool );

  bool is_const();
  bool is_ref();
  
private:
  std::string dbus_name;
  std::string cxx_name;
  std::string direction_string;
  std::string cpp_type_override;
  DBus::Signature signature;
  bool mis_const;
  bool mis_ref;
};

#endif

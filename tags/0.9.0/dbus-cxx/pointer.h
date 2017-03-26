/***************************************************************************
 *   Copyright (C) 2007,2008,2009 by Rick L. Vinyard, Jr.                  *
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
#include <dbus-cxx/dbus-cxx-config.h>

#ifndef DBUS_CXXPOINTER
#define DBUS_CXXPOINTER
  // Headers for smart pointers
  #if defined( DBUS_CXX_USE_BOOST_SMART_POINTER )
    #include <boost/shared_ptr.hpp>
    #include <boost/weak_ptr.hpp>
    #define DBusCxxPointer boost::shared_ptr
    #define DBusCxxWeakPointer boost::weak_ptr
    #define dbus_cxx_static_pointer_cast  boost::static_pointer_cast
    #define dbus_cxx_const_pointer_cast   boost::const_pointer_cast
    #define dbus_cxx_dynamic_pointer_cast boost::dynamic_pointer_cast
  #elif defined( DBUS_CXX_USE_CXX0X_SMART_POINTER )
    #include <memory>
    #define DBusCxxPointer std::shared_ptr
    #define DBusCxxWeakPointer std::weak_ptr
    #define dbus_cxx_static_pointer_cast  std::static_pointer_cast
    #define dbus_cxx_const_pointer_cast   std::const_pointer_cast
    #define dbus_cxx_dynamic_pointer_cast std::dynamic_pointer_cast
  #elif defined( DBUS_CXX_USE_TR1_SMART_POINTER )
    #include <typeinfo>
    #include <memory>
    #include <functional>
    #if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 1 ))
      #include <ext/concurrence.h>
      #include <tr1/type_traits>
    #else
      #include <bits/concurrence.h>
    #endif
    #include <ext/mt_allocator.h>
    #include <tr1/boost_shared_ptr.h>
    #define DBusCxxPointer std::tr1::shared_ptr
    #define DBusCxxWeakPointer std::tr1::weak_ptr
    #define dbus_cxx_static_pointer_cast  std::tr1::static_pointer_cast
    #define dbus_cxx_const_pointer_cast   std::tr1::const_pointer_cast
    #define dbus_cxx_dynamic_pointer_cast std::tr1::dynamic_pointer_cast
  #else
    #error "Could not find any available smart pointers to use"
  #endif

#endif

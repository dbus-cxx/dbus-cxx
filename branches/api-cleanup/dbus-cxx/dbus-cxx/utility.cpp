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
#include <mutex>
#include <thread>
#include <iostream>

#define DBUSCXX_INTERNAL
#include "utility.h"
#include "error.h"
#include "connection.h"

simplelogger_log_function dbuscxx_log_function = nullptr;

namespace DBus
{
  
  /** mutex to lock when initializing */
  std::mutex init_mutex;

  bool initialized_var = false;

  static enum SL_LogLevel log_level = SL_INFO;

  void init(bool threadsafe)
  {
    dbus_bool_t result;

    if( initialized_var ){
      return;
    }

    SIMPLELOGGER_DEBUG( "dbus", "Initializing dbus-cxx, threadsafe:" << threadsafe );

    if ( threadsafe ){
        std::unique_lock<std::mutex> lock( init_mutex );
        result = dbus_threads_init_default();
	if (!result) throw std::bad_alloc();

        result = dbus_connection_allocate_data_slot( & Connection::m_weak_pointer_slot );
        if ( not result ) throw ErrorFailed::create(); 
    }else{
        result = dbus_connection_allocate_data_slot( & Connection::m_weak_pointer_slot );
        if ( not result ) throw ErrorFailed::create(); 
    }

    initialized_var = true;
  }
  
  bool initialized()
  {
    return initialized_var;
  }

  void setLoggingFunction( simplelogger_log_function function ){
    dbuscxx_log_function = function;
  }

  void logStdErr( const char* logger_name, const struct SL_LogLocation* location,
      const enum SL_LogLevel level,
      const char* log_string ){
    if( level <= log_level ) return;

    char buffer[ 4096 ];
    const char* stringLevel;
    std::thread::id this_id = std::this_thread::get_id();

    SL_LOGLEVEL_TO_STRING( stringLevel, level );

    snprintf( buffer, 4096, "0x%08X %s [%s] - %s(%s:%d)", this_id, logger_name, stringLevel, log_string, 
      location->file,
      location->line_number );
    std::cerr << buffer << std::endl;
  }

  void setLogLevel( const enum SL_LogLevel level ){
    log_level = level;
  }
}




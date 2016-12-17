/***************************************************************************
 *   Copyright (C) 2007,2008,2009 by Rick L. Vinyard, Jr.                  *
 *   rvinyard@cs.nmsu.edu                                                  *
 *   Copyright (C) 2014- by Robert Middleton                               *
 *   rm5248@rm5248.com                                                     *
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
#ifndef DBUSCXX_HEADERLOG_H
#define DBUSCXX_HEADERLOG_H

#include <sstream>
#include <dbus-cxx/simplelogger_defs.h>

extern simplelogger_log_function dbuscxx_log_function;

#define DBUSCXX_LOG_CSTR_HEADER( logger, message, level ) do{\
    if( !dbuscxx_log_function ) break;\
    struct SL_LogLocation location;\
    location.line_number = __LINE__;\
    location.file = __FILE__;\
    location.function = __func__;\
    dbuscxx_log_function( logger, &location, level, message );\
    } while(0)

#define DBUSCXX_DEBUG_STDSTR( logger, message ) do{\
    std::stringstream stream;\
    stream << message;\
    DBUSCXX_LOG_CSTR_HEADER( logger, stream.str().c_str(), SL_DEBUG);\
    } while(0)

#endif

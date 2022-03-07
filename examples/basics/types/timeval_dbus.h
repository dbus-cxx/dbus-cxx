/***************************************************************************
 *   Copyright (C) 2009 by Rick L. Vinyard, Jr.                            *
 *   rvinyard@cs.nmsu.edu                                                  *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 ***************************************************************************/
#ifndef TIMEVAL_DBUS_H
#define TIMEVAL_DBUS_H

#include <string>
/* VERY IMPORTANT: Because of how dbus-cxx finds signatures, the definition
 * of the signature needs to be before the general dbus-cxx includes 
 * See time_server.cpp for a more detailed explanation.
 */
namespace DBus {
  inline std::string signature(struct timeval)     { return "ii";        }
}

#include <dbus-cxx.h>

/* We need to reimplement the overload operators and the signature operator */

DBus::MessageIterator& operator>>(DBus::MessageIterator& i, struct timeval& t);

DBus::MessageAppendIterator& operator<<(DBus::MessageAppendIterator& i, const struct timeval& t);


#endif



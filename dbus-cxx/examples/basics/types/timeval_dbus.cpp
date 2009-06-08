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
#include "timeval_dbus.h"

DBus::MessageIterator& operator>>(DBus::MessageIterator& i, struct timeval& t)
{
  t.tv_sec = i.get_int32();
  i.next();
  t.tv_usec = i.get_int32();
  i.next();
  return i;
}

DBus::MessageAppendIterator& operator<<(DBus::MessageAppendIterator& i, struct timeval& t)
{
  i.append( (int32_t)t.tv_sec );
  i.append( (int32_t)t.tv_usec );
  return i;
}

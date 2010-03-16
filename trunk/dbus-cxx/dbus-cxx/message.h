/***************************************************************************
 *   Copyright (C) 2007,2009,2010 by Rick L. Vinyard, Jr.                  *
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
#include <string>
#include <vector>
#include <map>

#include <dbus/dbus.h>

#include <dbus-cxx/pointer.h>
#include <dbus-cxx/messageiterator.h>
#include <dbus-cxx/messageappenditerator.h>

#ifndef DBUSCXX_MESSAGE_H
#define DBUSCXX_MESSAGE_H

namespace DBus
{

  class ReturnMessage;

  /**
   * @defgroup message DBus Messages
   */

  /**
   * This class represents a basic DBus message and also serves as a base
   * class for the specialized message types (call, return, signal, error).
   * 
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   *
   * @ingroup message
   */

  // TODO support
  //  - dbus_message_type_from_string
  //  - dbus_message_type_to_string
  //  - dbus_message_marshal
  //  - dbus_message_demarshal
  
  class Message
  {
    public:

      typedef DBusCxxPointer<Message> pointer;

      typedef DBusCxxPointer<const Message> const_pointer;

      typedef DBusCxxWeakPointer<Message> weak_pointer;

    protected:
      
      Message( MessageType type );

      Message( DBusMessage* cobj=NULL, CreateMethod m = CREATE_ALIAS );

      Message( Message::pointer other, CreateMethod m = CREATE_ALIAS );

      Message( Message::const_pointer other, CreateMethod m = CREATE_ALIAS );

    public:

      typedef MessageIterator iterator;

      typedef MessageAppendIterator append_iterator;

      static pointer create( MessageType type );

      static pointer create( DBusMessage* cobj=NULL, CreateMethod m = CREATE_ALIAS );

      static pointer create( Message::pointer other, CreateMethod m = CREATE_ALIAS );

      static pointer create( Message::const_pointer other, CreateMethod m = CREATE_ALIAS );

      DBusCxxPointer<ReturnMessage> create_reply() const;

      virtual ~Message();

      Message& operator = ( const Message& m );

      bool operator == ( const Message& other );

      bool is_valid() const;

      void invalidate();

      operator bool() const;

      uint32_t serial() const;

//       Message copy();

      int type() const;

      void set_auto_start( bool auto_start);

      bool auto_start();

      bool set_destination( const std::string& s );

      const char* destination() const;

      bool set_sender( const std::string& s );

      const char* sender() const;

      bool is_call( const std::string& interface, const std::string& method ) const;

      bool is_signal( const std::string& interface, const std::string& signal_name ) const;

      bool is_error( const std::string& error_name ) const;

      bool has_destination( const std::string& name ) const;

      bool has_sender( const std::string& name ) const;

      template <typename T>
      iterator operator>>( T& value ) const
      {
        iterator iter = this->begin();
        iter >> value;
        return iter;
      }
      
      template <typename T>
      append_iterator operator<<( const T& value )
      {
        append_iterator aiter( *this );
        aiter << value;
        return aiter;
      }

      iterator begin() const;

      iterator end() const;

      append_iterator append();

      DBusMessage* cobj() const;

    protected:

      friend void init(bool);

      DBusMessage* m_cobj;

      bool m_valid;

  };

}

template <typename T>
inline
DBus::Message::iterator operator>>( DBus::Message::const_pointer ptr, T& value )
{
  if ( not ptr ) throw -1;
  return (*ptr) >> value;
}

template <typename T>
inline
DBus::Message::append_iterator operator<<( DBus::Message::pointer ptr, const T& value )
{
  if ( not ptr ) throw -1;
  return (*ptr) << value;
}

#endif

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
#include "messageappenditerator.h"

#include <cstring>
#include <cstdlib>

#include "message.h"

namespace DBus
{

  bool MessageAppendIterator::protected_append( const bool& v )
  {
    bool result;
    dbus_bool_t b = v;

    if ( not this->is_valid() ) return false;

    result = dbus_message_iter_append_basic( &m_cobj, TYPE_BOOLEAN, &b );

    if ( ! result ) m_message->invalidate();

    return result;
  }

  bool MessageAppendIterator::protected_append( const std::string& v )
  {
    return protected_append( v.c_str() );
  }

  bool MessageAppendIterator::protected_append( const Signature& v )
  {
    bool result;
    if ( not this->is_valid() ) return false;
    const std::string& sig = v;
    const char* pstr = sig.c_str();
    result = dbus_message_iter_append_basic( &m_cobj, TYPE_SIGNATURE, &(pstr) );
    if ( ! result ) m_message->invalidate();
    return result;
  }

  bool MessageAppendIterator::protected_append( const Path& v )
  {
    bool result;
    if ( not this->is_valid() ) return false;
    const char* pstr = v.c_str();
    result = dbus_message_iter_append_basic( &m_cobj, TYPE_OBJECT_PATH, &(pstr) );
    if ( ! result ) m_message->invalidate();
    return result;
  }

  template <typename T>
      inline
      bool MessageAppendIterator::protected_append( const T& v )
  {
    bool result;

    if ( not this->is_valid() ) return false;

    result = dbus_message_iter_append_basic( &m_cobj, DBus::type( v ), &v );

    if ( ! result ) m_message->invalidate();

    return result;
  }

  MessageAppendIterator::MessageAppendIterator():
      m_message( NULL ), m_subiter( NULL )
  {
    memset( &m_cobj, 0x00, sizeof( DBusMessageIter ) );
  }

  MessageAppendIterator::MessageAppendIterator( Message& message ):
      m_message( NULL ), m_subiter( NULL )
  {
    memset( &m_cobj, 0x00, sizeof( DBusMessageIter ) );
    this->init( message );
  }

  MessageAppendIterator::MessageAppendIterator( Message::pointer message ):
      m_message( NULL ), m_subiter( NULL )
  {
    memset( &m_cobj, 0x00, sizeof( DBusMessageIter ) );
    if ( message ) this->init( *message );
  }

  MessageAppendIterator::~MessageAppendIterator()
  {
//     if ( m_subiter ) this->close_container();
  }

  DBusMessageIter* MessageAppendIterator::cobj()
  {
    return &m_cobj;
  }

  bool MessageAppendIterator::init( Message & message )
  {
    if ( message ) {
      dbus_message_iter_init_append( message.cobj(), &m_cobj );
      m_message = &message;
      if ( m_subiter ) free( m_subiter );
      m_subiter = NULL;
      return true;
    }

    m_message = NULL;
    if ( m_subiter ) free( m_subiter );
    m_subiter = NULL;
    return false;
  }

  void MessageAppendIterator::invalidate()
  {
    memset( &m_cobj, 0x00, sizeof( DBusMessageIter ) );
    m_message = NULL;
    m_subiter = NULL;
  }

  bool MessageAppendIterator::is_valid() const
  {
    if ( not( m_message and m_message->is_valid() ) ) return false;
    return true;
  }

  MessageAppendIterator::operator bool() const
  {
    return this->is_valid();
  }

  bool MessageAppendIterator::append( bool v )
  {
    return this->protected_append( v );
  }

  bool MessageAppendIterator::append( uint8_t v )
  {
    return this->protected_append( v );
  }

  bool MessageAppendIterator::append( int16_t v )
  {
    return this->protected_append( v );
  }

  bool MessageAppendIterator::append( uint16_t v )
  {
    return this->protected_append( v );
  }

  bool MessageAppendIterator::append( int32_t v )
  {
    return this->protected_append( v );
  }

  bool MessageAppendIterator::append( uint32_t v )
  {
    return this->protected_append( v );
  }

  bool MessageAppendIterator::append( int64_t v )
  {
    return this->protected_append( v );
  }

  bool MessageAppendIterator::append( uint64_t v )
  {
    return this->protected_append( v );
  }

  bool MessageAppendIterator::append( double v )
  {
    return this->protected_append( v );
  }

  bool MessageAppendIterator::append( const char* v )
  {
    return this->protected_append( v );
  }

  bool MessageAppendIterator::append( const std::string& v )
  {
    return this->protected_append( v );
  }

  bool MessageAppendIterator::append( const Signature& v )
  {
    return this->protected_append( v );
  }

  bool MessageAppendIterator::append( const Path& v )
  {
    return this->protected_append( v );
  }
  
  bool MessageAppendIterator::append( char v )
  {
    return this->protected_append( v );
  }
  
  bool MessageAppendIterator::append( int8_t v )
  {
    return this->protected_append( v );
  }
  
  bool MessageAppendIterator::append( float v )
  {
    double d = v;
    return this->protected_append( d );
  }

#if DBUS_CXX_SIZEOF_LONG_INT == 4
  
  bool MessageAppendIterator::append( long int v )
  {
    return this->protected_append( v );
  }
  
  bool MessageAppendIterator::append( long unsigned int v )
  {
    return this->protected_append( v );
  }

#endif


  bool MessageAppendIterator::open_container( ContainerType t, const std::string& sig )
  {
    bool success;

    if ( m_subiter ) this->close_container();

    if ( m_message )
      m_subiter = new MessageAppendIterator( *m_message );
    else
      m_subiter = new MessageAppendIterator();

    if ( t == CONTAINER_STRUCT || t == CONTAINER_DICT_ENTRY )
      success = dbus_message_iter_open_container( &m_cobj, t, NULL, m_subiter->cobj() );
    else
      success = dbus_message_iter_open_container( &m_cobj, t, sig.c_str(), m_subiter->cobj() );
    
    return success;
  }

  bool MessageAppendIterator::close_container( )
  {
    bool success;
    if ( ! m_subiter ) return false;
    success = dbus_message_iter_close_container( &m_cobj, m_subiter->cobj() );
    delete m_subiter;
    m_subiter = NULL;
    if ( ! success ) throw ErrorNoMemory::create( "MessageAppendIterator::close_container: No memory to close the container" );
    return success;
  }

  MessageAppendIterator* MessageAppendIterator::sub_iterator()
  {
    return m_subiter;
  }



}


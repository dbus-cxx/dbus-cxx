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
#include <dbus-cxx/variant.h>
#include "messageappenditerator.h"

#include <cstring>
#include <cstdlib>

#include "message.h"


namespace DBus
{

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

  MessageAppendIterator::MessageAppendIterator( std::shared_ptr<Message> message ):
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

  MessageAppendIterator& MessageAppendIterator::operator<<( const bool& v ){
    bool result;
    dbus_bool_t b = v;

    if ( not this->is_valid() ) return *this;

    result = dbus_message_iter_append_basic( &m_cobj, DBus::typeToDBusType( DataType::BOOLEAN ), &b );

    if ( ! result ) m_message->invalidate();

    return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const int8_t& v ){
    bool result;

    if ( not this->is_valid() ) return *this;

    result = dbus_message_iter_append_basic( &m_cobj, DBus::typeToDBusType( DataType::BYTE ), &v );

    if ( ! result ) m_message->invalidate();

    return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const uint8_t& v ){
    bool result;

    if ( not this->is_valid() ) return *this;

    result = dbus_message_iter_append_basic( &m_cobj, DBus::typeToDBusType( DataType::BYTE ), &v );

    if ( ! result ) m_message->invalidate();

    return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const int16_t& v ){
    bool result;

    if ( not this->is_valid() ) return *this;

    result = dbus_message_iter_append_basic( &m_cobj, DBus::typeToDBusType( DataType::INT16 ), &v );

    if ( ! result ) m_message->invalidate();

    return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const uint16_t& v ){
    bool result;

    if ( not this->is_valid() ) return *this;

    result = dbus_message_iter_append_basic( &m_cobj, DBus::typeToDBusType( DataType::UINT16 ), &v );

    if ( ! result ) m_message->invalidate();

    return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const int32_t& v ){
    bool result;

    if ( not this->is_valid() ) return *this;

    result = dbus_message_iter_append_basic( &m_cobj, DBus::typeToDBusType( DataType::INT32 ), &v );

    if ( ! result ) m_message->invalidate();

    return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const uint32_t& v ){
    bool result;

    if ( not this->is_valid() ) return *this;

    result = dbus_message_iter_append_basic( &m_cobj, DBus::typeToDBusType( DataType::UINT32 ), &v );

    if ( ! result ) m_message->invalidate();

    return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const int64_t& v ){
    bool result;

    if ( not this->is_valid() ) return *this;

    result = dbus_message_iter_append_basic( &m_cobj, DBus::typeToDBusType( DataType::INT64 ), &v );

    if ( ! result ) m_message->invalidate();

    return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const uint64_t& v ){
    bool result;

    if ( not this->is_valid() ) return *this;

    result = dbus_message_iter_append_basic( &m_cobj, DBus::typeToDBusType( DataType::UINT64 ), &v );

    if ( ! result ) m_message->invalidate();

    return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const double& v ){
    bool result;

    if ( not this->is_valid() ) return *this;

    result = dbus_message_iter_append_basic( &m_cobj, DBus::typeToDBusType( DataType::DOUBLE ), &v );

    if ( ! result ) m_message->invalidate();

    return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const float& v ){
    bool result;
    double d = v;

    if ( not this->is_valid() ) return *this;

    result = dbus_message_iter_append_basic( &m_cobj, DBus::typeToDBusType( DataType::DOUBLE ), &d );

    if ( ! result ) m_message->invalidate();

    return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const char* v ){
    bool result;

    if ( not this->is_valid() ) return *this;

    result = dbus_message_iter_append_basic( &m_cobj, DBus::typeToDBusType( DataType::STRING ), &v );

    if ( ! result ) m_message->invalidate();

    return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const std::string& v ){
    return this->operator<<( v.c_str() );
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const Signature& v ){
    bool result;

    if ( not this->is_valid() ) return *this;

    const std::string sig = v.str();
    const char* sig_val = sig.c_str();
    result = dbus_message_iter_append_basic( &m_cobj, DBus::typeToDBusType( DataType::SIGNATURE ), &sig_val );

    if ( ! result ) m_message->invalidate();

    return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const Path& v ){
    bool result;

    if ( not this->is_valid() ) return *this;

    const char* path_val = v.c_str();
    result = dbus_message_iter_append_basic( &m_cobj, DBus::typeToDBusType( DataType::OBJECT_PATH ), &path_val );

    if ( ! result ) m_message->invalidate();

    return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const std::shared_ptr<FileDescriptor> v ){
    bool result;
    int raw_fd;

    if ( not this->is_valid() ) return *this;
    if ( not v ) return *this;

    raw_fd = v->getDescriptor();
    result = dbus_message_iter_append_basic( &m_cobj, DBus::typeToDBusType( DataType::UNIX_FD ), &raw_fd );

    if ( ! result ) m_message->invalidate();

    return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const Variant& v ){
    bool result;

    if ( not this->is_valid() ) return *this;
    if( v.currentType() == DataType::INVALID ) return *this;

    this->open_container( ContainerType::VARIANT, v.signature()  );
    switch( v.currentType() ){
    case DataType::BYTE:
        *m_subiter << std::any_cast<uint8_t>(v.value());
        break;
    case DataType::BOOLEAN:
        *m_subiter << std::any_cast<bool>(v.value());
        break;
    case DataType::INT16:
        *m_subiter << std::any_cast<int16_t>(v.value());
        break;
    case DataType::UINT16:
        *m_subiter << std::any_cast<uint16_t>(v.value());
        break;
    case DataType::INT32:
        *m_subiter << std::any_cast<int32_t>(v.value());
        break;
    case DataType::UINT32:
        *m_subiter << std::any_cast<uint16_t>(v.value());
        break;
    case DataType::INT64:
        *m_subiter << std::any_cast<int64_t>(v.value());
        break;
    case DataType::UINT64:
        *m_subiter << std::any_cast<uint64_t>(v.value());
        break;
    case DataType::DOUBLE:
        *m_subiter << std::any_cast<double>(v.value());
        break;
    case DataType::STRING:
        *m_subiter << std::any_cast<std::string>(v.value());
        break;
    case DataType::OBJECT_PATH:
        *m_subiter << std::any_cast<Path>(v.value());
        break;
    case DataType::SIGNATURE:
        *m_subiter << std::any_cast<Signature>(v.value());
        break;
    case DataType::VARIANT:
        *m_subiter << std::any_cast<Variant>(v.value());
        break;
    case DataType::UNIX_FD:
        *m_subiter << std::any_cast<std::shared_ptr<FileDescriptor>>(v.value());
        break;
    default: break;
    }
    this->close_container();

    return *this;
  }


  bool MessageAppendIterator::open_container( ContainerType t, const std::string& sig )
  {
    bool success;

    if ( m_subiter ) this->close_container();

    if ( m_message )
      m_subiter = new MessageAppendIterator( *m_message );
    else
      m_subiter = new MessageAppendIterator();

    if ( t == ContainerType::STRUCT || t == ContainerType::DICT_ENTRY )
      success = dbus_message_iter_open_container( &m_cobj, DBus::typeToDBusContainerType( t ), NULL, m_subiter->cobj() );
    else
      success = dbus_message_iter_open_container( &m_cobj, DBus::typeToDBusContainerType( t ), sig.c_str(), m_subiter->cobj() );
    
    return success;
  }

  bool MessageAppendIterator::close_container( )
  {
    bool success;
    if ( ! m_subiter ) return false;
    success = dbus_message_iter_close_container( &m_cobj, m_subiter->cobj() );
    delete m_subiter;
    m_subiter = NULL;
    if ( ! success ) throw ErrorNoMemory( "MessageAppendIterator::close_container: No memory to close the container" );
    return success;
  }

  MessageAppendIterator* MessageAppendIterator::sub_iterator()
  {
    return m_subiter;
  }

}


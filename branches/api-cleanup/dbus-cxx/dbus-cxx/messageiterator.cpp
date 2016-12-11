/***************************************************************************
 *   Copyright (C) 2007,2009 by Rick L. Vinyard, Jr.                       *
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
#include "messageiterator.h"

#include <cstring>

#include "message.h"

namespace DBus
{

  MessageIterator::MessageIterator():
      m_message( NULL )
  {
    memset( &m_cobj, 0x00, sizeof( DBusMessageIter ) );
  }

  MessageIterator::MessageIterator( const Message& message ):
      m_message( NULL )
  {
    memset( &m_cobj, 0x00, sizeof( DBusMessageIter ) );
    this->init(message);
  }

  MessageIterator::MessageIterator( Message::pointer message ):
      m_message( NULL )
  {
    memset( &m_cobj, 0x00, sizeof( DBusMessageIter ) );
    if ( message ) this->init(*message);
  }

  const Message* MessageIterator::message() const
  {
    return m_message;
  }

  DBusMessageIter* MessageIterator::cobj()
  {
    return &m_cobj;
  }

  bool MessageIterator::init(const Message & message)
  {
    if ( message and dbus_message_iter_init(message.cobj(), &m_cobj) )
    {
      m_message = &message;
      return true;
    }

    m_message = NULL;
    return false;
  }

  void MessageIterator::invalidate()
  {
    memset( &m_cobj, 0x00, sizeof( DBusMessageIter ) );
    m_message = NULL;
  }

  bool MessageIterator::is_valid() const
  {
    if ( not (m_message and m_message->is_valid() ) ) return false;
    if ( this->arg_type() == TYPE_INVALID ) return false;
    return true;
  }

  bool MessageIterator::has_next() const
  {
    if ( this->is_valid() ) return dbus_message_iter_has_next( const_cast<DBusMessageIter*>(& m_cobj) );
    return false;
  }

  bool MessageIterator::next()
  {
    if ( not this->is_valid() ) return false;

    bool result;

    result = dbus_message_iter_next( & m_cobj );

    if ( not result or this->arg_type() == TYPE_INVALID )
    {
      this->invalidate();
      return false;
    }

    return true;
  }

  MessageIterator& MessageIterator::operator ++()
  {
    this->next();
    return ( *this );
  }

  MessageIterator MessageIterator::operator ++( int )
  {
    MessageIterator temp_copy;
    temp_copy = *this;
    ++( *this );
    return temp_copy;
  }

  bool MessageIterator::operator==( const MessageIterator& other )
  {
    return ( m_message == other.m_message && memcmp( &m_cobj, &( other.m_cobj ), sizeof( DBusMessageIter ) ) == 0 );
  }

  Type MessageIterator::arg_type() const
  {
    return checked_type_cast(dbus_message_iter_get_arg_type( const_cast<DBusMessageIter*>( & m_cobj ) ));
  }

  Type MessageIterator::element_type() const
  {
    if ( this->arg_type() != TYPE_ARRAY )
      return TYPE_INVALID;
    return checked_type_cast(dbus_message_iter_get_element_type( const_cast<DBusMessageIter*>( & m_cobj ) ));
  }

  bool MessageIterator::is_fixed() const
  {
    return dbus_type_is_fixed( this->element_type() );
  }

  bool MessageIterator::is_container() const
  {
    return dbus_type_is_container( this->arg_type() );
  }

  bool MessageIterator::is_array() const
  {
    return this->arg_type() == TYPE_ARRAY;
  }

  bool MessageIterator::is_dict() const
  {
    return this->is_array() && this->element_type() == TYPE_DICT_ENTRY;
  }

  MessageIterator MessageIterator::recurse()
  {
    MessageIterator iter;

    if ( not this->is_container() ) return iter;
    
    iter.m_message = m_message;
    dbus_message_iter_recurse( & m_cobj, & ( iter.m_cobj ) );
    return iter;
  }

  std::string MessageIterator::signature() const
  {
    char* sig;
    std::string retsig;

    sig = dbus_message_iter_get_signature( const_cast<DBusMessageIter*>( & m_cobj ) );

    retsig = sig;

    dbus_free( sig );

    return retsig;
  }

  MessageIterator::operator bool()
  {
    // TODO check for invalid
    switch ( this->arg_type() )
    {
      case TYPE_BYTE:    return (uint8_t)(*this);
      case TYPE_BOOLEAN: return get_bool();
      case TYPE_INT16:   return (int16_t)(*this);
      case TYPE_UINT16:  return (uint16_t)(*this);
      case TYPE_INT32:   return (int32_t)(*this);
      case TYPE_UINT32:  return (uint32_t)(*this);
      case TYPE_INT64:   return (int64_t)(*this);
      case TYPE_UINT64:  return (uint64_t)(*this);
      case TYPE_DOUBLE:  return (double)(*this);
      default:
        throw ErrorInvalidTypecast::create("MessageIterator:: casting non-numeric type to boolean value");
    }
  }

  MessageIterator::operator uint8_t()
  {
    // TODO check for invalid
    switch ( this->arg_type() )
    {
      case TYPE_BYTE:    return get_uint8();
      case TYPE_BOOLEAN: return (bool)(*this);
      case TYPE_INT16:   return (int16_t)(*this);
      case TYPE_UINT16:  return (uint16_t)(*this);
      case TYPE_INT32:   return (int32_t)(*this);
      case TYPE_UINT32:  return (uint32_t)(*this);
      case TYPE_INT64:   return (int64_t)(*this);
      case TYPE_UINT64:  return (uint64_t)(*this);
      case TYPE_DOUBLE:  return (double)(*this);
      default:
        throw ErrorInvalidTypecast::create("MessageIterator:: casting non-numeric type to numeric value");
    }
  }

  MessageIterator::operator uint16_t()
  {
    // TODO check for invalid
    switch ( this->arg_type() )
    {
      case TYPE_BYTE:    return (uint8_t)(*this);
      case TYPE_BOOLEAN: return (bool)(*this);
      case TYPE_INT16:   return (int16_t)(*this);
      case TYPE_UINT16:  return get_uint16();
      case TYPE_INT32:   return (int32_t)(*this);
      case TYPE_UINT32:  return (uint32_t)(*this);
      case TYPE_INT64:   return (int64_t)(*this);
      case TYPE_UINT64:  return (uint64_t)(*this);
      case TYPE_DOUBLE:  return (double)(*this);
      default:
        throw ErrorInvalidTypecast::create("MessageIterator:: casting non-numeric type to numeric value");
    }
  }

  MessageIterator::operator int16_t()
  {
    // TODO check for invalid
    switch ( this->arg_type() )
    {
      case TYPE_BYTE:    return (uint8_t)(*this);
      case TYPE_BOOLEAN: return (bool)(*this);
      case TYPE_INT16:   return get_int16();
      case TYPE_UINT16:  return (uint16_t)(*this);
      case TYPE_INT32:   return (int32_t)(*this);
      case TYPE_UINT32:  return (uint32_t)(*this);
      case TYPE_INT64:   return (int64_t)(*this);
      case TYPE_UINT64:  return (uint64_t)(*this);
      case TYPE_DOUBLE:  return (double)(*this);
      default:
        throw ErrorInvalidTypecast::create("MessageIterator:: casting non-numeric type to numeric value");
    }
  }

  MessageIterator::operator uint32_t()
  {
    // TODO check for invalid
    switch ( this->arg_type() )
    {
      case TYPE_BYTE:    return (uint8_t)(*this);
      case TYPE_BOOLEAN: return (bool)(*this);
      case TYPE_INT16:   return (int16_t)(*this);
      case TYPE_UINT16:  return (uint16_t)(*this);
      case TYPE_INT32:   return (int32_t)(*this);
      case TYPE_UINT32:  return get_uint32();
      case TYPE_INT64:   return (int64_t)(*this);
      case TYPE_UINT64:  return (uint64_t)(*this);
      case TYPE_DOUBLE:  return (double)(*this);
      default:
        throw ErrorInvalidTypecast::create("MessageIterator:: casting non-numeric type to numeric value");
    }
  }

  MessageIterator::operator int32_t()
  {
    // TODO check for invalid
    switch ( this->arg_type() )
    {
      case TYPE_BYTE:    return (uint8_t)(*this);
      case TYPE_BOOLEAN: return (bool)(*this);
      case TYPE_INT16:   return (int16_t)(*this);
      case TYPE_UINT16:  return (uint16_t)(*this);
      case TYPE_INT32:   return get_int32();
      case TYPE_UINT32:  return (uint32_t)(*this);
      case TYPE_INT64:   return (int64_t)(*this);
      case TYPE_UINT64:  return (uint64_t)(*this);
      case TYPE_DOUBLE:  return (double)(*this);
      default:
        throw ErrorInvalidTypecast::create("MessageIterator:: casting non-numeric type to numeric value");
    }
  }

  MessageIterator::operator uint64_t()
  {
    // TODO check for invalid
    switch ( this->arg_type() )
    {
      case TYPE_BYTE:    return (uint8_t)(*this);
      case TYPE_BOOLEAN: return (bool)(*this);
      case TYPE_INT16:   return (int16_t)(*this);
      case TYPE_UINT16:  return (uint16_t)(*this);
      case TYPE_INT32:   return (int32_t)(*this);
      case TYPE_UINT32:  return (uint32_t)(*this);
      case TYPE_INT64:   return (int64_t)(*this);
      case TYPE_UINT64:  return get_uint64();
      case TYPE_DOUBLE:  return (double)(*this);
      default:
        throw ErrorInvalidTypecast::create("MessageIterator:: casting non-numeric type to numeric value");
    }
  }

  MessageIterator::operator int64_t()
  {
    // TODO check for invalid
    switch ( this->arg_type() )
    {
      case TYPE_BYTE:    return (uint8_t)(*this);
      case TYPE_BOOLEAN: return (bool)(*this);
      case TYPE_INT16:   return (int16_t)(*this);
      case TYPE_UINT16:  return (uint16_t)(*this);
      case TYPE_INT32:   return (int32_t)(*this);
      case TYPE_UINT32:  return (uint32_t)(*this);
      case TYPE_INT64:   return get_int64();
      case TYPE_UINT64:  return (uint64_t)(*this);
      case TYPE_DOUBLE:  return (double)(*this);
      default:
        throw ErrorInvalidTypecast::create("MessageIterator:: casting non-numeric type to numeric value");
    }
  }

  MessageIterator::operator double()
  {
    // TODO check for invalid
    switch ( this->arg_type() )
    {
      case TYPE_BYTE:    return (uint8_t)(*this);
      case TYPE_BOOLEAN: return (bool)(*this);
      case TYPE_INT16:   return (int16_t)(*this);
      case TYPE_UINT16:  return (uint16_t)(*this);
      case TYPE_INT32:   return (int32_t)(*this);
      case TYPE_UINT32:  return (uint32_t)(*this);
      case TYPE_INT64:   return (int64_t)(*this);
      case TYPE_UINT64:  return (uint64_t)(*this);
      case TYPE_DOUBLE:  return get_double();
      default:
        throw ErrorInvalidTypecast::create("MessageIterator:: casting non-numeric type to numeric value");
    }
  }

  MessageIterator::operator const char*()
  {
    switch ( this->arg_type() )
    {
      case TYPE_STRING:
      case TYPE_OBJECT_PATH:
      case TYPE_SIGNATURE:
        return get_string();
      default:
        throw ErrorInvalidTypecast::create("MessageIterator:: extracting non-string type to char*");
    }
  }

  MessageIterator::operator char()
  {
    // TODO check for invalid
    switch ( this->arg_type() )
    {
      case TYPE_BYTE:    return static_cast<char>(get_uint8());
      case TYPE_BOOLEAN: return (bool)(*this);
      case TYPE_INT16:   return (int16_t)(*this);
      case TYPE_UINT16:  return (uint16_t)(*this);
      case TYPE_INT32:   return (int32_t)(*this);
      case TYPE_UINT32:  return (uint32_t)(*this);
      case TYPE_INT64:   return (int64_t)(*this);
      case TYPE_UINT64:  return (uint64_t)(*this);
      case TYPE_DOUBLE:  return (double)(*this);
      default:
        throw ErrorInvalidTypecast::create("MessageIterator:: casting non-numeric type to numeric value");
    }
  }

  MessageIterator::operator int8_t()
  {
    // TODO check for invalid
    switch ( this->arg_type() )
    {
      case TYPE_BYTE:    return static_cast<int8_t>(get_uint8());
      case TYPE_BOOLEAN: return (bool)(*this);
      case TYPE_INT16:   return (int16_t)(*this);
      case TYPE_UINT16:  return (uint16_t)(*this);
      case TYPE_INT32:   return (int32_t)(*this);
      case TYPE_UINT32:  return (uint32_t)(*this);
      case TYPE_INT64:   return (int64_t)(*this);
      case TYPE_UINT64:  return (uint64_t)(*this);
      case TYPE_DOUBLE:  return (double)(*this);
      default:
        throw ErrorInvalidTypecast::create("MessageIterator:: casting non-numeric type to numeric value");
    }
  }

  MessageIterator::operator float()
  {
    // TODO check for invalid
    switch ( this->arg_type() )
    {
      case TYPE_BYTE:    return (uint8_t)(*this);
      case TYPE_BOOLEAN: return (bool)(*this);
      case TYPE_INT16:   return (int16_t)(*this);
      case TYPE_UINT16:  return (uint16_t)(*this);
      case TYPE_INT32:   return (int32_t)(*this);
      case TYPE_UINT32:  return (uint32_t)(*this);
      case TYPE_INT64:   return (int64_t)(*this);
      case TYPE_UINT64:  return (uint64_t)(*this);
      case TYPE_DOUBLE:  return static_cast<float>(get_double());
      default:
        throw ErrorInvalidTypecast::create("MessageIterator:: casting non-numeric type to numeric value");
    }
  }
  
#if DBUS_CXX_SIZEOF_LONG_INT == 4
  MessageIterator::operator unsigned long int()
  {
    // TODO check for invalid
    switch ( this->arg_type() )
    {
      case TYPE_BYTE:    return (uint8_t)(*this);
      case TYPE_BOOLEAN: return (bool)(*this);
      case TYPE_INT16:   return (int16_t)(*this);
      case TYPE_UINT16:  return (uint16_t)(*this);
      case TYPE_INT32:   return (int32_t)(*this);
      case TYPE_UINT32:  return get_uint32();
      case TYPE_INT64:   return (int64_t)(*this);
      case TYPE_UINT64:  return (uint64_t)(*this);
      case TYPE_DOUBLE:  return (double)(*this);
      default:
        throw ErrorInvalidTypecast::create("MessageIterator:: casting non-numeric type to numeric value");
    }
  }

  MessageIterator::operator long int()
  {
    // TODO check for invalid
    switch ( this->arg_type() )
    {
      case TYPE_BYTE:    return (uint8_t)(*this);
      case TYPE_BOOLEAN: return (bool)(*this);
      case TYPE_INT16:   return (int16_t)(*this);
      case TYPE_UINT16:  return (uint16_t)(*this);
      case TYPE_INT32:   return get_int32();
      case TYPE_UINT32:  return (uint32_t)(*this);
      case TYPE_INT64:   return (int64_t)(*this);
      case TYPE_UINT64:  return (uint64_t)(*this);
      case TYPE_DOUBLE:  return (double)(*this);
      default:
        throw ErrorInvalidTypecast::create("MessageIterator:: casting non-numeric type to numeric value");
    }
  }

#endif

  bool MessageIterator::get_bool()
  {
    // TODO check for invalid
    dbus_bool_t ptr;
    if ( this->arg_type() != TYPE_BOOLEAN )
      throw ErrorInvalidTypecast::create("MessageIterator: getting bool and type is not TYPE_BOOLEAN");
    dbus_message_iter_get_basic( &m_cobj, &ptr );
    return ptr;
  }

  uint8_t MessageIterator::get_uint8()
  {
    // TODO check for invalid
    uint8_t ptr;
    if ( this->arg_type() != TYPE_BYTE )
      throw ErrorInvalidTypecast::create("MessageIterator: getting uint8_t and type is not TYPE_BYTE");
    dbus_message_iter_get_basic( &m_cobj, &ptr );
    return ptr;
  }

  int16_t MessageIterator::get_int16()
  {
    // TODO check for invalid
    dbus_int16_t ptr;
    if ( this->arg_type() != TYPE_INT16 )
      throw ErrorInvalidTypecast::create("MessageIterator: getting int16_t and type is not TYPE_INT16");
    dbus_message_iter_get_basic( &m_cobj, &ptr );
    return ptr;
  }

  uint16_t MessageIterator::get_uint16()
  {
    // TODO check for invalid
    dbus_uint16_t ptr;
    if ( this->arg_type() != TYPE_UINT16 )
      throw ErrorInvalidTypecast::create("MessageIterator: getting uint16_t and type is not TYPE_UINT16");
    dbus_message_iter_get_basic( &m_cobj, &ptr );
    return ptr;
  }

  int32_t MessageIterator::get_int32()
  {
    // TODO check for invalid
    dbus_int32_t ptr;
    if ( this->arg_type() != TYPE_INT32 )
      throw ErrorInvalidTypecast::create("MessageIterator: getting int32_t and type is not TYPE_INT32");
    dbus_message_iter_get_basic( &m_cobj, &ptr );
    return ptr;
  }

  uint32_t MessageIterator::get_uint32()
  {
    // TODO check for invalid
    dbus_uint32_t ptr;
    if ( this->arg_type() != TYPE_UINT32 )
      throw ErrorInvalidTypecast::create("MessageIterator: getting uint32_t and type is not TYPE_UINT32");
    dbus_message_iter_get_basic( &m_cobj, &ptr );
    return ptr;
  }

  int64_t MessageIterator::get_int64()
  {
    // TODO check for invalid
    dbus_int64_t ptr;
    if ( this->arg_type() != TYPE_INT64 )
      throw ErrorInvalidTypecast::create("MessageIterator: getting int64_t and type is not TYPE_INT64");
    dbus_message_iter_get_basic( &m_cobj, &ptr );
    return ptr;
  }

  uint64_t MessageIterator::get_uint64()
  {
    // TODO check for invalid
    dbus_uint64_t ptr;
    if ( this->arg_type() != TYPE_UINT64 )
      throw ErrorInvalidTypecast::create("MessageIterator: getting uint64_t and type is not TYPE_UINT64");
    dbus_message_iter_get_basic( &m_cobj, &ptr );
    return ptr;
  }

  double MessageIterator::get_double()
  {
    // TODO check for invalid
    double ptr;
    if ( this->arg_type() != TYPE_DOUBLE )
      throw ErrorInvalidTypecast::create("MessageIterator: getting double and type is not TYPE_DOUBLE");
    dbus_message_iter_get_basic( &m_cobj, &ptr );
    return ptr;
  }

  const char* MessageIterator::get_string()
  {
    char* ptr;
    if ( not ( this->arg_type() == TYPE_STRING or this->arg_type() == TYPE_OBJECT_PATH or this->arg_type() == TYPE_SIGNATURE ) )
      throw ErrorInvalidTypecast::create("MessageIterator: getting char* and type is not one of TYPE_STRING, TYPE_OBJECT_PATH or TYPE_SIGNATURE");
    dbus_message_iter_get_basic( &m_cobj, &ptr );
    return ptr;
  }

//   void MessageIterator::value( Variant& temp )
//   {
// 
//     switch ( this->arg_type() ) {
//       case TYPE_BYTE: {
//         uint8_t x;
//         this->value( x );
//         temp = x;
//       }
//       return;
// 
//       case TYPE_BOOLEAN: {
//         bool x;
//         this->value( x );
//         temp = x;
//       }
//       return;
// 
//       case TYPE_INT16: {
//         int16_t x;
//         this->value( x );
//         temp = x;
//       }
//       return;
// 
//       case TYPE_UINT16: {
//         uint16_t x;
//         this->value( x );
//         temp = x;
//       }
//       return;
// 
//       case TYPE_INT32: {
//         int32_t x;
//         this->value( x );
//         temp = x;
//       }
//       return;
// 
//       case TYPE_UINT32: {
//         uint32_t x;
//         this->value( x );
//         temp = x;
//       }
//       return;
// 
//       case TYPE_INT64: {
//         int64_t x;
//         this->value( x );
//         temp = x;
//       }
//       return;
// 
//       case TYPE_UINT64: {
//         uint64_t x;
//         this->value( x );
//         temp = x;
//       }
//       return;
// 
//       case TYPE_DOUBLE: {
//         double x;
//         this->value( x );
//         temp = x;
//       }
//       return;
// 
//       case TYPE_STRING: {
//         std::string x;
//         this->value( x );
//         temp = x;
//       }
//       return;
// 
//       case TYPE_OBJECT_PATH: {
//         Path x;
//         this->value( x );
//         temp = x;
//       }
//       return;
// 
//       case TYPE_SIGNATURE: {
//         Signature x;
//         this->value( x );
//         temp = x;
//       }
//       return;
//     }
// 
//     throw ErrorInvalidTypecast( "MessageIterator: Extracting non-primitive DBus type into variant" );
//   }

//   bool MessageIterator::append_array( char type, const void* ptr, size_t length )
//   {
//     return dbus_message_iter_append_fixed_array( ( DBusMessageIter* ) & _iter, type, ptr, length );
//   }

//   int MessageIterator::get_array( void* ptr )
//   {
//     int length;
//     dbus_message_iter_get_fixed_array( ( DBusMessageIter* ) & _iter, ptr, &length );
//     return length;
//   }

//   MessageIterator MessageIterator::new_array( const char* sig )
//   {
//     MessageIterator arr( msg() );
//     dbus_message_iter_open_container(
//         ( DBusMessageIter* ) & _iter, DBUS_TYPE_ARRAY, sig, ( DBusMessageIter* ) & ( arr._iter )
//                                     );
//     return arr;
//   }

//   MessageIterator MessageIterator::new_variant( const char* sig )
//   {
//     MessageIterator var( msg() );
//     dbus_message_iter_open_container(
//         ( DBusMessageIter* ) _iter, DBUS_TYPE_VARIANT, sig, ( DBusMessageIter* ) & ( var._iter )
//                                     );
//     return var;
//   }

//   MessageIterator MessageIterator::new_struct()
//   {
//     MessageIterator stu( msg() );
//     dbus_message_iter_open_container(
//         ( DBusMessageIter* ) _iter, DBUS_TYPE_STRUCT, NULL, ( DBusMessageIter* ) & ( stu._iter )
//                                     );
//     return stu;
//   }

//   MessageIterator MessageIterator::new_dict_entry()
//   {
//     MessageIterator ent( msg() );
//     dbus_message_iter_open_container(
//         ( DBusMessageIter* ) _iter, DBUS_TYPE_DICT_ENTRY, NULL, ( DBusMessageIter* ) & ( ent._iter )
//                                     );
//     return ent;
//   }

//   void MessageIterator::close_container( MessageIterator& container )
//   {
//     dbus_message_iter_close_container( ( DBusMessageIter* ) & _iter, ( DBusMessageIter* ) & ( container._iter ) );
//   }

}


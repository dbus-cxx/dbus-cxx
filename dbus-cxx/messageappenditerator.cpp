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
#include <dbus-cxx/variant.h>
#include <dbus-cxx/message.h>
#include <any>
#include <stdint.h>
#include <limits>
#include <cstring>
#include "enums.h"
#include "filedescriptor.h"
#include "message.h"
#include "signature.h"
#include "types.h"
#include "validator.h"

namespace DBus
{

  MessageAppendIterator::MessageAppendIterator( ContainerType container ):
      m_message( nullptr ),
      m_subiter( nullptr ),
      m_currentContainer( container )
  {
  }

  MessageAppendIterator::MessageAppendIterator( Message& message, ContainerType container ):
      m_marshaling( &(message.m_body), Endianess::Big ),
      m_message( &message ),
      m_subiter( nullptr ),
      m_currentContainer( container )
  {
      if( container != ContainerType::None ){
          m_marshaling = Marshaling( &m_workingBuffer, Endianess::Big );
      }
  }

  MessageAppendIterator::MessageAppendIterator( std::shared_ptr<Message> message, ContainerType container ):
      m_message( message.get() ),
      m_subiter( nullptr )
  {
      if( message ){
          m_marshaling = Marshaling( &(message->m_body), Endianess::Big );
      }
      if( container != ContainerType::None ){
          m_marshaling = Marshaling( &m_workingBuffer, Endianess::Big );
      }
  }

  MessageAppendIterator::~MessageAppendIterator()
  {
//     if ( m_subiter ) this->close_container();
  }

  void MessageAppendIterator::invalidate()
  {
    m_message = nullptr;
    m_subiter = nullptr;
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
    if ( not this->is_valid() ) return *this;

    if( m_currentContainer == ContainerType::None ){
        m_message->append_signature( signature( v ) );
    }
    m_marshaling.marshal( v );

    return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const int8_t& v ){
    if ( not this->is_valid() ) return *this;

    if( m_currentContainer == ContainerType::None ){
        m_message->append_signature( signature( v ) );
    }
    m_marshaling.marshal( v );

    return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const uint8_t& v ){
      if ( not this->is_valid() ) return *this;

      if( m_currentContainer == ContainerType::None ){
          m_message->append_signature( signature( v ) );
      }
      m_marshaling.marshal( v );

      return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const int16_t& v ){
      if ( not this->is_valid() ) return *this;

      if( m_currentContainer == ContainerType::None ){
          m_message->append_signature( signature( v ) );
      }
      m_marshaling.marshal( v );

      return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const uint16_t& v ){
      if ( not this->is_valid() ) return *this;

      if( m_currentContainer == ContainerType::None ){
          m_message->append_signature( signature( v ) );
      }
      m_marshaling.marshal( v );

      return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const int32_t& v ){
      if ( not this->is_valid() ) return *this;

      if( m_currentContainer == ContainerType::None ){
          m_message->append_signature( signature( v ) );
      }
      m_marshaling.marshal( v );

      return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const uint32_t& v ){
      if ( not this->is_valid() ) return *this;

      if( m_currentContainer == ContainerType::None ){
          m_message->append_signature( signature( v ) );
      }
      m_marshaling.marshal( v );

      return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const int64_t& v ){
      if ( not this->is_valid() ) return *this;

      if( m_currentContainer == ContainerType::None ){
          m_message->append_signature( signature( v ) );
      }
      m_marshaling.marshal( v );

      return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const uint64_t& v ){
      if ( not this->is_valid() ) return *this;

      if( m_currentContainer == ContainerType::None ){
          m_message->append_signature( signature( v ) );
      }
      m_marshaling.marshal( v );

      return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const double& v ){
      if ( not this->is_valid() ) return *this;

      if( m_currentContainer == ContainerType::None ){
          m_message->append_signature( signature( v ) );
      }
      m_marshaling.marshal( v );

      return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const float& v ){
    return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const char* v ){
    uint32_t len = strlen( v ) & UINT32_MAX;

    if ( not this->is_valid() ) return *this;

    if( m_currentContainer == ContainerType::None ){
        m_message->append_signature( signature( std::string() ) );
    }

    m_marshaling.marshal( std::string( v, len ) );

    return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const std::string& v ){
      if ( not this->is_valid() ) return *this;

      if( m_currentContainer == ContainerType::None ){
          m_message->append_signature( signature( std::string() ) );
      }

      m_marshaling.marshal( v );
      return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const Signature& v ){

    if ( not this->is_valid() ) return *this;

    std::string realSig = v.str();
    if( realSig.length() > UINT8_MAX ){
        m_message->invalidate();
        return *this;
    }

    if( m_currentContainer == ContainerType::None ){
        m_message->append_signature( signature( v ) );
    }
    m_marshaling.marshal( static_cast<uint8_t>( realSig.length() & 0xFF ) );
    for( const char& c : realSig ){
        m_marshaling.marshal( static_cast<uint8_t>( c ) );
    }
    m_marshaling.marshal( static_cast<uint8_t>( 0 ) );

    return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const Path& v ){
    if ( not this->is_valid() ) return *this;

    if( m_currentContainer != ContainerType::None ){
        m_message->append_signature( signature( v ) );
    }
    m_marshaling.marshal( v );

    return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const std::shared_ptr<FileDescriptor> v ){
    bool result;
    int raw_fd;

    if ( not this->is_valid() ) return *this;
    if ( not v ) return *this;

    raw_fd = v->getDescriptor();
    //result = dbus_message_iter_append_basic( &m_cobj, DBus::typeToDBusType( DataType::UNIX_FD ), &raw_fd );

    if ( ! result ) m_message->invalidate();

    return *this;
  }

  MessageAppendIterator& MessageAppendIterator::operator<<( const Variant& v ){
    if ( not this->is_valid() ) return *this;
    if( v.currentType() == DataType::INVALID ){
        return *this;
    }

    this->open_container( ContainerType::VARIANT, v.signature()  );
    *m_subiter << v;
    this->close_container();

    return *this;
  }


  bool MessageAppendIterator::open_container( ContainerType t, const std::string& sig )
  {
    std::string signature;
    int32_t array_align = 0;

    switch( t ){
    case ContainerType::None:
        break;
    case ContainerType::STRUCT:
        signature.append( "(" );
        signature.append( sig );
        signature.append( ")" );
        break;
    case ContainerType::ARRAY:
        signature.append( "a" );
        signature.append( sig );
        // The size will get marshalled once we close the container
    {
        SignatureIterator tmpSigIter( sig );
        TypeInfo ti( tmpSigIter.type() );
        array_align = ti.alignment();
    }
        break;
    case ContainerType::VARIANT:
        signature.append( "v" );
        break;
    case ContainerType::DICT_ENTRY:
        signature.append( "{" );
        signature.append( sig );
        signature.append( "}" );
        break;
    }

    if ( m_subiter ) this->close_container();

    if ( m_message ){
        m_message->append_signature( signature );
      m_subiter = new MessageAppendIterator( *m_message, t );
      m_subiter->m_arrayAlignment = array_align;
    } else
      m_subiter = new MessageAppendIterator( t );

    return true;
  }

  bool MessageAppendIterator::close_container( )
  {
    if ( ! m_subiter ) return false;

    switch( m_subiter->m_currentContainer ){
    case ContainerType::None: return false;
    case ContainerType::ARRAY:
    {
        uint32_t arraySize = static_cast<uint32_t>( m_subiter->m_workingBuffer.size() );
        if( arraySize > Validator::maximum_array_size() ){
            m_message->invalidate();
            return true;
        }
        m_marshaling.marshal( arraySize );
        m_marshaling.align( m_subiter->m_arrayAlignment );
    }
        break;
    case ContainerType::DICT_ENTRY:
    case ContainerType::STRUCT:
        m_marshaling.align( 8 );
        break;
    case ContainerType::VARIANT:
        break;
    }

    for( const uint8_t& dataByte : m_subiter->m_workingBuffer ){
        m_marshaling.marshal( dataByte );
    }
    delete m_subiter;
    m_subiter = nullptr;
    return true;
  }

  MessageAppendIterator* MessageAppendIterator::sub_iterator()
  {
    return m_subiter;
  }

}


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
#include "message.h"
#include <dbus/dbus.h>
#include "messageappenditerator.h"
#include "messageiterator.h"
#include "returnmessage.h"
#include "callmessage.h"
#include "errormessage.h"
#include "signalmessage.h"
#include "variant.h"
#include "marshaling.h"
#include "demarshaling.h"
#include <dbus-cxx/dbus-cxx-private.h>
#include <dbus-cxx/simplelogger.h>

static const char* LOGGER_NAME = "DBus.Message";

namespace DBus
{

  Message::Message( MessageType type ): m_valid(false)
  {
    m_cobj = dbus_message_new( static_cast<int>( type ) );
  }

  Message::Message( DBusMessage* cobj, CreateMethod m ): m_valid(false)
  {
    if ( cobj == nullptr )
    {
      m_cobj = cobj;
    }
    else if ( m == CreateMethod::ALIAS )
    {
      m_cobj = cobj;
      m_valid = true;
      dbus_message_ref(m_cobj);
    }
    else
    {
      m_cobj = dbus_message_copy(cobj);
      m_valid = true;
    }
  }

  Message::Message( std::shared_ptr<Message> other, CreateMethod m ): m_cobj(nullptr), m_valid(false)
  {
    if ( other and other->m_cobj != nullptr )
    {
      if ( m == CreateMethod::ALIAS )
      {
        m_cobj = other->m_cobj;
        dbus_message_ref(m_cobj);
      }
      else
      {
        m_cobj = dbus_message_copy(other->m_cobj);
      }
    }
  }

  Message::Message( std::shared_ptr<const Message> other, CreateMethod m ): m_cobj(nullptr), m_valid(false)
  {
    if ( other and other->m_cobj != nullptr )
    {
      if ( m == CreateMethod::ALIAS )
      {
        m_cobj = other->m_cobj;
        dbus_message_ref(m_cobj);
      }
      else
      {
        m_cobj = dbus_message_copy(other->m_cobj);
      }
    }
  }

  MessageType Message::type() const {
      return MessageType::INVALID;
  }

  std::shared_ptr<Message> Message::create(MessageType type)
  {
    return std::shared_ptr<Message>(new Message(type) );
  }

  std::shared_ptr<Message> Message::create(DBusMessage * cobj, CreateMethod m)
  {
    return std::shared_ptr<Message>(new Message(cobj, m) );
  }

  std::shared_ptr<Message> Message::create(std::shared_ptr<Message> other, CreateMethod m)
  {
    return std::shared_ptr<Message>(new Message(other, m) );
  }

  std::shared_ptr<Message> Message::create(std::shared_ptr<const Message> other, CreateMethod m)
  {
    return std::shared_ptr<Message>(new Message(other, m) );
  }

  Message::~Message()
  {
    if ( m_cobj ) dbus_message_unref( m_cobj );
  }

  Message& Message::operator = ( const Message& m )
  {
    if ( m_cobj != nullptr )
      dbus_message_unref( m_cobj );
    m_cobj = m.m_cobj;
    if ( m_cobj != nullptr ) {
      dbus_message_ref( m_cobj );
    }
    return *this;
  }

  bool Message::operator == ( const Message& other )
  {
    return m_cobj == other.m_cobj;
  }

  bool Message::is_valid() const
  {
    // TODO fix this
//     return ( m_cobj != nullptr and m_valid );
    return m_valid;
  }

  void Message::invalidate()
  {
    m_valid = false;
  }

  Message::operator bool() const
  {
    return this->is_valid();
  }

  uint32_t Message::serial() const
  {
      Variant value = header_field( MessageHeaderFields::Reply_Serial );
      if( value.currentType() != DataType::INVALID ){
          return std::any_cast<uint32_t>( value.value() );
      }
    return 0;
  }

//   Message Message::copy()
//   {
    // TODO fix
//     return Message( *this, CreateMethod::COPY );
//   }

  void Message::set_auto_start( bool auto_start)
  {
    if ( m_cobj == nullptr ) return;
    dbus_message_set_auto_start( m_cobj, auto_start );
  }

  bool Message::auto_start()
  {
    if ( m_cobj == nullptr ) return false;
    return dbus_message_get_auto_start( m_cobj );
  }

  bool Message::set_destination( const std::string& s )
  {
    if ( m_cobj == nullptr ) return false;
    m_headerMap[ MessageHeaderFields::Destination ] = DBus::Variant( s );
    return dbus_message_set_destination( m_cobj, s.c_str() );
  }

  const char* Message::destination() const
  {
    if ( m_cobj == nullptr ) return NULL;
    return dbus_message_get_destination( m_cobj );
  }

  bool Message::set_sender( const std::string& s )
  {
    if ( m_cobj == nullptr ) return false;
    return dbus_message_set_sender( m_cobj, s.c_str() );
  }

  const char* Message::sender() const
  {
    if ( m_cobj == nullptr ) return NULL;
    return dbus_message_get_sender( m_cobj );
  }

  bool Message::is_call( const std::string& interface, const std::string& method ) const
  {
    if ( m_cobj == nullptr ) return false;
    return dbus_message_is_method_call( m_cobj, interface.c_str(), method.c_str() );
  }

  bool Message::is_signal( const std::string& interface, const std::string& signal_name ) const
  {
    if ( m_cobj == nullptr ) return false;
    return dbus_message_is_signal( m_cobj, interface.c_str(), signal_name.c_str() );
  }

  bool Message::is_error( const std::string& error_name ) const
  {
    if ( m_cobj == nullptr ) return false;
    return dbus_message_is_error( m_cobj, error_name.c_str() );
  }

  bool Message::has_destination( const std::string& name ) const
  {
    if ( m_cobj == nullptr ) return false;
    return dbus_message_has_destination( m_cobj, name.c_str() );
  }

  bool Message::has_sender( const std::string& name ) const
  {
    if ( m_cobj == nullptr ) return false;
    return dbus_message_has_sender( m_cobj, name.c_str() );
  }

  MessageIterator Message::begin() const
  {
    return MessageIterator( *this );
  }

  MessageIterator Message::end() const
  {
    return MessageIterator();
  }

  MessageAppendIterator Message::append()
  {
    return MessageAppendIterator( *this );
  }

  DBusMessage* Message::cobj( ) const
  {
    return m_cobj;
  }

  Signature Message::signature() const
  {
      Variant v = header_field( MessageHeaderFields::Signature );
      if( v.currentType() == DataType::SIGNATURE ){
          return std::any_cast<Signature>( v.value() );
      }
    return Signature();
  }

  bool Message::has_signature( const std::string& signature ) const
  {
    //return dbus_message_has_signature( m_cobj, signature.c_str() );
      return false;
  }

bool Message::serialize_to_vector( std::vector<uint8_t>* vec, uint32_t serial ) const {
    Marshaling marshal( vec, Endianess::Big );
    Variant serialHeader = header_field( MessageHeaderFields::Reply_Serial );
    bool mustHaveSerial = false;

	vec->reserve( m_body.size() + 256 );
    marshal.marshal( static_cast<uint8_t>( 'B' ) );
    switch( type() ){
    case MessageType::INVALID:
        return false;
    case MessageType::CALL:
        marshal.marshal( static_cast<uint8_t>( 1 ) );
		break;
    case MessageType::RETURN:
        marshal.marshal( static_cast<uint8_t>( 2 ) );
        mustHaveSerial = true;
		break;
    case MessageType::ERROR:
        marshal.marshal( static_cast<uint8_t>( 3 ) );
        mustHaveSerial = true;
		break;
    case MessageType::SIGNAL:
        marshal.marshal( static_cast<uint8_t>( 4 ) );
		break;
	}

	// Now marshal the flags
    marshal.marshal( m_flags );

	// Marshal the protocol version
    marshal.marshal( static_cast<uint8_t>( 1 ) );

	// Marshal the length
    marshal.marshal( static_cast<uint32_t>( m_body.size() ) );

    // Marshal the serial.  If the header field 'serial' exists,
    // use that instead of the passed-in serial.  This means that we may skip serial numbers,
    // but that shouldn't cause an issue
    if( mustHaveSerial ){
        if( serialHeader.currentType() == DataType::UINT32 ){
            serial = std::any_cast<uint32_t>( serialHeader );
        }else{
            SIMPLELOGGER_ERROR( LOGGER_NAME, "Unable to serialize message: reply serial required but not found!" );
            return false;
        }
    }else{
        marshal.marshal( serial );
    }

    // Marshal our header array
    marshal.marshal( static_cast<uint32_t>( 0 ) ); // The size of the header array; we update this later

    for( const std::pair<const MessageHeaderFields,Variant>& entry : m_headerMap ){
        if( entry.second.currentType() == DataType::INVALID ) continue;

        marshal.align( 8 );
        marshal.marshal( header_field_to_int( entry.first ) );
        marshal.marshal( entry.second );
	}
    // The size of the header array is always at offset 12
    marshal.marshalAtOffset( 12, static_cast<uint32_t>( vec->size() ) - 16 );

    // Align the message data to an 8-byte boundary and add the data!
    marshal.align( 8 );

    for( const uint8_t& byte : m_body ){
        vec->push_back( byte );
    }

    return true;
}

std::shared_ptr<Message> Message::create_from_data( uint8_t* data, uint32_t data_len ){
    Demarshaling demarshal( data, data_len, Endianess::Big );
    uint8_t method_type;
    uint8_t flags;
    uint8_t protoVersion;
    uint32_t bodyLen;
    uint32_t serial;
    uint32_t arrayLen;
    std::shared_ptr<Message> retmsg;
    std::map<MessageHeaderFields,Variant> headerMap;
    Endianess msgEndian = Endianess::Big;

    if( demarshal.demarshal_uint8_t() == 'l' ){
        demarshal.setEndianess( Endianess::Little );
        msgEndian = Endianess::Little;
    }

    method_type = demarshal.demarshal_uint8_t();
    flags = demarshal.demarshal_uint8_t();
    protoVersion = demarshal.demarshal_uint8_t();
    bodyLen = demarshal.demarshal_uint32_t();
    serial = demarshal.demarshal_uint32_t();
    arrayLen = demarshal.demarshal_uint32_t();

    while( demarshal.current_offset() < ( 12 + arrayLen ) ){
        uint8_t key_demarshal;
        MessageHeaderFields key;
        Variant value;
        demarshal.align( 8 );
        key_demarshal = demarshal.demarshal_uint8_t();
        value = demarshal.demarshal_variant();

        key = int_to_header_field( key_demarshal );
        if( key == MessageHeaderFields::Invalid ){
            std::ostringstream logmsg;
            logmsg << "Found invalid header field "
                   << static_cast<int>( key )
                   << " when parsing; ignoring.  Value: "
                   << value;
            SIMPLELOGGER_WARN( LOGGER_NAME, logmsg.str() );
            continue;
        }

        headerMap[ key ] = value;
    }

    // Make sure we're aligned to an 8-byte boundary
    demarshal.align( 8 );

    switch( method_type ){
    case 1:
        retmsg = CallMessage::create();
        break;
    case 2:
        retmsg = ReturnMessage::create();
        break;
    case 3:
        retmsg = ErrorMessage::create();
        break;
    case 4:
        retmsg = SignalMessage::create();
        break;
    }

    retmsg->m_flags = flags;
    retmsg->m_valid = true;
    retmsg->m_headerMap = headerMap;
    retmsg->m_endianess = msgEndian;
    retmsg->m_body.reserve( bodyLen );
    for( uint32_t x = 0; x < bodyLen; x++ ){
        retmsg->m_body.push_back( demarshal.demarshal_uint8_t() );
    }

    return retmsg;
}

void Message::append_signature(std::string toappend){
    DBus::Variant val = m_headerMap[ MessageHeaderFields::Signature ];
    std::string newval;

    if( val.currentType() != DataType::INVALID ){
        Signature sig = std::any_cast<Signature>( val.value() );
        newval += sig.str();
    }

    newval += toappend;
    m_headerMap[ MessageHeaderFields::Signature ] = DBus::Variant( Signature( newval ) );
}

Variant Message::header_field( MessageHeaderFields field ) const {
    std::map<MessageHeaderFields,Variant>::const_iterator location =
            m_headerMap.find( field );
    if( location != m_headerMap.end() ){
        return location->second;
    }

    return DBus::Variant();
}

void Message::clear_sig_and_data(){
    std::map<MessageHeaderFields,Variant>::const_iterator location =
            m_headerMap.find( MessageHeaderFields::Signature );
    if( location != m_headerMap.end() ){
        m_headerMap.erase( location );
    }
    m_body.clear();
}

std::ostream& operator <<( std::ostream& os, const DBus::Message* msg ){
    os << "DBus::Message = [";

    switch( msg->type() ){
    case DBus::MessageType::INVALID:
        os << "Invalid";
        break;
    case DBus::MessageType::CALL:
        os << "CallMessage";
        break;
    case DBus::MessageType::ERROR:
        os << "ErrorMessage";
        break;
    case DBus::MessageType::RETURN:
        os << "ReturnMessage";
        break;
    case DBus::MessageType::SIGNAL:
        os << "SignalMessage";
        break;
    }

    os << std::endl;
    os << "  Message length: " << msg->m_body.size() << std::endl;
    os << "  Endianess: " << msg->m_endianess << std::endl;
    os << "  Headers:" << std::endl;
    for( const std::pair<const MessageHeaderFields,DBus::Variant>& set : msg->m_headerMap ){
        std::any value = set.second.value();
        os << "    ";
        switch( set.first ){
        case MessageHeaderFields::Invalid:
            os << "!! Invalid message header has been stored !!";
            break;
        case MessageHeaderFields::Path:
            os << "Path: " << std::any_cast<Path>( value );
            break;
        case MessageHeaderFields::Interface:
            os << "Interface: " << std::any_cast<std::string>( value );
            break;
        case MessageHeaderFields::Member:
            os << "Member: " << std::any_cast<std::string>( value );
            break;
        case MessageHeaderFields::Error_Name:
            os << "Error Name: " << std::any_cast<std::string>( value );
            break;
        case MessageHeaderFields::Reply_Serial:
            os << "Reply Serial: " << std::any_cast<uint32_t>( value );
            break;
        case MessageHeaderFields::Destination:
            os << "Destination: " << std::any_cast<std::string>( value );
            break;
        case MessageHeaderFields::Sender:
            os << "Sender: " << std::any_cast<std::string>( value );
            break;
        case MessageHeaderFields::Signature:
            os << "Signature: " << std::any_cast<Signature>( value );
            break;
        case MessageHeaderFields::Unix_FDs:
            os << "# Unix FDs: " << std::any_cast<uint32_t>( value );
            break;
        }
        os << std::endl;
    }
    os << "]";

    return os;
}

}

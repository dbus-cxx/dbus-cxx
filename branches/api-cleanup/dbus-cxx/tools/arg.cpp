#include "arg.h"
#include "parsed_object.h"

#include <string.h>

Direction Arg::direction()
{
  if ( strcasecmp( direction_string.c_str(), "in" ) == 0 )
    return DIRECTION_IN;
  else if ( strcasecmp( direction_string.c_str(), "out" ) == 0 )
    return DIRECTION_OUT;
  return DIRECTION_NONE;
}

std::string Arg::name(){ 
  if ( cxx_name.empty() ) 
    return dbus_name; 
  return cxx_name; 
}

std::string Arg::cpp_type(ProxyAdapter pa)
{
  std::string typestr;
  
  if ( cpp_type_override.empty() ){
    //type = cppTypeFromSignature( signature );
    if ( not signature.begin().is_basic() ){
      std::string complexType;
      DBus::Signature::iterator it = signature.begin().recurse();
      typestr += type_string_from_code( signature.begin().type() );
      while( it.is_valid() ){
        if( it.is_array() ){
          complexType += type_string_from_code( it.type() );
        } else if( it.is_dict() ){
          complexType += type_string_from_code( it.type() );
          it.next();
          complexType += ",";
          complexType = type_string_from_code( it.type() );
        } else {
          complexType += type_string_from_code( it.type() );
        }
        typestr += "<" + complexType + ">";
        it.next();
      }
    } else {
      DBus::Signature::iterator it = signature.begin();
      typestr = type_string_from_code( it.type() );
    }
  }else typestr = cpp_type_override;

  if ( pa == PROXY_PARAM and mis_const ) typestr = "const " + typestr;
  if ( pa == PROXY_PARAM and mis_ref )   typestr = typestr + "&";

  return typestr;
}

std::string Arg::cpp_dbus_type()
{
  switch ( type() ) {
    case DBus::TYPE_INVALID:     throw DBus::ErrorInvalidMessageType::create();
    case DBus::TYPE_BYTE:        return "uint8_t";
    case DBus::TYPE_BOOLEAN:     return "bool";
    case DBus::TYPE_INT16:       return "int16_t";
    case DBus::TYPE_UINT16:      return "uint16_t";
    case DBus::TYPE_INT32:       return "int32_t";
    case DBus::TYPE_UINT32:      return "uint32_t";
    case DBus::TYPE_INT64:       return "int64_t";
    case DBus::TYPE_UINT64:      return "uint64_t";
    case DBus::TYPE_DOUBLE:      return "double";
    case DBus::TYPE_STRING:      return "::std::string";
    case DBus::TYPE_OBJECT_PATH: return "::DBus::Path";
    case DBus::TYPE_SIGNATURE:   return "::DBus::Signature";
    case DBus::TYPE_ARRAY:       return "::std::vector";
    case DBus::TYPE_VARIANT:     throw DBus::ErrorInvalidMessageType::create();
    case DBus::TYPE_STRUCT:      throw DBus::ErrorInvalidMessageType::create();
    case DBus::TYPE_DICT_ENTRY:  throw DBus::ErrorInvalidMessageType::create();
  }

  throw DBus::ErrorInvalidMessageType::create();
}

std::string Arg::stubsignature()
{
  return DBus::signature( type() );
}

DBus::Type Arg::type()
{
  if ( not signature.is_singleton() ) return DBus::TYPE_INVALID;
  //if ( not signature.begin().is_basic() ) return DBus::TYPE_INVALID;
  return signature.begin().type();
}

std::string Arg::strfmt( int depth )
{
  std::ostringstream sout;
  out_tabs( sout, depth );
  if ( direction() != DIRECTION_NONE )
    sout << "Arg: " << dbus_name << "\t\tType: " << signature << "\t\tDir: " << direction_string << std::endl;
  else
    sout << "Arg: " << dbus_name << "\t\tType: " << signature << std::endl;
  return sout.str();
}

// std::string Arg::cpp_cast(const std::string& var)
// {
//   if ( cpp_type_override.empty() ) return var;
//   return "static_cast< " + cpp_type_override + " >( " + var + " )";
// }
// 
// std::string Arg::dbus_cast(const std::string& var)
// {
//   if ( cpp_type_override.empty() ) return var;
//   return "static_cast< " + cpp_dbus_type() + " >( " + var + " )";
// }

void Arg::set_dbus_name( std::string name ){
  dbus_name = name;
}

void Arg::set_direction_string( std::string direction ){
  direction_string = direction;
}

void Arg::set_signature( std::string sig ) {
  DBus::Signature newSig( sig );
  signature = newSig;
}

void Arg::set_cpp_type_override( std::string type ){
  cpp_type_override = type;
}

void Arg::set_const( bool con ){
  mis_const = con;
}

void Arg::set_ref( bool ref ){
  mis_ref = ref;
}

bool Arg::is_const(){
  return mis_const;
}

bool Arg::is_ref(){
  return mis_ref;
}


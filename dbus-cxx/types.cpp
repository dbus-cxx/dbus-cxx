/***************************************************************************
 *   Copyright (C) 2019 by Robert Middleton.                               *
 *   robert.middleton@rm5248.com                                           *
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
#include <dbus-cxx/types.h>
#include <dbus-cxx/enums.h>
#include <string>
#include <vector>

namespace DBus {

  TypeInfo::TypeInfo( DataType d ) : m_type( d ){}

  bool TypeInfo::isTemplated() const {
    switch( m_type ){
      case DataType::ARRAY:
      case DataType::DICT_ENTRY:
      case DataType::STRUCT:
        return true;
    }
    return false;
  }

  std::vector<std::string> TypeInfo::includeFilesForType() const {
    std::vector<std::string> ret;

    switch ( m_type )
    {
      case DataType::BYTE:
      case DataType::INT16:
      case DataType::UINT16:
      case DataType::INT32:
      case DataType::UINT32:
      case DataType::INT64:
      case DataType::UINT64:
        ret.push_back( "stdint.h" );
        break;
      case DataType::STRING:
        ret.push_back( "string" );
        break;
      case DataType::ARRAY:
        ret.push_back( "vector" );
        break;
      case DataType::DICT_ENTRY:
        ret.push_back( "map" );
        break;
    }

    return ret;
  }

  std::string TypeInfo::cppType() const {
    switch ( m_type )
    {
      case DataType::BYTE:
        return "uint8_t";
      case DataType::BOOLEAN:
        return "bool";
      case DataType::INT16:
        return "int16_t";
      case DataType::UINT16:
        return "uint16_t";
      case DataType::INT32:
        return "int32_t";
      case DataType::UINT32:
        return "uint32_t";
      case DataType::INT64:
        return "int64_t";
      case DataType::UINT64:
        return "uint64_t";
      case DataType::DOUBLE:
        return "double";
      case DataType::STRING:
        return "std::string";
      case DataType::OBJECT_PATH:
        return "DBus::Path";
      case DataType::SIGNATURE:
        return "DBus::Signature";
      case DataType::ARRAY:
        return "std::vector";
      case DataType::VARIANT:
        return "VARIANT(need impl)";
      case DataType::STRUCT:
        return "std::tuple";
      case DataType::DICT_ENTRY:
        return "std::pair";
      case DataType::UNIX_FD:
        return "DBus::FileDescriptor";
    }
    return std::string("Invalid");
  }

}

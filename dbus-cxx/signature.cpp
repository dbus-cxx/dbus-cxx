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
#include "signature.h"
#include <dbus/dbus.h>
#include <stack>

#include "types.h"

namespace DBus
{

  const Signature::size_type npos = std::string::npos;

  Signature::Signature() : m_startingNode( nullptr )
  {
  }

  Signature::Signature( const std::string & s, size_type pos, size_type n ):
      m_signature( s, pos, n ),m_startingNode( nullptr )
  {
      bool ok = true;
      std::stack<ContainerType> containerStack;
      std::string::const_iterator it = m_signature.begin();
      m_startingNode = create_signature_tree( &it, &containerStack, &ok );
  }

  Signature::Signature( const char *s ):
      m_signature( s ), m_startingNode( nullptr )
  {
      bool ok = true;
      std::stack<ContainerType> containerStack;
      std::string::const_iterator it = m_signature.begin();
      m_startingNode = create_signature_tree( &it, &containerStack, &ok );
  }

  Signature::Signature( const char * s, size_type n ):
      m_signature( s,n ), m_startingNode( nullptr )
  {
      bool ok = true;
      std::stack<ContainerType> containerStack;
      std::string::const_iterator it = m_signature.begin();
      m_startingNode = create_signature_tree( &it, &containerStack, &ok );
  }

  Signature::Signature( size_type n, char c ):
      m_signature( n,c ), m_startingNode( nullptr )
  {
      bool ok = true;
      std::stack<ContainerType> containerStack;
      std::string::const_iterator it = m_signature.begin();
      m_startingNode = create_signature_tree( &it, &containerStack, &ok );
  }

  Signature::~Signature()
  {
  }

  Signature::operator const std::string &() const
  {
    return m_signature;
  }

  const std::string& Signature::str() const
  {
    return m_signature;
  }

  Signature & Signature::operator =(const std::string & s)
  {
    m_signature = s;
    return *this;
  }

  Signature & Signature::operator =(const char* s)
  {
    m_signature = s;
    return *this;
  }

  Signature::iterator Signature::begin()
  {
    return SignatureIterator( m_startingNode );
  }

  Signature::const_iterator Signature::begin() const
  {
    return SignatureIterator( m_startingNode );
  }

  Signature::iterator Signature::end()
  {
    return SignatureIterator( nullptr );
  }

  Signature::const_iterator Signature::end() const
  {
    return SignatureIterator( nullptr );
  }

  bool Signature::is_valid() const
  {
    if ( m_signature.empty() ) return false;
    return dbus_signature_validate( m_signature.c_str(), nullptr );
  }

  bool Signature::is_singleton() const
  {
    if ( m_signature.empty() ) return false;
    return dbus_signature_validate_single( m_signature.c_str(), nullptr );
  }

  priv::SignatureNode* Signature::create_signature_tree( std::string::const_iterator* it,
                                                            std::stack<ContainerType>* container_stack,
                                                            bool* ok){
      DataType tmpDataType;
      priv::SignatureNode* first = nullptr;
      priv::SignatureNode* current = nullptr;
      bool ending_container;

      if( container_stack->size() > 64 ){
          *ok = false;
          return nullptr;
      }

      if( *it == m_signature.cend() ){
          return nullptr;
      }

      do{
          tmpDataType = char_to_dbus_type( **it );
          ending_container = is_ending_container( **it );
          if( tmpDataType == DataType::INVALID ){
              *ok = false;
              return nullptr;
          }

          if( ending_container ){
              if( container_stack->size() == 0 ){
                  *ok = false;
                  return nullptr;
              }
              ContainerType currentTop = container_stack->top();
              if( currentTop == ContainerType::STRUCT &&
                      tmpDataType == DataType::STRUCT ){
                container_stack->pop();
                return first;
              }else if( currentTop == ContainerType::DICT_ENTRY &&
                      tmpDataType == DataType::DICT_ENTRY ){
                  container_stack->pop();
                  return first;
              }else{
                  *ok = false;
                  return nullptr;
              }
          }


          priv::SignatureNode* newnode = new priv::SignatureNode( tmpDataType );
          if( current != nullptr ){
              current->m_next = newnode;
              current = newnode;
          }

          if( first == nullptr ){
              first = newnode;
              current = newnode;
          }

          TypeInfo ti( tmpDataType );
          if( !container_stack->empty() &&
                  container_stack->top() == ContainerType::ARRAY &&
                  ti.is_basic() ){
            // We just added this basic element to the array
              break;
          }

          if( !container_stack->empty() &&
                  container_stack->top() == ContainerType::ARRAY &&
                  ti.is_container() &&
                  ending_container ){
            // We just added this container element to the array
              break;
          }

          if( ti.is_container() &&
                  tmpDataType != DataType::VARIANT ){
              container_stack->push( char_to_container_type( **it ) );
              (*it)++;
            current->m_sub = create_signature_tree( it, container_stack, ok );
            if( tmpDataType == DataType::ARRAY ){
                (*it)++;
                break;
            }
          }

          (*it)++;
      }while( *it != m_signature.cend() );

      return first;
  }


  void Signature::print_tree( std::ostream *stream ) const {
      DBus::priv::SignatureNode* current = m_startingNode;

      while( current != nullptr ){
          *stream << current->m_dataType;
          current = current->m_next;
          if( current == nullptr ){
              *stream << " (null) ";
          }else{
              *stream << " --> ";
          }
      }
  }

  void Signature::print_node( std::ostream* stream, priv::SignatureNode* node, int spaces ) const {
      if( node == nullptr ){
          return;
      }

      //if( node->)
      for( int x = 0; x < spaces; x++ ){
          *stream << " ";
      }

      *stream << node->m_dataType;
  }

}


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
#include "dbus-cxx-private.h"

#include "types.h"

static const char* LOGGER_NAME = "DBus.Signature";

namespace DBus
{

  const Signature::size_type npos = std::string::npos;

  Signature::Signature() : m_startingNode( nullptr ),
      m_valid( false )
  {
  }

  Signature::Signature( const std::string & s, size_type pos, size_type n ):
      m_signature( s, pos, n ),m_startingNode( nullptr )
  {
      initialize();
  }

  Signature::Signature( const char *s ):
      m_signature( s ), m_startingNode( nullptr )
  {
      initialize();
  }

  Signature::Signature( const char * s, size_type n ):
      m_signature( s,n ), m_startingNode( nullptr )
  {
      initialize();
  }

  Signature::Signature( size_type n, char c ):
      m_signature( n,c ), m_startingNode( nullptr )
  {
      initialize();
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
      if( !m_valid ) return SignatureIterator();
    return SignatureIterator( m_startingNode );
  }

  Signature::const_iterator Signature::begin() const
  {
      if( !m_valid ) return SignatureIterator();
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
    return m_valid;
  }

  bool Signature::is_singleton() const
  {
    return m_valid &&
            m_startingNode != nullptr &&
            m_startingNode->m_dataType != DataType::INVALID  &&
            m_startingNode->m_next == nullptr &&
            m_startingNode->m_sub == nullptr;
  }

  std::shared_ptr<priv::SignatureNode> Signature::create_signature_tree( std::string::const_iterator* it,
                                                            std::stack<ContainerType>* container_stack,
                                                            bool* ok){
      DataType tmpDataType;
      std::shared_ptr<priv::SignatureNode> first = nullptr;
      std::shared_ptr<priv::SignatureNode> current = nullptr;
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
                return first;
              }else if( currentTop == ContainerType::DICT_ENTRY &&
                      tmpDataType == DataType::DICT_ENTRY ){
                  return first;
              }else{
                  *ok = false;
                  return nullptr;
              }
          }


          std::shared_ptr<priv::SignatureNode> newnode = std::make_shared<priv::SignatureNode>( tmpDataType );
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
              (*it)++;
              break;
          }

          if( ti.is_container() &&
                  tmpDataType != DataType::VARIANT ){
              ContainerType toPush = char_to_container_type( **it );
              container_stack->push( toPush );
              (*it)++;
            current->m_sub = create_signature_tree( it, container_stack, ok );

            if( container_stack->top() != toPush ){
                // Unbalanced
                *ok = false;
                return nullptr;
            }

            // If we're the ending character of a container,
            // advance the iterator so we go to the next character
            ending_container = is_ending_container( **it );
            if( ending_container &&
                    toPush == ContainerType::STRUCT ){
                container_stack->pop();
                if( !container_stack->empty() ){
                    (*it)++;
                    return first;
                }

            }else if( ending_container &&
                      toPush == ContainerType::DICT_ENTRY ){
                container_stack->pop();
                (*it)++;
                return first;
            }else if( toPush == ContainerType::ARRAY &&
                      current->m_sub != nullptr ){
                // Note: need to be special about popping and advancing iterator
                // Assume we have 'aaid' as our signature.  When popping the array
                // off of our stack, we only need to advance the iterator once.
                // So basically, don't advance the iterator unless we're all the way
                // at the begining of the array.
                // This also means that we will either continue or break, depending on
                // if we are at the end of the signature or not, so we know if we need
                // to go on at all.
                bool isArrayEnd = true;

                container_stack->pop();
                if( !container_stack->empty() &&
                        container_stack->top() == ContainerType::ARRAY ){
                    isArrayEnd = false;
                }

                if( isArrayEnd && *it != m_signature.cend() ){
                    //(*it)++;
                    continue;
                }

                break;
            }else{
                *ok = false;
                return nullptr;
            }
          }

          if( *it != m_signature.cend() ) (*it)++;
      }while( *it != m_signature.cend() );

      return first;
  }


  void Signature::print_tree( std::ostream *stream ) const {
      std::shared_ptr<priv::SignatureNode> current = m_startingNode;

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

  void Signature::initialize(){
      m_valid = true;
      std::stack<ContainerType> containerStack;
      std::string::const_iterator it = m_signature.begin();
      m_startingNode = create_signature_tree( &it, &containerStack, &m_valid );

      if( !containerStack.empty() ||
              it != m_signature.end() ){
          SIMPLELOGGER_DEBUG( LOGGER_NAME, "Either stack not empty or signature not used up completely" );
          m_valid = false;
      }

      std::ostringstream logmsg;
      logmsg << "Signature \'" << m_signature << "\' is ";
      if( m_valid ){
          logmsg << "valid";
      }else{
          logmsg << "invalid";
      }

      SIMPLELOGGER_DEBUG( LOGGER_NAME, logmsg.str() );
  }

}


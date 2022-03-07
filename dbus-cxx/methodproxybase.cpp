// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
/***************************************************************************
 *   Copyright (C) 2009 by Rick L. Vinyard, Jr.                            *
 *   rvinyard@cs.nmsu.edu                                                  *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 ***************************************************************************/
#include "methodproxybase.h"
#include "callmessage.h"
#include "interfaceproxy.h"

namespace DBus {

class PendingCall;
class ReturnMessage;

class MethodProxyBase::priv_data {
public:
    priv_data( const std::string& name ) :
        m_interface( nullptr ),
        m_name( name ) {}

    InterfaceProxy* m_interface;
    const std::string m_name;
};


MethodProxyBase::MethodProxyBase( const std::string& name ):
    m_priv( std::make_unique<priv_data>( name ) ) {
}

MethodProxyBase::MethodProxyBase( const MethodProxyBase& other ) :
    m_priv( std::make_unique<priv_data>( other.m_priv->m_name ) ) {
    m_priv->m_interface = other.m_priv->m_interface;
}

std::shared_ptr<MethodProxyBase> MethodProxyBase::create( const std::string& name ) {
    return std::shared_ptr<MethodProxyBase>( new MethodProxyBase( name ) );
}

MethodProxyBase::~MethodProxyBase() {
}

InterfaceProxy* MethodProxyBase::interface_name() const {
    return m_priv->m_interface;
}

const std::string& MethodProxyBase::name() const {
    return m_priv->m_name;
}

std::shared_ptr<CallMessage> DBus::MethodProxyBase::create_call_message() const {
    if( !m_priv->m_interface ) { return std::shared_ptr<CallMessage>(); }

    std::shared_ptr<CallMessage> cm = m_priv->m_interface->create_call_message( m_priv->m_name );
    cm->set_no_reply( false );
    return cm;
}

std::shared_ptr<const ReturnMessage> DBus::MethodProxyBase::call( std::shared_ptr<const CallMessage> call_message, int timeout_milliseconds ) const {
    if( !m_priv->m_interface ) { return std::shared_ptr<const ReturnMessage>(); }

    return m_priv->m_interface->call( call_message, timeout_milliseconds );
}

//  std::shared_ptr<PendingCall> DBus::MethodProxyBase::call_async(std::shared_ptr<const CallMessage> call_message, int timeout_milliseconds) const
//  {
//    if ( not m_priv->m_interface ) return std::shared_ptr<PendingCall>();
//    return m_priv->m_interface->call_async(call_message, timeout_milliseconds);
//  }

void MethodProxyBase::set_interface( InterfaceProxy* proxy ) {
    m_priv->m_interface = proxy;
}

}


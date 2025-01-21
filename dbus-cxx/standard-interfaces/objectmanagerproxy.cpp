// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
/***************************************************************************
 *   Copyright (C) 2025 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 ***************************************************************************/
#include "objectmanagerproxy.h"
#include "../variant.h"
#include "../path.h"

using DBus::ObjectManagerProxy;

ObjectManagerProxy::ObjectManagerProxy() :
    InterfaceProxy( DBUS_CXX_OBJECT_MANAGER_INTERFACE )
{
    m_get_managed_objects_method = this->create_method<ObjectManagerObjects()>( "GetManagedObjects" );
}

std::shared_ptr<ObjectManagerProxy> ObjectManagerProxy::create(){
    return std::shared_ptr<ObjectManagerProxy>( new ObjectManagerProxy() );
}

DBus::ObjectManagerObjects ObjectManagerProxy::GetManagedObjects(){
    return (*m_get_managed_objects_method)();
}

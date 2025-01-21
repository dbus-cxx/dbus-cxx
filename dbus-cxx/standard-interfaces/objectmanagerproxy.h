// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
/***************************************************************************
 *   Copyright (C) 2025 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 ***************************************************************************/
#ifndef DBUSCXX_OBJECTMANAGER_PROXY_H
#define DBUSCXX_OBJECTMANAGER_PROXY_H

#include "../interfaceproxy.h"
#include <memory>

namespace DBus {

class ObjectManagerProxy : public InterfaceProxy {
private:
    ObjectManagerProxy();

public:
    static std::shared_ptr<ObjectManagerProxy> create();

    ObjectManagerObjects GetManagedObjects();

private:
    std::shared_ptr<MethodProxy<ObjectManagerObjects()>> m_get_managed_objects_method;
};

}

#endif // DBUSCXX_OBJECTMANAGER_PROXY_H

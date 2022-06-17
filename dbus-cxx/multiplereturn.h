// SPDX-License-Identifier: LGPL-3.0-or-later OR BSD-3-Clause
/***************************************************************************
 *   Copyright (C) 2022 by Christopher Schimp                              *
 *   silverchris@gmail.com                                                 *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 ***************************************************************************/
#ifndef DBUSCXX_MULTIPLERETURN_H
#define DBUSCXX_MULTIPLERETURN_H

#include <dbus-cxx/enums.h>
#include <dbus-cxx/path.h>
#include <dbus-cxx/signature.h>
#include <dbus-cxx/dbus-cxx-config.h>
#include <dbus-cxx/marshaling.h>
#include <dbus-cxx/types.h>
#include <dbus-cxx/error.h>
#include <string>
#include <cstdint>
#include <ostream>
#include <tuple>
#include <dbus-cxx/messageiterator.h>

namespace DBus {

    class MessageIterator;

    template<typename... Ts>
    class MultipleReturn {
    public:
        std::tuple<Ts...> m_data;

    public:
        MultipleReturn()= default;

        explicit MultipleReturn(Ts... args) {
            m_data = std::make_tuple(std::ref(args)...);
        };

        bool operator==(const MultipleReturn &other) const {
            return m_data == other.m_data;
        }

        MultipleReturn &operator=(const MultipleReturn &other) {
            m_data = other.m_data;
            return *this;
        }

    private:
        friend class MessageAppendIterator;
        friend class MessageIterator;
    };

}
#endif

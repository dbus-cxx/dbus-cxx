/***************************************************************************
 *   Copyright (C) 2007,2008,2009 by Rick L. Vinyard, Jr.                  *
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
#include "error.h"
#include "message.h"

namespace DBus {

Error::Error() {
}

Error::Error( const char* name, const char* message ) {
    m_name = std::string( name );

    if( message != nullptr ) {
        m_message = std::string( message );
    }
}

Error::Error( const char* name, std::string message ) {
    m_name = std::string( name );
    m_message = std::string( message );
}

Error::Error( std::string name, std::string message ) {
    m_name = std::string( name );
    m_message = std::string( message );
}

Error::~Error( ) noexcept {
}

std::string Error::name() const {
    return m_name;
}

std::string Error::message() const {
    return m_message;
}

const char* Error::what() const noexcept {
    return m_message.c_str();
}

}



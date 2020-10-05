/***************************************************************************
 *   Copyright (C) 2009,2010 by Rick L. Vinyard, Jr.                       *
 *   rvinyard@cs.nmsu.edu                                                  *
 *   Copyright (C) 2014- by Robert Middleton                               *
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

#ifndef DBUS_CXX_FILEDESCRIPTOR
#define DBUS_CXX_FILEDESCRIPTOR

#include <memory>

namespace DBus {

/**
 * A FileDescriptor holds a UNIX file descriptor that can be passed between processes.
 *
 */
class FileDescriptor {
protected:
    FileDescriptor() :
        m_valid( false ),
        m_fd( -1 ) {}

    explicit FileDescriptor( int fd ) :
        m_valid( true ),
        m_fd( fd ) {}

    explicit FileDescriptor( const FileDescriptor& other ) :
        m_valid( other.m_valid ),
        m_fd( other.m_fd ) {}

public:
    static std::shared_ptr<FileDescriptor> create( int fd ) {
        std::shared_ptr<FileDescriptor> p( new FileDescriptor( fd ) );
        return p;
    }

    ~FileDescriptor() {}

    int getDescriptor() const {
        return m_fd;
    }

    operator bool() const {
        return m_valid;
    }

private:
    bool m_valid;
    int m_fd;
};

}

#endif

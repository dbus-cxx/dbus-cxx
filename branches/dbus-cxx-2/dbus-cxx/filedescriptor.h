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

namespace DBus{

/**
 * A FileDescriptor holds a UNIX file descriptor that can be passed between processes.
 *
 */
class FileDescriptor{
protected:
	FileDescriptor(){
		m_fd = -1;
	}

        explicit FileDescriptor( int fd ){
            m_fd = fd;
        }

        explicit FileDescriptor( const FileDescriptor& other ){
            m_fd = other.m_fd;
        }

public:
      typedef DBusCxxPointer<FileDescriptor> pointer;

      static pointer create( int fd ){
          pointer p = pointer(new FileDescriptor( fd ));
          return p;
      }

	~FileDescriptor(){}

/*
    FileDescriptor& operator=( int fd ){
		m_fd = fd;
		return *this;
	}
*/

	int getDescriptor() const{
		return m_fd;
	}

/*
	void setDescriptor( int fd ){
		m_fd = fd;
	}
*/

private:
	int m_fd;
};

}

#endif

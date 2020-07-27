/***************************************************************************
 *   Copyright (C) 2020 by Robert Middleton                                *
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
/*
 * BSD socket emulation code for Winsock2
 * Brent Cook <bcook@openbsd.org>
 */
#ifndef DBUSCXX_WIN32NETCOMPAT_H
#define DBUSCXX_WIN32NETCOMPAT_H

#ifdef _WIN32

#include <ws2tcpip.h>
#include <errno.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif
int wsa_errno(int err);

int posix_connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen);

int posix_open(const char* path, ...);

int posix_close(int fd);

ssize_t posix_read(int fd, void* buf, size_t count);

ssize_t posix_write(int fd, const void* buf, size_t count);

int posix_getsockopt(int sockfd, int level, int optname,
	void* optval, socklen_t* optlen);

int posix_setsockopt(int sockfd, int level, int optname,
	const void* optval, socklen_t optlen);
#ifdef __cplusplus
}
#endif

#ifndef NO_REDEF_POSIX_FUNCTIONS
#define connect(sockfd, addr, addrlen) posix_connect(sockfd, addr, addrlen)
#define open(path, ...) posix_open(path, __VA_ARGS__)
#define close(fd) posix_close(fd)
#define read(fd, buf, count) posix_read(fd, buf, count)
#define write(fd, buf, count) posix_write(fd, buf, count)
#define getsockopt(sockfd, level, optname, optval, optlen) \
	posix_getsockopt(sockfd, level, optname, optval, optlen)
#define setsockopt(sockfd, level, optname, optval, optlen) \
	posix_setsockopt(sockfd, level, optname, optval, optlen)
#endif /* NO_REDEF_POSIX_FUNCTIONS */

#endif /* _WIN32 */

#endif /* DBUSCXX_WIN32NETCOMPAT_H */

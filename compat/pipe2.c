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
 * pipe2/pipe/socketpair emulation
 * Brent Cook <bcook@openbsd.org>
 */
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>

#undef socketpair

static int setfd(int fd, int flag)
{
	int rc = -1;
	if (flag & FD_CLOEXEC) {
		HANDLE h = (HANDLE)_get_osfhandle(fd);
		if (h != NULL)
			rc = SetHandleInformation(h, HANDLE_FLAG_INHERIT, 0) == 0 ? -1 : 0;
	}
	return rc;
}

static int setfl(int fd, int flag)
{
	int rc = -1;
	if (flag & O_NONBLOCK) {
		long mode = 1;
		rc = ioctlsocket(fd, FIONBIO, &mode);
	}
	return rc;
}

int socketpair(int domain, int type, int protocol, int socket_vector[2])
{
	if (domain != AF_UNIX || !(type & SOCK_STREAM) || protocol != PF_UNSPEC)
		return -1;

	socket_vector[0] = -1;
	socket_vector[1] = -1;

	int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listener == -1) {
		return -1;
	}

	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = htonl(INADDR_LOOPBACK),
		.sin_port = 0,
	};

	int yes = 1, e;
	if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
		(void*)&yes, sizeof yes) == -1)
		goto err;

	if (bind(listener, (struct sockaddr*)&addr, sizeof addr) != 0)
		goto err;

	memset(&addr, 0, sizeof addr);
	socklen_t addrlen = sizeof addr;
	if (getsockname(listener, (struct sockaddr*)&addr, &addrlen) != 0)
		goto err;

	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	addr.sin_family = AF_INET;

	if (listen(listener, 1) != 0)
		goto err;

	socket_vector[0] = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);
	if (socket_vector[0] == -1)
		goto err;

	if (connect(socket_vector[0], (struct sockaddr*)&addr, sizeof addr) != 0)
		goto err;

	socket_vector[1] = accept(listener, NULL, NULL);
	if (socket_vector[1] == -1)
		goto err;

	closesocket(listener);
	return 0;

err:
	e = WSAGetLastError();
	closesocket(listener);
	closesocket(socket_vector[0]);
	closesocket(socket_vector[1]);
	WSASetLastError(e);
	socket_vector[0] = -1;
	socket_vector[1] = -1;
	return -1;
}

int pipe(int fildes[2])
{
	return socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, PF_UNSPEC, fildes);
}

int pipe2(int fildes[2], int flags)
{
	int rc = pipe(fildes);
	if (rc == 0) {
		if (flags & O_NONBLOCK) {
			rc |= setfl(fildes[0], O_NONBLOCK);
			rc |= setfl(fildes[1], O_NONBLOCK);
		}
		if (flags & O_CLOEXEC) {
			rc |= setfd(fildes[0], FD_CLOEXEC);
			rc |= setfd(fildes[1], FD_CLOEXEC);
		}
		if (rc != 0) {
			int e = errno;
			close(fildes[0]);
			close(fildes[1]);
			errno = e;
		}
	}
	return rc;
}

int bsd_socketpair(int domain, int type, int protocol, int socket_vector[2])
{
	int flags = type & ~0xf;
	type &= 0xf;
	int rc = socketpair(domain, type, protocol, socket_vector);
	if (rc == 0) {
		if (flags & SOCK_NONBLOCK) {
			rc |= setfl(socket_vector[0], O_NONBLOCK);
			rc |= setfl(socket_vector[1], O_NONBLOCK);
		}
		if (flags & SOCK_CLOEXEC) {
			rc |= setfd(socket_vector[0], FD_CLOEXEC);
			rc |= setfd(socket_vector[1], FD_CLOEXEC);
		}
		if (rc != 0) {
			int e = errno;
			close(socket_vector[0]);
			close(socket_vector[1]);
			errno = e;
		}
	}
	return rc;
}

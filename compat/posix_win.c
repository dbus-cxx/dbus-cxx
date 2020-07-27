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
 * File IO compatibility shims
 * Brent Cook <bcook@openbsd.org>
 * Kinichiro Inoguchi <inoguchi@openbsd.org>
 */
#define NO_REDEF_POSIX_FUNCTIONS

#include <sys/types.h>
#include <win32netcompat.h>
#include <ws2tcpip.h>

#include <fcntl.h>
#include <stdio.h>
#include <windows.h>

int
wsa_errno(int err)
{
	switch (err) {
	case WSAENOBUFS:
		errno = ENOMEM;
		break;
	case WSAEACCES:
		errno = EACCES;
		break;
	case WSANOTINITIALISED:
		errno = EPERM;
		break;
	case WSAEHOSTUNREACH:
	case WSAENETDOWN:
		errno = EIO;
		break;
	case WSAEFAULT:
		errno = EFAULT;
		break;
	case WSAEINTR:
		errno = EINTR;
		break;
	case WSAEINVAL:
		errno = EINVAL;
		break;
	case WSAEINPROGRESS:
		errno = EINPROGRESS;
		break;
	case WSAEWOULDBLOCK:
		errno = EAGAIN;
		break;
	case WSAEOPNOTSUPP:
		errno = ENOTSUP;
		break;
	case WSAEMSGSIZE:
		errno = EFBIG;
		break;
	case WSAENOTSOCK:
		errno = ENOTSOCK;
		break;
	case WSAENOPROTOOPT:
		errno = ENOPROTOOPT;
		break;
	case WSAECONNREFUSED:
		errno = ECONNREFUSED;
		break;
	case WSAEAFNOSUPPORT:
		errno = EAFNOSUPPORT;
		break;
	case WSAEBADF:
		errno = EBADF;
		break;
	case WSAENETRESET:
	case WSAENOTCONN:
	case WSAECONNABORTED:
	case WSAECONNRESET:
	case WSAESHUTDOWN:
	case WSAETIMEDOUT:
		errno = EPIPE;
		break;
	}
	return -1;
}

uid_t getuid(void)
{
	/* Windows fstat sets 0 as st_uid */
	return 0;
}

int
posix_open(const char* path, ...)
{
	va_list ap;
	int mode = 0;
	int flags;

	va_start(ap, path);
	flags = va_arg(ap, int);
	if (flags & O_CREAT)
		mode = va_arg(ap, int);
	va_end(ap);

	flags |= O_BINARY;
	if (flags & O_CLOEXEC) {
		flags &= ~O_CLOEXEC;
		flags |= O_NOINHERIT;
	}
	flags &= ~O_NONBLOCK;
	return open(path, flags, mode);
}

int
posix_connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen)
{
	int rc = connect(sockfd, addr, addrlen);
	if (rc == SOCKET_ERROR)
		return wsa_errno(WSAGetLastError());
	return rc;
}

int
posix_close(int fd)
{
	if (closesocket(fd) == SOCKET_ERROR) {
		int err = WSAGetLastError();
		return (err == WSAENOTSOCK || err == WSAEBADF ||
			err == WSANOTINITIALISED) ?
			close(fd) : wsa_errno(err);
	}
	return 0;
}

ssize_t
posix_read(int fd, void* buf, size_t count)
{
	ssize_t rc = recv(fd, buf, count, 0);
	if (rc == SOCKET_ERROR) {
		int err = WSAGetLastError();
		return (err == WSAENOTSOCK || err == WSAEBADF ||
			err == WSANOTINITIALISED) ?
			read(fd, buf, count) : wsa_errno(err);
	}
	return rc;
}

ssize_t
posix_write(int fd, const void* buf, size_t count)
{
	ssize_t rc = send(fd, buf, count, 0);
	if (rc == SOCKET_ERROR) {
		int err = WSAGetLastError();
		return (err == WSAENOTSOCK || err == WSAEBADF ||
			err == WSANOTINITIALISED) ?
			write(fd, buf, count) : wsa_errno(err);
	}
	return rc;
}

int
posix_getsockopt(int sockfd, int level, int optname,
	void* optval, socklen_t* optlen)
{
	int rc = getsockopt(sockfd, level, optname, (char*)optval, optlen);
	return rc == 0 ? 0 : wsa_errno(WSAGetLastError());

}

int
posix_setsockopt(int sockfd, int level, int optname,
	const void* optval, socklen_t optlen)
{
	int rc = setsockopt(sockfd, level, optname, (char*)optval, optlen);
	return rc == 0 ? 0 : wsa_errno(WSAGetLastError());
}

#ifdef _MSC_VER
struct timezone;
int gettimeofday(struct timeval * tp, struct timezone * tzp)
{
	/*
	 * Note: some broken versions only have 8 trailing zero's, the correct
	 * epoch has 9 trailing zero's
	 */
	static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);

	SYSTEMTIME  system_time;
	FILETIME    file_time;
	uint64_t    time;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	time = ((uint64_t)file_time.dwLowDateTime);
	time += ((uint64_t)file_time.dwHighDateTime) << 32;

	tp->tv_sec = (long)((time - EPOCH) / 10000000L);
	tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
	return 0;
}

#endif /* _MSC_VER */

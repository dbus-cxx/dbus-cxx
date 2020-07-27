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

/* Written by Eric Blake <ebb9@byu.net>.  */

#include <fcntl.h>

#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/socket.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <io.h>

/* Upper bound on getdtablesize().  See lib/getdtablesize.c.  */
# define OPEN_MAX_MAX 0x10000

/* Duplicate OLDFD into the first available slot of at least NEWFD,
   which must be positive, with FLAGS determining whether the duplicate
   will be inheritable.  */
static int dupfd( int oldfd, int newfd, int flags )
{
  /* Mingw has no way to create an arbitrary fd.  Iterate until all
     file descriptors less than newfd are filled up.  */
  HANDLE curr_process = GetCurrentProcess ();
  HANDLE old_handle = (HANDLE) _get_osfhandle (oldfd);
  unsigned char fds_to_close[OPEN_MAX_MAX / CHAR_BIT];
  unsigned int fds_to_close_bound = 0;
  int result;
  BOOL inherit = flags & O_CLOEXEC ? FALSE : TRUE;
  int mode;

  if (newfd < 0 || getdtablesize () <= newfd)
    {
      errno = EINVAL;
      return -1;
    }
  if (old_handle == INVALID_HANDLE_VALUE
      || (mode = setmode (oldfd, O_BINARY)) == -1)
    {
      /* oldfd is not open, or is an unassigned standard file
         descriptor.  */
      errno = EBADF;
      return -1;
    }
  setmode (oldfd, mode);
  flags |= mode;

  for (;;)
    {
      HANDLE new_handle;
      int duplicated_fd;
      unsigned int index;

      if (!DuplicateHandle (curr_process,           /* SourceProcessHandle */
                            old_handle,             /* SourceHandle */
                            curr_process,           /* TargetProcessHandle */
                            (PHANDLE) &new_handle,  /* TargetHandle */
                            (DWORD) 0,              /* DesiredAccess */
                            inherit,                /* InheritHandle */
                            DUPLICATE_SAME_ACCESS)) /* Options */
        {
          switch (GetLastError ())
            {
              case ERROR_TOO_MANY_OPEN_FILES:
                errno = EMFILE;
                break;
              case ERROR_INVALID_HANDLE:
              case ERROR_INVALID_TARGET_HANDLE:
              case ERROR_DIRECT_ACCESS_HANDLE:
                errno = EBADF;
                break;
              case ERROR_INVALID_PARAMETER:
              case ERROR_INVALID_FUNCTION:
              case ERROR_INVALID_ACCESS:
                errno = EINVAL;
                break;
              default:
                errno = EACCES;
                break;
            }
          result = -1;
          break;
        }
      duplicated_fd = _open_osfhandle ((intptr_t) new_handle, flags);
      if (duplicated_fd < 0)
        {
          CloseHandle (new_handle);
          result = -1;
          break;
        }
      if (newfd <= duplicated_fd)
        {
          result = duplicated_fd;
          break;
        }

      /* Set the bit duplicated_fd in fds_to_close[].  */
      index = (unsigned int) duplicated_fd / CHAR_BIT;
      if (fds_to_close_bound <= index)
        {
          if (sizeof fds_to_close <= index)
            /* Need to increase OPEN_MAX_MAX.  */
            abort ();
          memset (fds_to_close + fds_to_close_bound, '\0',
                  index + 1 - fds_to_close_bound);
          fds_to_close_bound = index + 1;
        }
      fds_to_close[index] |= 1 << ((unsigned int) duplicated_fd % CHAR_BIT);
    }

  /* Close the previous fds that turned out to be too small.  */
  {
    int saved_errno = errno;
    unsigned int duplicated_fd;

    for (duplicated_fd = 0;
         duplicated_fd < fds_to_close_bound * CHAR_BIT;
         duplicated_fd++)
      if ((fds_to_close[duplicated_fd / CHAR_BIT]
           >> (duplicated_fd % CHAR_BIT))
          & 1)
        close (duplicated_fd);

    errno = saved_errno;
  }

  return result;
}

/* Perform the specified ACTION on the file descriptor FD, possibly
   using the argument ARG further described below.  This replacement
   handles the following actions, and forwards all others on to the
   native fcntl.  An unrecognized ACTION returns -1 with errno set to
   EINVAL.

   F_DUPFD - duplicate FD, with int ARG being the minimum target fd.
   If successful, return the duplicate, which will be inheritable;
   otherwise return -1 and set errno.

   F_DUPFD_CLOEXEC - duplicate FD, with int ARG being the minimum
   target fd.  If successful, return the duplicate, which will not be
   inheritable; otherwise return -1 and set errno.

   F_GETFD - ARG need not be present.  If successful, return a
   non-negative value containing the descriptor flags of FD (only
   FD_CLOEXEC is portable, but other flags may be present); otherwise
   return -1 and set errno.  */

int fcntl( int fd, int action, /* arg */... )
{
  va_list arg;
  int result = -1;
  va_start (arg, action);
  switch (action)
    {
    case F_DUPFD:
      {
        int target = va_arg (arg, int);
        result = dupfd(fd, target, 0);
        break;
      }

    case F_DUPFD_CLOEXEC:
      {
        int target = va_arg (arg, int);
        result = dupfd(fd, target, O_CLOEXEC);
        break;
      }

    case F_GETFD:
      {
        HANDLE handle = (HANDLE) _get_osfhandle (fd);
        DWORD flags;
        if (handle == INVALID_HANDLE_VALUE
            || GetHandleInformation (handle, &flags) == 0)
          errno = EBADF;
        else
          result = (flags & HANDLE_FLAG_INHERIT) ? 0 : FD_CLOEXEC;
        break;
      } /* F_GETFD */

    case F_GETFL:
      {
        result = 0;
        break;
      }

    case F_SETFD:
      {
        int flag = va_arg (arg, int);
        if (flag & FD_CLOEXEC) {
            HANDLE h = (HANDLE)_get_osfhandle(fd);
            if (h != NULL)
                result = SetHandleInformation(h, HANDLE_FLAG_INHERIT, 0) == 0 ? -1 : 0;
        }
        break;
      }

    case F_SETFL:
      {
        int flag = va_arg (arg, int);
        if (flag & O_NONBLOCK) {
            long mode = 1;
            result = ioctlsocket(fd, FIONBIO, &mode);
        }
        break;
      }

    default:
      {
        errno = EINVAL;
        break;
      }
    }
  va_end (arg);
  return result;
}

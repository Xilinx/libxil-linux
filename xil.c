/*
 * Copyright (C) Edgar E. Iglesias
 * Minimalistic libxil replacement for testing GCC. This one is inteded
 * to run on linux systems.
 *
 * GPLv2
 */
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "xil-init.h"

typedef int pid_t;

extern int _open(const char *pathname, int flags, mode_t mode);
extern int _close(int fd);
extern int _fstat(int fd, struct stat *buf);
extern ssize_t _write(int fd, const void *buf, size_t count);
extern ssize_t _read(int fd, void *buf, size_t count);
extern off_t _lseek(int fd, off_t offset, int whence);
extern int _unlink(const char *name);
extern pid_t _getpid(void);
extern int _kill(pid_t pid, int sig);

int _set_errno(int n)
{
        if (n < 0) {
                errno = -n;
                return -1;
        }
        return n;
}

#define LINUX_O_ACCMODE          0003
#define LINUX_O_RDONLY             00
#define LINUX_O_WRONLY             01
#define LINUX_O_RDWR               02
#define LINUX_O_CREAT            0100 /* not fcntl */
#define LINUX_O_EXCL             0200 /* not fcntl */
#define LINUX_O_NOCTTY           0400 /* not fcntl */
#define LINUX_O_TRUNC           01000 /* not fcntl */
#define LINUX_O_APPEND          02000
#define LINUX_O_NONBLOCK        04000
#define LINUX_O_NDELAY        LINUX_O_NONBLOCK
#define LINUX_O_SYNC           010000
#define LINUX_FASYNC           020000 /* fcntl, for BSD compatibility */
#define LINUX_O_DIRECTORY      040000 /* must be a directory */
#define LINUX_O_NOFOLLOW      0100000 /* don't follow links */
#define LINUX_O_LARGEFILE     0200000
#define LINUX_O_DIRECT        0400000 /* direct disk access hint */

struct {
	int flag;
	int linux_flag;
} fcntl_baremetal2linux[] = {
	{ O_WRONLY, LINUX_O_WRONLY },
	{ O_RDWR, LINUX_O_RDWR },
	{ O_CREAT, LINUX_O_CREAT },
	{ O_EXCL, LINUX_O_EXCL },
	{ O_NOCTTY, LINUX_O_NOCTTY },
	{ O_TRUNC, LINUX_O_TRUNC },
	{ O_APPEND, LINUX_O_APPEND },
	{ O_NONBLOCK, LINUX_O_NONBLOCK },
	{ O_SYNC, LINUX_O_SYNC },
	{ FASYNC, LINUX_FASYNC },
#ifdef O_DIRECTORY
	{ O_DIRECTORY, LINUX_O_DIRECTORY },
#endif
#ifdef O_NOFOLLOW
	{ O_NOFOLLOW, LINUX_O_NOFOLLOW },
#endif
#ifdef O_LARGEFILE
	{ O_LARGEFILE, LINUX_O_LARGEFILE },
#endif
#ifdef O_DIRECT
	{ O_DIRECT, LINUX_O_DIRECT },
#endif
	{ 0, 0 }
};


int open _PARAMS ((const char *pathname, int flags, ...))
{
	mode_t mode = 0;
	int i, linux_flags = 0;

	/* FIXME: This is a major hack.
	   Newlib -elf target has differnt fcntl masks than linux.
	   So we need to map them.

	   O_CREAT is one of them, possibly more. */
	i = 0;
	while (fcntl_baremetal2linux[i].flag) {
		/* If set, translate to the corresponding linux flag.  */
		if (flags & fcntl_baremetal2linux[i].flag) {
			linux_flags |= fcntl_baremetal2linux[i].linux_flag;
		}
		i++;
	}

	/* For O_CREAT we need to extract a mode.  */
	if (linux_flags & LINUX_O_CREAT) {
		va_list ap;

		va_start(ap, flags);
		mode = va_arg(ap, mode_t);
		va_end(ap);
	}

	return _set_errno(_open(pathname, linux_flags, mode));
}

int fstat(int fd, struct stat *buf)
{
	return _set_errno(_fstat(fd, buf));
}

ssize_t write(int fd, const void *buf, size_t count)
{
	return _set_errno(_write(fd, buf, count));
}

ssize_t read(int fd, void *buf, size_t count)
{
	return _set_errno(_read(fd, buf, count));
}

extern int close(int fd)
{
	return _set_errno(_close(fd));
}

off_t lseek(int fd, off_t offset, int whence)
{
	return _set_errno(_lseek(fd, offset, whence));
}

pid_t getpid(void)
{
	return _set_errno(_getpid());
}

int kill(pid_t pid, int sig)
{
	return _set_errno(_kill(pid, sig));
}

int unlink(const char *name)
{
	return _set_errno(_unlink(name));
}

__attribute__ ((constructor))
void __init_xil(void) {
	__init_xil_sbrk();
	__init_xil_signals();
}

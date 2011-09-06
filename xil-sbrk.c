/*
 * Copyright (C) Edgar E. Iglesias
 * Minimalistic libxil replacement for testing GCC. This one is inteded
 * to run on linux systems.
 *
 * GPLv2
 */
#include <inttypes.h>
#include <errno.h>
#include <unistd.h>

extern char _end[];
static void *curbrk;

extern void *_brk(void *addr);

int brk(void *addr)
{
        void *newbrk;

        if (curbrk == addr) {
                return 0;
	}

        newbrk = _brk(addr);
        curbrk = newbrk;
        if (newbrk < addr) {
                errno = ENOMEM;
                return -1;
        }
        return 0;
}

void *_sbrk(intptr_t increment)
{
        unsigned char *prevbrk = curbrk;
	int err;

        err = brk(prevbrk + increment);
        if (err == -1) {
		return (void *) - 1;
	}

	return prevbrk;
}

void *sbrk(intptr_t increment)
{
	return _sbrk(increment);
}

void __init_xil_sbrk(void)
{
	curbrk = _brk(0);
}

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
static void *curbrk = _end;

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
        void *prevbrk = curbrk;

        if (brk(prevbrk + increment) == -1) {
                return  (void *) -1;
	}

        return prevbrk;
}

void *sbrk(intptr_t increment)
{
	return _sbrk(increment);
}

/*
 * This is a horrific hack to deliver signals into newlib without changing
 * Xilinx newlib setup.
 *
 * Written by Edgar E. Iglesias
 *
 * GPLv2
 */

#include <stdio.h>
#include <signal.h>

struct linux_sigaction {
        sighandler_t sa_handler;
        unsigned long sa_flags;
        unsigned long sa_restorer;
        sigset_t sa_mask;
};

int _rt_sigaction(int signum, const struct linux_sigaction *act,
                     struct linux_sigaction *oldact);


extern void __sigtramp(int signum);
static void sighandler(int signum)
{
	__sigtramp(signum);
}

void __init_xil_signals(void)
{
	struct linux_sigaction act = {0};

	act.sa_handler = sighandler;
	_rt_sigaction(SIGFPE, &act, NULL);
}

/*
 * arch/i386/include/klibc/archsignal.h
 *
 * Architecture-specific signal definitions
 *
 */

#ifndef _KLIBC_ARCHSIGNAL_H
#define _KLIBC_ARCHSIGNAL_H

/* The in-kernel headers for i386 still have libc5
   crap in them.  Reconsider using <asm/signal.h>
   when/if it gets cleaned up; for now, duplicate
   the definitions here. */

#define _NSIG           64
#define _NSIG_BPW       32
#define _NSIG_WORDS     (_NSIG / _NSIG_BPW)

typedef struct {
	unsigned long sig[_NSIG_WORDS];
} sigset_t;

#define SIGHUP           1
#define SIGINT           2
#define SIGQUIT          3
#define SIGILL           4
#define SIGTRAP          5
#define SIGABRT          6
#define SIGIOT           6
#define SIGBUS           7
#define SIGFPE           8
#define SIGKILL          9
#define SIGUSR1         10
#define SIGSEGV         11
#define SIGUSR2         12
#define SIGPIPE         13
#define SIGALRM         14
#define SIGTERM         15
#define SIGSTKFLT       16
#define SIGCHLD         17
#define SIGCONT         18
#define SIGSTOP         19
#define SIGTSTP         20
#define SIGTTIN         21
#define SIGTTOU         22
#define SIGURG          23
#define SIGXCPU         24
#define SIGXFSZ         25
#define SIGVTALRM       26
#define SIGPROF         27
#define SIGWINCH        28
#define SIGIO           29
#define SIGPOLL         SIGIO
#define SIGPWR          30
#define SIGSYS          31
#define SIGUNUSED       31

#define SIGRTMIN        32
#define SIGRTMAX        (_NSIG-1)

/*
 * SA_FLAGS values:
 *
 * SA_ONSTACK indicates that a registered stack_t will be used.
 * SA_INTERRUPT is a no-op, but left due to historical reasons. Use the
 * SA_RESTART flag to get restarting signals (which were the default long ago)
 * SA_NOCLDSTOP flag to turn off SIGCHLD when children stop.
 * SA_RESETHAND clears the handler when the signal is delivered.
 * SA_NOCLDWAIT flag on SIGCHLD to inhibit zombies.
 * SA_NODEFER prevents the current signal from being masked in the handler.
 *
 * SA_ONESHOT and SA_NOMASK are the historical Linux names for the Single
 * Unix names RESETHAND and NODEFER respectively.
 */
#define SA_NOCLDSTOP	0x00000001u
#define SA_NOCLDWAIT	0x00000002u
#define SA_SIGINFO	0x00000004u
#define SA_ONSTACK	0x08000000u
#define SA_RESTART	0x10000000u
#define SA_NODEFER	0x40000000u
#define SA_RESETHAND	0x80000000u

#define SA_NOMASK	SA_NODEFER
#define SA_ONESHOT	SA_RESETHAND
#define SA_INTERRUPT	0x20000000	/* dummy -- ignored */

#define SA_RESTORER	0x04000000

/*
 * sigaltstack controls
 */
#define SS_ONSTACK	1
#define SS_DISABLE	2

#define MINSIGSTKSZ	2048
#define SIGSTKSZ	8192
#if 0
#include <asm-generic/signal.h>
#endif

#ifndef SIG_BLOCK
#define SIG_BLOCK          0    /* for blocking signals */
#endif
#ifndef SIG_UNBLOCK
#define SIG_UNBLOCK        1    /* for unblocking signals */
#endif
#ifndef SIG_SETMASK
#define SIG_SETMASK        2    /* for setting the signal mask */
#endif

#ifndef __ASSEMBLY__
typedef void __signalfn_t(int);
typedef __signalfn_t __user *__sighandler_t;

typedef void __restorefn_t(void);
typedef __restorefn_t __user *__sigrestore_t;

#define SIG_DFL ((__force __sighandler_t)0)     /* default signal handling */
#define SIG_IGN ((__force __sighandler_t)1)     /* ignore signal */
#define SIG_ERR ((__force __sighandler_t)-1)    /* error return from signal */
#endif

/* This uses gcc anonymous union support... */
struct siginfo;

struct sigaction {
	union {
		__sighandler_t sa_handler;
		void (*sa_sigaction)(int, struct siginfo *, void *);
	};
	unsigned long	sa_flags;
	__sigrestore_t 	sa_restorer;
	sigset_t	sa_mask;
};

#endif

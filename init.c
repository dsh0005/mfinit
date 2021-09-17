#define _XOPEN_SOURCE 700
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#define RC_PATH "/etc/"
#define RC_FILENAME "rc"

#if defined(REALLY_MINIMAL) && REALLY_MINIMAL != 0
#include <sys/syscall.h>
#endif

#if defined(REALLY_MINIMAL) && REALLY_MINIMAL != 0

#ifndef __x86_64__
#error "can't do really minimal on anything but x64"
#endif

#define sfi __attribute__((always_inline)) static inline

sfi pid_t sc_getpid(void){
	pid_t ret;
	__asm__("syscall"
		: "=a" (ret)
		: "a" (SYS_getpid)
		: "rcx", "r11");
	return ret;
}

sfi void sc_exit(int code){
	__asm__ __volatile__("syscall"
		:
		: "a" (SYS_exit), "D" (code)
		: "rcx", "r11");
}

sfi int sc_rt_sigprocmask(int how, sigset_t *nset, sigset_t *oset, size_t sigsetsize){
	int ret;
	register long r10 __asm__("r10") = sigsetsize;
	__asm__("syscall"
		: "=a" (ret)
		: "a" (SYS_rt_sigprocmask), "D" (how), "S" (nset), "d" (oset), "r" (r10)
		: "memory", "rcx", "r11");
	return ret;
}

sfi pid_t sc_fork(void){
	pid_t ret;
	__asm__ __volatile__("syscall"
		: "=a" (ret)
		: "a" (SYS_fork)
		: "memory", "rcx", "r11");
	return ret;
}

sfi int sc_waitid(int which, pid_t upid, siginfo_t * infop, int options){
	int ret;
	register struct rusage *ru __asm__("r8") = NULL;
	register int r10 __asm__("r10") = options;
	__asm__ __volatile__("syscall"
		: "=a" (ret)
		: "a" (SYS_waitid), "D" (which), "S" (upid), "d" (infop), "r" (r10), "r" (ru)
		: "memory", "rcx", "r11");
	return ret;
}

sfi pid_t sc_setsid(void){
	pid_t ret;
	__asm__ __volatile__("syscall"
		: "=a" (ret)
		: "a" (SYS_setsid)
		: "memory", "rcx", "r11");
	return ret;
}

sfi int sc_setpgid(pid_t pid, pid_t pgid){
	int ret;
	__asm__ __volatile__("syscall"
		: "=a" (ret)
		: "a" (SYS_setpgid), "D" (pid), "S" (pgid)
		: "memory", "rcx", "r11");
	return ret;
}

sfi int sc_execve(const char *filename, char * const * argv, char * const * envp){
	int ret;
	__asm__ __volatile__("syscall"
		: "=a" (ret)
		: "a" (SYS_execve), "D" (filename), "S" (argv), "d" (envp)
		: "memory", "rcx", "r11");
	return ret;
}

void _start(void)
{
	sigset_t set;
	int status;
	siginfo_t dummy;
	size_t i;
	char * const set_c = (char*)&set;
	char * const rc_args[] = { RC_FILENAME, 0 };
	char * const rc_env[] = { 0 };

	if (sc_getpid() != 1) (void)sc_exit(1);
	for (i = 0; i < sizeof(sigset_t); i++)
		set_c[i] = '\xff';
#	ifdef SIGCANCEL
		set.__val[((SIGCANCEL)-1)/(8*sizeof(unsigned long))] &= ~(1UL << (((SIGCANCEL-1))%(8*sizeof(unsigned long))));
#	endif
#	ifdef SIGSETXID
		set.__val[((SIGSETXID)-1)/(8*sizeof(unsigned long))] &= ~(1UL << (((SIGSETXID-1))%(8*sizeof(unsigned long))));
#	endif
	if (sc_rt_sigprocmask(SIG_BLOCK, &set, 0, sizeof(sigset_t))) (void)sc_exit(3);

	status = sc_fork();
	if (status == -1) (void)sc_exit(4);
	else if (status) for (;;) (void)sc_waitid(P_ALL, 0, &dummy, WEXITED);

	if (sc_rt_sigprocmask(SIG_UNBLOCK, &set, 0, sizeof(sigset_t))) (void)sc_exit(5);

	if (sc_setsid() == (pid_t)-1) (void)sc_exit(6);
	if (sc_setpgid(0, 0)) (void)sc_exit(7);
	(void)sc_execve(RC_PATH RC_FILENAME, rc_args, rc_env);
	(void)sc_exit(8);
}
#else
int main(void)
{
	sigset_t set;
	int status;
	char * const rc_args[] = { RC_FILENAME, 0 };
	char * const rc_env[] = { 0 };

	if (getpid() != 1) return 1;

	if (sigfillset(&set)) return 2;
	if (sigprocmask(SIG_BLOCK, &set, 0)) return 3;

	status = fork();
	if (status == -1) return 4;
	else if (status) for (;;) (void)wait(&status);

	if (sigprocmask(SIG_UNBLOCK, &set, 0)) return 5;

	if (setsid() == (pid_t)-1) return 6;
	if (setpgid(0, 0)) return 7;
	(void)execve(RC_PATH RC_FILENAME, rc_args, rc_env);
	return 8;
}
#endif

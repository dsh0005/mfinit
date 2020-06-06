#define _XOPEN_SOURCE 700
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#define RC_PATH "/etc/"
#define RC_FILENAME "rc"

#if defined(REALLY_MINIMAL) && REALLY_MINIMAL != 0
#define _GNU_SOURCE
#include <sys/syscall.h>
extern int syscall(int, ...);
#endif

#if defined(REALLY_MINIMAL) && REALLY_MINIMAL != 0
void _start(void)
{
	sigset_t set;
	int status;
	siginfo_t dummy;
	size_t i;
	char * const set_c = (char*)&set;

	if (syscall(SYS_getpid) != 1) (void)syscall(SYS_exit, 1);
	for (i = 0; i < sizeof(sigset_t); i++)
		set_c[i] = ~0;
#	ifdef SIGCANCEL
		set->__val[((SIGCANCEL)-1)/(8*sizeof(unsigned long))] &= ~(1UL << (((SIGCANCEL-1))%(8*sizeof(unsigned long))));
#	endif
#	ifdef SIGSETXID
		set->__val[((SIGSETXID)-1)/(8*sizeof(unsigned long))] &= ~(1UL << (((SIGSETXID-1))%(8*sizeof(unsigned long))));
#	endif
	if (syscall(SYS_rt_sigprocmask, SIG_BLOCK, &set, 0, sizeof(sigset_t))) (void)syscall(SYS_exit, 3);

	status = syscall(SYS_fork);
	if (status == -1) (void)syscall(SYS_exit, 4);
	else if (status) for (;;) (void)syscall(SYS_waitid, P_ALL, 0, &dummy, WEXITED);

	if (syscall(SYS_rt_sigprocmask, SIG_UNBLOCK, &set, 0, sizeof(sigset_t))) (void)syscall(SYS_exit, 5);

	if (syscall(SYS_setsid) == (pid_t)-1) (void)syscall(SYS_exit, 6);
	if (syscall(SYS_setpgid, 0, 0)) (void)syscall(SYS_exit, 7);
	(void)syscall(SYS_execve, RC_PATH RC_FILENAME, (char * const []){ RC_FILENAME, 0 }, (char * const[]){ 0 });
	(void)syscall(SYS_exit, 8);
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

#define _XOPEN_SOURCE 700
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#define RC_PATH "/etc/"
#define RC_FILENAME "rc"

#if defined(REALLY_MINIMAL) && REALLY_MINIMAL != 0
#define _GNU_SOURCE
#include <sys/syscall.h>
#endif

#if defined(REALLY_MINIMAL) && REALLY_MINIMAL != 0
void
#else
int
#endif
impl(void);

void sc_impl(void);

#if defined(REALLY_MINIMAL) && REALLY_MINIMAL != 0
void _start(void)
{
	sc_impl();
}
#else
int main(void)
{
	return impl();
}
#endif

/* Helper macros for our really minimal config. */
#if defined(REALLY_MINIMAL) && REALLY_MINIMAL != 0
# define EXIT_VALUE(x) _exit(x)
#else
# define EXIT_VALUE(x) return x
#endif

void sc_impl(void)
{
	sigset_t set;
	int status;

	if (syscall(SYS_getpid) != 1) EXIT_VALUE(1);
}

#if defined(REALLY_MINIMAL) && REALLY_MINIMAL != 0
void
#else
int
#endif
impl(void)
{
	sigset_t set;
	int status;

	if (getpid() != 1) EXIT_VALUE(1);

	if (sigfillset(&set)) EXIT_VALUE(2);
	if (sigprocmask(SIG_BLOCK, &set, 0)) EXIT_VALUE(3);

	status = fork();
	if (status == -1) EXIT_VALUE(4);
	else if (status) for (;;) wait(&status);

	if (sigprocmask(SIG_UNBLOCK, &set, 0)) EXIT_VALUE(5);

	if (setsid() == (pid_t)-1) EXIT_VALUE(6);
	if (setpgid(0, 0)) EXIT_VALUE(7);
	(void)execve(RC_PATH RC_FILENAME, (char * const []){ RC_FILENAME, 0 }, (char * const[]){ 0 });
	EXIT_VALUE(8);
}

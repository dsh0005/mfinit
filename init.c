#define _XOPEN_SOURCE 700
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

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
	return execve(RC_PATH RC_FILENAME, rc_args, rc_env);
}

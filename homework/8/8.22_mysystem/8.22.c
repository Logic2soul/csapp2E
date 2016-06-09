#include "csapp.h"
#define MAXARGS 128

pid_t efork() {
	pid_t pid;
	pid = fork();
	if (pid < 0) {
		fprintf(stderr, "pid error: %s\n", strerror(errno));
		exit(-1);
	}
	return pid;
}

int mysystem(char *command) {
	pid_t pid, r_pid;
	int status;
	char *argv[MAXARGS];

	argv[0] = "/bin/sh";
	argv[1] = "-c";
	argv[2] = command;

	if (command == NULL)
		return 1;
	if ((pid = efork()) == 0) {
		while (execve("/bin/sh", argv, environ) < 0) {
			if (errno != EINTR) {
				fprintf(stderr, "execve sh error.\n");
				exit(127);
			}
		}
	}
	while ((r_pid = waitpid(-1, &status, 0)) < 0) {
		if (errno != EINTR) {
			fprintf(stderr, "waitpid error: %s\n", strerror(errno));
			exit(-1);
		}
	}
	if (WIFEXITED(status)) {
		printf("child %d terminated normally with exit status=%d\n",
			r_pid, WEXITSTATUS(status));
		return WEXITSTATUS(status);
	}
	else 
		return -1;
}

int main(int argc, char **argv) {
	int ret;
	char buf[MAXLINE];
	void (*old_handler)(int);

	strcpy(buf, argv[1]);
	old_handler = signal(SIGCHLD, SIG_DFL);
	ret = mysystem(buf);
	signal(SIGCHLD, old_handler);
	return ret;
}

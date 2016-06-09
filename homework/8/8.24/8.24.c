#include "csapp.h"
#define N 2

pid_t efork() {
	pid_t pid = fork();
	if (pid < 0) {
		fprintf(stderr, "fork error: %s\n", strerror(errno));
		exit(-1);
	}
	return pid;
}

int main() {
	int status, i, fd, n;
	void *p = (void *)0x400000;
	pid_t pid;

	for (i = 0; i < N; i++) {
		if ((pid = efork()) == 0) {
			*(int *)p = 1;
		}
	}
	while ((pid = waitpid(-1, &status, 0)) > 0) {
		if (WIFEXITED(status))
			printf("child %d terminated normally with exit status=%d\n", 
				pid, WEXITSTATUS(status));
		else {
			fprintf(stderr, "child %d terminated by ", pid);
			psignal(11, "signal 11");
		}
	}
	if (errno != ECHILD) {
		fprintf(stderr, "waitpid error: %s\n", strerror(errno));
		exit(-1);
	}
	exit(0);
}

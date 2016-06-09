#include "csapp.h"

pid_t efork() {
	pid_t pid;
	pid = fork();
	if (pid < 0) {
		fprintf(stderr, "fork error: %s\n", strerror(errno));
		exit(0);
	}
	return pid;
}

void end(void) {
	printf("2");
	fflush(stdout);
}

int main() {
	if (efork() == 0)
		atexit(end);
	if (efork() == 0) {
		printf("0");
		fflush(stdout);
	}
	else
		printf("1");
		fflush(stdout);
	exit(0);
}


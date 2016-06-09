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

int main() {
	int x = 3;

	if (efork() != 0)
		printf("x=%d\n", ++x);

	printf("x=%d\n", --x);
	exit(0);
}

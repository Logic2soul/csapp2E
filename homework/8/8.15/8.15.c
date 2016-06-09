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

void doit() {
	if (efork() == 0) {
		efork();
		printf("hello\n");
		return;
	}
	return;
}

int main() {
	doit();
	printf("hello\n");
	exit(0);
}

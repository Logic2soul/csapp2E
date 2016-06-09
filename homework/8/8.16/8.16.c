#include "csapp.h"

int counter = 1;

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
	if (efork() == 0) {
		counter--;
		exit(0);
	}
	else {
		if (wait(NULL) < 0) {
			if (errno != ECHILD) {
				fprintf(stderr, "wait error: %s\n", strerror(errno));
				exit(0);
			}
		}
		printf("counter = %d\n", ++counter);
	}
	exit(0);
}

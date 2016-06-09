#include "csapp.h"

void doit() {
	if (fork() < 0) {
		fprintf(stderr, "fork error: %s\n", strerror(errno));
		exit(0);
	}
	if (fork() < 0) {
		fprintf(stderr, "fork error: %s\n", strerror(errno));
		exit(0);
	}
	printf("hello\n");
	return;
}

int main() {
	doit();
	printf("hello\n");
	exit(0);
}


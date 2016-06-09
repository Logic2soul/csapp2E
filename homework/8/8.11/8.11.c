#include "csapp.h"

int main() {
	int i;

	for (i = 0; i < 2; i++) {
		if (fork() < 0) {
			fprintf(stderr, "fork error:%s\n", strerror(errno));
			exit(0);
		}
	}
	printf("hello\n");
	exit(0);
}




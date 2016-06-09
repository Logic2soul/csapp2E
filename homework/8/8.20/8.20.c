#include "csapp.h"

int main(int argc, char **argv) {
	if (execve("/bin/ls", argv, environ) < 0) {
		fprintf(stderr, "command not found.\n");
		exit(0);
	}
}


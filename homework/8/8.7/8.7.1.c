#include <csapp.h>

int main(int argc, char **argv) {
	unsigned sec;
	
	if (argc != 2) {
		fprintf(stderr, "usage: %s <unsigned>\n", argv[0]);
		exit(0);
	}

	sscanf(argv[1], "%u", &sec);

	unsigned i = Sleep(sec);
        printf("Slept for %d of %d secs.\n", sec - i, sec);

	exit(0);
}

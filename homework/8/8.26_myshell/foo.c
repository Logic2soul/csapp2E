#include "csapp.h"

void alarm_handler(int sig) {
	exit(0);
}

int main(int argc, char **argv) {
	
	signal(SIGALRM, alarm_handler);
	alarm(atoi(argv[1]));
	while (1) 
		;
	exit(0);
}


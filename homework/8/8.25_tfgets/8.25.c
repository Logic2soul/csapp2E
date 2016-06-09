#include "csapp.h"
sigjmp_buf env;

void handler(int sig) {
	siglongjmp(env, 1);
}

char *tfgets(char *buf, int bufsize, FILE *f) {
	int nbyte, fd;

	while (signal(SIGALRM, handler) == SIG_ERR)
		;
	fd = fileno(f);
	alarm(5);
	if (sigsetjmp(env, 1))
		return NULL;
	while ((nbyte = read(fd, buf, bufsize - 1)) < 0)
		;
	alarm(0);
	buf[nbyte] = 0;
	return buf;
}

int main() {
	char buf[5];
	char *bufp = buf;
	int i;

	bufp = tfgets(buf, 5, stdin);
	if (bufp == NULL)
		printf("across 5 seconds, input: NULL.\n");
	else {
		for (i = 0; i < 5; i++) 
			printf("%d ", bufp[i]);
		printf("\n");
	}
	return 0;
}

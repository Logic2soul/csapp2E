#include <csapp.h>
#include <termios.h>

sigjmp_buf buf;

int mygetch() {
	struct termios oldt,
	newt;
	int ch;
	tcgetattr( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &newt );
	ch = getchar();
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
	return ch;
}

void handler1(int sig) {
	int status;
	pid_t pid;
	if ((pid = waitpid(-1, &status, 0)) > 0)
		fprintf(stderr,"child %d reaped.\n", (int)pid);
	else if (errno != ECHILD) {
		fprintf(stderr, "waitpid error: %s\n", strerror(errno));
		exit(0);
	}
	return;
}
	
int main(int argc, char **argv) {	
	int i = 0;
	int j;
	int fd[2];
	pid_t pid;
	
	signal(SIGPIPE, SIG_IGN);
	if (signal(SIGCHLD, handler1) == SIG_ERR) {
		fprintf(stderr, "signal error.\n");
		exit(0);
	}

	if ((j = pipe(fd)) < 0) {
		fprintf(stderr, "create pipe error: %s\n", strerror(errno));
		exit(0);
	}

	if ((pid = fork()) < 0) {
		fprintf(stderr, "fork error: %s\n", strerror(errno));
		exit(0);
	}
	if (pid == 0) {
		close(fd[1]);
		dup2(fd[0], 0);
		close(fd[0]);
		int m;
		char d;
		while (1) {
			if ((m = read(0, &d, 1)) == 1) {
				if (d == 'q')
					exit(0);
				printf("key %c pressed.\n", d);
			}
		}
	}
	close(fd[0]);
	char w = 'a';
	char y = w - 32;
	while (1) {
		write(1, &y, 1);
		write(fd[1], &w, 1);
		w++;
		if (w > 122)
			w = 'a';
		y = w - 32;
		sleep(3);
	}
	return 0;
}


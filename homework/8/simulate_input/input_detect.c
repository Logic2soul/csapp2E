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
	siglongjmp(buf, 1);
}
	
int main(int argc, char **argv) {	
	int i = 0;
	pid_t pid;

	if (signal(SIGINT, handler1) == SIG_ERR) {
		fprintf(stderr, "signal error.\n");
		exit(0);
	}

	if (sigsetjmp(buf, 1)) {
		int key;
		key = mygetch();
		if (key == 'q')
			exit(0);
		printf("key %c pressed.\n", key);
		sleep(3);
	}

	char w = 'f';

	write(0, &w, 1);
	sleep(3);

	return 0;
}


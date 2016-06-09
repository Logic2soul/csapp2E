#include <csapp.h>
#include <linux/input.h>
#include <termios.h>

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
	pid_t pid;
	int status;

	if ((pid = waitpid(-1, &status, 0)) < 0) {
		fprintf(stderr, "waitpid error: %s\n", strerror(errno));
		exit(0);
	}
	printf("reaped child %d\n", (int)pid);
	return;
}

void simulate_key(int fd, int kval) {
	struct input_event event;
	gettimeofday(&event.time, 0);
	event.type = EV_KEY;
	event.value = 1;
	event.code = kval;
	write(fd, &event, sizeof(event));
	gettimeofday(&event.time, 0);
	event.type = EV_KEY;
	event.value = 0;
	event.code = kval;
	write(fd, &event, sizeof(event));
	event.type = EV_SYN;
	event.value = 0;
	event.code = SYN_REPORT;
	write(fd, &event, sizeof(event));
}

void simulate_mouse(int fd, int rel_x, int rel_y) {
	struct input_event t;
	gettimeofday(&t.time, 0);
	t.type = EV_REL;
	t.value = rel_x;
	t.code = REL_X;
	write(fd, &t, sizeof(t));
	t.type = EV_REL;
	t.value = rel_y;
	t.code = REL_Y;
	write(fd, &t, sizeof(t));
	t.type = EV_SYN;
	t.value = 0;
	t.code = SYN_REPORT;
	write(fd, &t, sizeof(t));
}

int main(int argc, char **argv) {
	int fd_mouse = -1;
	int fd_kbd = -1;
	int i = 0;
	pid_t pid;

	if (signal(SIGCHLD, handler1) == SIG_ERR) {
		fprintf(stderr, "signal error.\n");
		exit(0);
	}

	fd_kbd = open("/dev/tty", O_RDWR);
	if (fd_kbd <= 0) {
		fprintf(stderr, "can not open keyboard input file.\n");
		return -1;
	}
	fd_mouse = open("/dev/input/event4", O_RDWR);
	if (fd_mouse <= 0) {
		fprintf(stderr, "can not open mouse input file.\n");
		return -1;
	}
	
	if ((pid = fork()) == 0) {
		int key;
		while (key = mygetch()) {
			if (key == 'q')
				exit(0);
			printf("key %c pressed.\n", key);
		}
	}

	char w = 'f';

	for (i = 0; i < 10; i++) {
		write(fd_kbd, &w, 1);
		sleep(3);
	}

	w = 'q';
	for (i = 0; i < 5; i++) {
		write(fd_kbd, &w, 1);
		sleep(3);
	}
	close(fd_kbd);
	close(fd_mouse);
	return 0;
}


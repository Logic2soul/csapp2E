#include <csapp.h>
#include <linux/input.h>

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

	fd_kbd = open("/dev/input/event3", O_RDWR);
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
		struct input_event listen;
		int n;
		while (1) {
			if ((n = read(fd_kbd, &listen, sizeof(listen))) < 0) {
				fprintf(stderr, "read error: %s\n", strerror(errno));
				exit(0);
			}
			if (n != 0) {
				if (listen.code == KEY_ESC)
					exit(0);
				if (listen.type == EV_KEY)	
					printf("key %d %s\n", listen.code, listen.value ? "pressed" : "released");
			}
		}
	}

	for (i = 0; i < 10; i++) {
		simulate_key(fd_kbd, KEY_A);
		simulate_mouse(fd_mouse, 0.1, 0.1);
		sleep(3);
	}
	for (i = 0; i < 5; i++) {
		simulate_key(fd_kbd, KEY_ESC);
		sleep(3);
	}
	close(fd_kbd);
	close(fd_mouse);
	return 0;
}


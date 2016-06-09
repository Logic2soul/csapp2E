#include <csapp.h>
#include <termios.h>

char w = 'a';

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

void handler2(int sig) {

        printf("%c\n", w); 
        w++;
        if (w > 122)
        	w = 'a';
	alarm(3);
	return;
}   
	
int main(int argc, char **argv) {	
	
	int key;

	if (signal(SIGALRM, handler2) == SIG_ERR) {
		fprintf(stderr, "signal error.\n");
		exit(0);
	}
	alarm(3);
	while (1) {
		key = mygetch();
		if (key == 'q')
			exit(0);
		printf("key %c pressed.\n", key);
	}
	return 0;
}


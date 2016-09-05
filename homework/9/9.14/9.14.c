#include <csapp.h>

int main(void) {
	int fd;
	char *bufp;
	char buf[] = "Hello, world!\n";

	if ((fd = open("/home/Logic2soul/hello.txt", O_RDWR|O_CREAT, 0)) == -1) {
		fprintf(stderr, "open error: %s\n", strerror(errno));
		exit(-1);
	}
	if (write(fd, buf, sizeof(buf)) == -1) {
		fprintf(stderr, "write error: %s\n", strerror(errno));
		exit(-1);
	}
	if ((bufp = mmap(NULL, sizeof(buf), PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0)) == (char *)-1) {
		fprintf(stderr, "mmap error: %s\n", strerror(errno));
		exit(-1);
	}
	*bufp = 'J';
	printf("%s\n", bufp);
	return 0;
}

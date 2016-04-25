#include <stdio.h>
#include <stdlib.h>

#define w 32

unsigned rotate_right(unsigned x, int n)
{
	return (x << ((w -n) & ((1 << 5) - 1))) | (x >> n);
}

int main(int argc, char** argv)
{
	unsigned x;
	int n;
	if(argc != 3)
	{
		fprintf(stderr, "usage: %s <unsigned> <int>\n", argv[0]);
		exit(0);
	}
	sscanf(argv[1], "%x", &x);
	sscanf(argv[2], "%d", &n);
	printf("rotate_right: %x\n", rotate_right(x, n));
	exit(0);	
}

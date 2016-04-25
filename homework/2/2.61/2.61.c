#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	int x;
	if(argc != 2)
	{
		fprintf(stderr, "usage: %s <int>\n", argv[0]);
		exit(0);
	}
	sscanf(argv[1], "%x", &x);
	printf("A: %d\nB: %d\nC: %d\nD: %d\n", !(x ^ (~0)), !(x ^ 0), !((x & 0xFF000000) ^0xFF000000), !((x & 0xFF) ^ 0));
	exit(0);
}

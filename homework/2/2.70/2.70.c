#include <stdio.h>
#include <stdlib.h>

int fits_bits(int x, int n)
{
	int y = n & ((1 << 5) - 1);
	return!((((unsigned) x >> y) ^ 0) && y);
}

int main(int argc, char** argv)
{
	int x, n;
	if(argc != 3)
	{
		fprintf(stderr, "usage: %s <int> <int>\n", argv[0]);
		exit(0);
	}
	sscanf(argv[1], "%x", &x);
	sscanf(argv[2], "%d", &n);
	printf("fits_bits: %d\n", fits_bits(x, n));
	exit(0);
}

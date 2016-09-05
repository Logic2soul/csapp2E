#include <stdio.h>
#include <stdlib.h>

int any_even_one(unsigned x)
{
	return ((x & 0x55555555) && 1);
}

int main(int argc, char** argv)
{
	unsigned n;
	int is_even = 2;
	if(argc != 2)
	{
	fprintf(stderr, "usage: %s <unsigned>\n", argv[0]);
	exit(0);
	}
	sscanf(argv[1], "%x", &n);
	printf("%x\n", n);
	is_even = any_even_one(n);
	printf("is_even: %d\n", is_even);
	exit(0);
}

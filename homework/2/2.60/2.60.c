#include <stdio.h>
#include <stdlib.h>
#include <string.h>
unsigned replace_byte(unsigned x, unsigned char b, int i)
{
	char* bytep;
	bytep = (char*) &x;
	memcpy(bytep + i, &b, 1);
	return x;
}

int main(int argc, char** argv)
{
	unsigned n;
	unsigned char b;
	int i;
	if(argc != 4)
	{
		fprintf(stderr, "usage: %s <unsigned> <unsigned char> <int>\n", argv[0] );
		exit(0);
	}
	sscanf(argv[2], "%x", &b);
	sscanf(argv[1], "%x", &n);
	sscanf(argv[3], "%d", &i);
	n = replace_byte(n, b, i);
	printf("%x %x %d\n", n, b, i);
	exit(0);
}

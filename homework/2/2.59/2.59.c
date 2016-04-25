#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	int x, y, z;
	if(argc != 3)
		{
			fprintf(stderr, "usage: %s <int> <int>\n", argv[0]);
			exit(0);
		}
		sscanf(argv[1], "%x", &x);
	       sscanf(argv[2], "%x", &y);
		z = (y & (~ 0xFF)) | (x & 0xFF);
		printf("%x\n", z);
	return z;
}


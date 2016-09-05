#include <stdio.h>
#include <stdlib.h>

int divide_power2(int x, int k)
{
	return (k && ((1 << (k - 1)) & x)) + (x >> k);	
}

int main(int argc, char** argv)
{
	int x, k;
	if(argc != 3)
	{ 
		fprintf(stderr, "usage: %s <int> <int>\n", argv[0]);
		exit(0);
	}
	sscanf(argv[1], "%d", &x);
	sscanf(argv[2], "%d", &k);
	printf("divide_power2: %d\n", divide_power2(x, k));
	exit(0);
}

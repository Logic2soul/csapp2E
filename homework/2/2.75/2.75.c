#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int signed_high_prod(int x, int y)
{
	long long int lx = (long long int)x;
	long long int ly =(long long int) y;
	long long int mul = lx * ly;
	int shp = (int) (mul >> 32);
	return shp;
}

unsigned unsigned_high_prod(unsigned x, unsigned y)
{
	int shp = signed_high_prod((int) x, (int) y);
	return shp + ((x & INT_MIN) && 1) * y + ((y & INT_MIN) && 1) * x;
}

int main(int argc, char** argv)
{
	unsigned x, y;
	if(argc != 3)
	{
		fprintf(stderr, "usage: %s <unsigned> <unsigned>\n", argv[0]);
		exit(0);
	}
	sscanf(argv[1], "%x", &x);
	sscanf(argv[2], "%x", &y);
	printf("unsigned_high_prod: %x\n", unsigned_high_prod(x, y));
	exit(0);
}

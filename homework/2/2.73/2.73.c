#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int saturating_add(int x, int y)
{
	int pos_over = !(x & INT_MIN) && !(y & INT_MIN) && ((x + y) & INT_MIN);
	int neg_over = (x & INT_MIN) && (y & INT_MIN) && !((x + y) & INT_MIN);
	return ((pos_over || neg_over) << 31) - pos_over;
}

int main(int argc, char** argv)
{
	int x, y;
	if(argc != 3)
	{
		fprintf(stderr, "usage: %s <int> <int>\n", argv[0]);
		exit(0);
	}
	sscanf(argv[1], "%x", &x);
	sscanf(argv[2], "%x", &y);
	printf("saturating_add: %x\n", saturating_add(x, y));
	exit(0);
}

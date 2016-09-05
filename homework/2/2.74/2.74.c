#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int tsub_ovf(int x, int y)
{
	int neg_y = -y;
	int pos_over = !(x & INT_MIN) && !(neg_y & INT_MIN) && ((x + neg_y) & INT_MIN);
	int neg_over = (x & INT_MIN) && (neg_y & INT_MIN) && !((x + neg_y) & INT_MIN);
	return pos_over || neg_over;	
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
	printf("tsub_ovf: %d\n", tsub_ovf(x, y));
	exit(0);
}

#include <stdio.h>
#include <stdlib.h>

int lower_bits(int x, int n)
{
/*对n取32的余数y， 类似n % 32的处理*/
	int y = n & ((1 << 5) - 1);
	return x | ((1 << y) - 1 - !y);
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
	printf("lower_bits: %x\n", lower_bits(x, n));
	exit(0);
}

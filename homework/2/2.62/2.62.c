#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int int_shifts_are_logical()
{
	int x = -1;
	return !((x >> 1) & INT_MIN);
}

int main(void)
{
	printf("int_shifts_are_logical: %d\n", int_shifts_are_logical() );
	exit(0);
}

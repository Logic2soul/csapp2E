#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int testint = 1, i;
	char * p = (char* ) &testint;
	
	if(*p == 1)
		{
		printf("the machine is little endian\n");
		return 1;
		}
	else 
		{
		printf("the machine is big endian\n");
		return 0;
		}
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 10

int good_echo()
{
	int i;
	char buf[BUFSIZE];

	while(1)
	{
	fgets(buf, BUFSIZE, stdin);
	puts(buf);
	for(i = 0; i < 9; i++)
		{
		if(buf[i] == '1')
			return 1;
		else if(buf[i] == '\n')
			return 0;
		}
	}
}

int main(void)
{
	printf("%d\n", good_echo());
	return 0;
}

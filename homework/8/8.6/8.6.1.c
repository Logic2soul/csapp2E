#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv, char **envp)
{
	void **p;
	int i;

	printf("Command line arguments:\n");
	for(i = 0, p = argv; *p != NULL; i++, p++)
		printf("argv[%d]: %s\n", i, *p);
	printf("Environment variables:\n");
	for(i = 0, p = envp; *p != NULL; i++, p++)
		printf("envp[%d]: %s\n", i, *p);
	exit(0);
}


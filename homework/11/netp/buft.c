#include <csapp.h>
void main()
{
	char buf[MAXLINE] = "12";
	char *p;

	printf("old: %s\n", buf);
	for(p = buf; *p != '\0'; p++);
	fgets(p, MAXLINE, stdin);	
	printf("new: %s", buf);
	printf("bytes: %d\n", strlen(buf));
}

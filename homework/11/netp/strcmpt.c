#include <csapp.h>

void main()
{
	char s[] = "exit\n";
	char *p, *i;
	char buf[MAXLINE];
	p = fgets(buf, MAXLINE, stdin);
	for(i = p; i < (p + 6); i++)
	{
	printf("%d\n", *i);
	}
	for(i = s; i < (s + 6); i++)
        {
        printf("%d\n", *i);
        }
	printf("p: %ss: %s", p, s);
	if(strcmp(p, s) > 0)printf("1\n");
	else if(strcmp(p, s) == 0)printf("0\n");
		else printf("-1\n");
	exit(0);
}

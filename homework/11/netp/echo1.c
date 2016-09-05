/*
 * echo - read and echo text lines until client closes connection
 */
/* $begin echo */
#include "csapp.h"

void echo(int connfd) 
{
	size_t n; 
	int nc = 0;
	char buff[MAXLINE]; 
	rio_t rio;
	char *p;
	char *i;

	Rio_readinitb(&rio, connfd);
	while((n = Rio_readlineb(&rio, buff, MAXLINE)) != 0) { //line:netp:echo:eof
		printf("server received %d bytes\nbuff: %o\n", (int)n, buff);
		for(p = buff; *p != '\n'; p++);
		fgets(p, MAXLINE, stdin);
		printf("add: %d\n", strlen(buff));
		Rio_writen(connfd, buff, strlen(buff));
		printf("buff: %o\n", buff);
		for(i = buff; i < buff + 10; i++)
		{
			printf("%c", *i);
		}
		printf("\n");
	}
}
/* $end echo */


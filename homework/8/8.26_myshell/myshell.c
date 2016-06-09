#include <csapp.h>
#define MAXARGS 128

void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_cmd(char **argv);

int main()
{
	char cmdline[MAXLINE];
	
	while(1)
	{	printf("> ");
		Fgets(cmdline, MAXLINE, stdin);
		if(feof(stdin))
			exit(0);
		eval(cmdline);
	}
}

void eval(char *cmdline)
{
	char buf[MAXLINE];
	char *argv[MAXARGS];
	int bg;
	pid_t pid;
	
	strcpy(buf, cmdline);
	bg = parseline(buf, argv);
	if(argv[0] == NULL)
		return;
	if(!builtin_cmd(argv))
	{	
		if((pid = Fork()) == 0)
		{	
			if(execve(argv[0], argv, environ) < 0)
			{		
				printf("%s : Command not find.\n", argv[0]);
				exit(0);
			}
		}
		if(!bg)
		{
			int status;
			if(waitpid(pid, &status, 0) < 0)
				fprintf(stderr, "waitpid error: %s.\n", strerror(errno));
		}
		else
			printf("%d %s", pid, cmdline);
	}
	return;
}

int builtin_cmd(char **argv)
{
	if(!strcmp(argv[0], "quit"))
		exit(0);
	if(!strcmp(argv[0], "&"))
		return 1;
	return 0;
}

int parseline(char *buf, char **argv)
{
	char *bufp = buf;
	int i = 0;
	int n = strlen(buf);
	
	if(n == MAXLINE - 1)
	{
	  fprintf(stderr, "error: arguments too long.\n");
	  exit(0);
	}
	
	buf[n - 1] = 0;
	argv[0] = NULL;
	
	if(*bufp == 0)
	      return 0;
	else if(*bufp != ' ')
		  argv[i++] = bufp;
		  
	for(bufp += 1; *bufp != 0; bufp++)
	{
		if(*bufp == ' ')
		{
		  if(*(bufp - 1) != ' ' && *(bufp - 1) != 0)
		     *bufp = 0;
		}
		else if(*(bufp -1) == ' ' || *(bufp - 1) == 0)
			argv[i++] = bufp;
	}
	
	argv[i] = NULL;
	
	if(i > 0)
	{
	    if(!strcmp(argv[i - 1], "&"))
		return 1;
	    else
		return 0;
	}
	return 0;
}

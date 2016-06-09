#include <csapp.h>
#define MAXARGS 128

struct job {
	pid_t pid;
	int jid;
	int state;
	struct job *pre;
	struct job *next;
	char *argv[MAXARGS];
	char buf[MAXLINE];
}head, tail;

pid_t shellpid = 0;
pid_t forepid = 0;

void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_cmd(char **argv);
void initjobs();
void addjob(pid_t pid, char *cmdline);
void deletejob(pid_t pid);
struct job *selectjob(pid_t pid);
void joblist();
void sigchld_handler(int sig);
void sigint_handler(int sig);
void sigtstp_handler(int sig);
pid_t j2p(int jid);
int p2j(pid_t pid);
int fbg_argv_ok(char **argv, pid_t *pid_ptr, int *jid_ptr);
void fg_waitpid(pid_t pid);
void restart(pid_t pid);
void front_or_back(char **argv, int fg_or_not);


int main() {
	char cmdline[MAXLINE];

	forepid = 0;
	shellpid = getpid();
	printf("shell pgrp=%d\n", getpgrp());
	
	while (signal(SIGINT, sigint_handler) == SIG_ERR)
		;
	while (signal(SIGTSTP, sigtstp_handler) == SIG_ERR)
		;
	while (signal(SIGCHLD, sigchld_handler) == SIG_ERR)
		;
	initjobs();
	while (1) {	
		printf("myshell> ");
		Fgets(cmdline, MAXLINE, stdin);
		if (feof(stdin))
			exit(0);
		eval(cmdline);
	}
}

void initjobs() {
	head.pid = 0;
	head.jid = 0;
	head.state = 0;
	head.pre = NULL;
	head.next = &tail;
	tail.pid = 0;
	tail.jid = 0;
	tail.state = 0;
	tail.pre = &head;
	tail.next = NULL;
	return;
}

void addjob(pid_t pid, char *cmdline) {
	struct job *p;
	
	if (pid < 1)
		return;
	p = (struct job *)malloc(sizeof(struct job));
	p->pid = pid;
	p->jid = (tail.pre)->jid + 1;
	p->state = 1;
	p->pre = tail.pre;
	p->next = (tail.pre)->next;
	(tail.pre)->next = p;
	tail.pre = p;
	strcpy(p->buf, cmdline);
	parseline(p->buf, p->argv);
	return;
}

void deletejob(pid_t pid) {
	struct job *p = &head;
	int i = 0;

	if (pid < 1)
		return;
	while (p != &tail) {
		if (p->pid == pid) {
			p->pre->next = p->next;
			p->next->pre = p->pre;
			free(p);
			for (p = &head; p != &tail; p = p->next) {
				p->jid = i++;
			}
			break;
		}
		p = p->next;
	}
	return;
}

struct job *selectjob(pid_t pid) {	
	struct job *p = &head;

	if (pid < 1)
		return NULL;
	while (p != &tail) {
		if (p->pid == pid) {
			break;
		}
		p = p->next;
	}
	if (p == &tail)
		return NULL;
	return p;
}

void joblist() {
	struct job *p = head.next;
	int i;

	while (p != &tail) {
		printf("[%d] %d %s   ", p->jid, p->pid, p->state ? "Running":"Stopped");
		for (i = 0; p->argv[i] != NULL; i++)
			printf("%s ", p->argv[i]);
		printf("\n");
		p = p->next;
	}
}

void sigint_handler(int sig) {
	if (forepid)
		kill(forepid, SIGKILL);
}

void sigtstp_handler(int sig) {
	if (forepid)
		kill(forepid, SIGTSTP);
}

void sigchld_handler(int sig) {
	pid_t pid;
	int status;
	struct job *p;
	if ((pid = waitpid(-1, &status, WUNTRACED)) > 0) {
		if (WIFEXITED(status)) {
			deletejob(pid);
			fprintf(stderr, "job %d terminated normally with exit status=%d\n",
					pid, WEXITSTATUS(status));
			if (pid == forepid) {
				forepid = 0;
				kill(shellpid, SIGCONT);
			}
		}
		else if (WIFSTOPPED(status)) {
			p = selectjob(pid);
			p->state = 0;
			fprintf(stderr, "job [%d] %d stopped by signal: Stopped\n", 
					p2j(pid), pid);
			if (pid == forepid) {
				kill(shellpid, SIGCONT);
			}
		}
		else if (WIFSIGNALED(status)) {
			deletejob(pid);
			fprintf(stderr, "job %d terminated by ", pid);
			psignal(WTERMSIG(status), "signal");
			if (pid == forepid) {
				forepid = 0;
				kill(shellpid, SIGCONT);
			}
		}
	}
	else if (errno != ECHILD)
		fprintf(stderr, "waitpid error: %s\n", 
				strerror(errno));
	printf("handler reaped pid=%d, ready to return right now.\n", pid);
	return;
}

void eval(char *cmdline) {	
	char buf[MAXLINE];
	char *argv[MAXARGS];
	int bg;
	int i;
	struct job *p;
	pid_t pid;
	sigset_t mask;

	strcpy(buf, cmdline);
	bg = parseline(buf, argv);
	if (argv[0] == NULL)
		return;
	if (!builtin_cmd(argv)) {
		sigemptyset(&mask);
		sigaddset(&mask, SIGCHLD);
		sigprocmask(SIG_BLOCK, &mask, NULL);

		if ((pid = Fork()) == 0) {

			while (signal(SIGINT, SIG_IGN) == SIG_ERR)
				;
			if (bg) {
				printf("bg pgrp=%d, forepid=%d\n", getpgrp(), forepid);
			}
			else {
				forepid = getpid();
				printf("fg pgrp=%d, forepid=%d\n", getpgrp(), forepid);
			}
			if (execve(argv[0], argv, environ) < 0) {		
				printf("%s : Command not found.\n", 
					argv[0]);
				exit(0);
			}
		}
		printf("parent first\n");
		addjob(pid, cmdline);
		sigprocmask(SIG_UNBLOCK, &mask, NULL);
		if (bg) {
			p = selectjob(pid);
			printf("[%d] %d ", p->jid, p->pid);
			for (i = 0; p->argv[i] != NULL; i++)
				printf("%s ", p->argv[i]);
			printf("\n");
		}
		else {
			printf("ready to fg_waitpid\n");
			pause();
			printf("fg_waitpid end\n");
		}
	}
	return;
}

pid_t j2p(int jid) {
	struct job *p = &head;
	int i;
	
	if (jid < 1)
		return -1;
	for (i = 0; i < jid; i++) {
		p = p->next;
		if (p == &tail)
			return -1;
	}
	return p->pid;
}

int p2j(pid_t pid) {
	struct job *p = &head;
	int i = 0;
	
	if (pid < 1)
		return -1;
	while (p != &tail) {
		if (p->pid == pid)
			break;
		p = p->next;
		i++;
	}
	if (p == &tail)
		return -1;
	return i;
}

int fbg_argv_ok(char **argv, pid_t *pid_ptr, int *jid_ptr) {
	char *p = argv[1];

	if (argv[1] == NULL || argv[2] != NULL)
		return 0;
	if (*p != '%' && (*p < '0' || *p > '9'))
		return 0;
	if (*p == '%' && *(p + 1) == 0)
		return 0;
	for (p += 1; *p != 0; p++)
		if (*p < '0' || *p > '9')
			return 0;
	p = argv[1];
	if (*p == '%') {
		if ((*jid_ptr = atoi(p + 1)) <= 0)
			return 0;
	}
	else if ((*pid_ptr = atoi(p)) <= 0)
		return 0;
	return 1;
}

void restart_job(pid_t pid) {	
	struct job *p;
		
	p = selectjob(pid);	
	while (kill(pid, SIGCONT) < 0)
		;
	p->state = 1;
	return;
}

void fg_waitpid(pid_t pid) {
	int status;
	pid_t r_pid;
	struct job *p;

	if ((r_pid = waitpid(pid, &status, WUNTRACED)) > 0) {
		if (WIFEXITED(status)) {
			deletejob(r_pid);
			fprintf(stderr, "job %d terminated normally with exit status=%d\n",
					r_pid, WEXITSTATUS(status));
		}
		else	if (WIFSTOPPED(status)) {
			p = selectjob(r_pid);
			p->state = 0;
			fprintf(stderr, "job [%d] %d stopped by signal: Stopped\n", 
					p->jid, r_pid);
		}
		else	if (WIFSIGNALED(status)) {
			deletejob(r_pid);
			fprintf(stderr, "job %d terminated by ", r_pid);
			psignal(WTERMSIG(status), "signal");
		}
	}
	else	if (errno != ECHILD)
		fprintf(stderr, "waitpid error: %s\n", 
				strerror(errno));
	printf("foreground reaped pid=%d, ready to return\n", r_pid);
	return;
}

void front_or_back(char **argv, int fg_or_not) {
	int jid = 0;
	int fg_run = fg_or_not;
	pid_t pid = 0;

	if (fbg_argv_ok(argv, &pid, &jid)) {
		if (pid) {
			if (selectjob(pid)) {
				restart_job(pid);
				if (fg_run) {
					forepid = pid;
					pause();
				}
			}
			else
				fprintf(stderr, "%d: No such process\n", pid);
		}
		else {   
			if (selectjob(j2p(jid))) {
				restart_job(j2p(jid));
				if (fg_run) {
					forepid = j2p(jid);
					pause();
				}
			}
			else
				fprintf(stderr, "%d: No such process\n", j2p(jid));
		}
	}
	else
		fprintf(stderr, "usage: %s <%%jid> or %s <pid>\n",
				argv[0], argv[0]);
	return;
}

int builtin_cmd(char **argv) {

	if (!strcmp(argv[0], "quit"))
		exit(0);
	if (!strcmp(argv[0], "&"))
		return 1;
	if (!strcmp(argv[0], "jobs")) {
		joblist();
		return 1;
	}
	if (!strcmp(argv[0], "fg")) {
		front_or_back(argv, 1);
		return 1;
	}
	if (!strcmp(argv[0], "bg")) {
		front_or_back(argv, 0);
		return 1;
	}
	return 0;
}

int parseline(char *buf, char **argv) {
	char *bufp = buf;
	int i = 0;
	int n = strlen(buf);

	if (n == MAXLINE - 1) {
		fprintf(stderr, "error: arguments too long.\n");
		exit(0);
	}

	buf[n - 1] = 0;
	argv[0] = NULL;

	if (*bufp == 0)
		return 0;
	else if (*bufp != ' ')
		argv[i++] = bufp;

	for (bufp += 1; *bufp != 0; bufp++) {
		if (*bufp == ' ') {
			if (*(bufp - 1) != ' ' && *(bufp - 1) != 0)
				*bufp = 0;
		}
		else if (*(bufp -1) == ' ' || *(bufp - 1) == 0)
			argv[i++] = bufp;
	}

	argv[i] = NULL;

	if (i > 0) {
		if (!strcmp(argv[i - 1], "&"))
			return 1;
		else
			return 0;
	}
	return 0;
}

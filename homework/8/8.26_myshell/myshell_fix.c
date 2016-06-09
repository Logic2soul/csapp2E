#include <csapp.h>
#define MAXARGS 128

struct job {
	pid_t pid;
	int jid;
	int state;
	struct job *pre;
	struct job *next;
	char job_buf[MAXLINE];
}head, tail;

pid_t forepid = 0;
sigjmp_buf env;

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
void sigalarm_handler(int sig);
pid_t j2p(int jid);
int p2j(pid_t pid);
pid_t fbg_argv_to_pid(char **argv);
void fg_waitpid(pid_t pid);
void restart(pid_t pid);
void bg_restart(pid_t pid);
void frontground(char **argv);
void bg_job_info(pid_t pid);
void background(char **argv);
void child_block();
void child_unblock();

int main() {
	char cmdline[MAXLINE];
	
	forepid = 0;
	printf("tcgetsid: %d\ngetsid=%d\ntcgetpgrp=%d\nmyshell pgrp=%d\n", 
		tcgetsid(STDIN_FILENO), getsid(getpid()), tcgetpgrp(STDIN_FILENO), getpgrp());
	
	signal(SIGINT, sigint_handler);
	signal(SIGTSTP, sigtstp_handler);
	signal(SIGCHLD, sigchld_handler);
	signal(SIGALRM, sigalarm_handler);
	initjobs();
	sigsetjmp(env, 1);
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
	p->next = &tail;
	(tail.pre)->next = p;
	tail.pre = p;
	strcpy(p->job_buf, cmdline);
	return;
}

struct job *selectjob(pid_t pid) {	
	struct job *p = &head;

	if (pid < 1)
		return NULL;
	p = p->next;
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

void deletejob(pid_t pid) {
	struct job *p;
	struct job *p1;

	if (pid < 1)
		return;
	if (p = selectjob(pid)) {
		for (p1 = p->next; p1 != &tail; p1 = p1->next) {
			p1->jid = p1->jid - 1;
		}
		p->pre->next = p->next;
		p->next->pre = p->pre;
		free(p);
	}
	return;
}

void joblist() {
	struct job *p = head.next;
	int i;

	while (p != &tail) {
		printf("[%d] %d %s   %s", p->jid, p->pid, 
					p->state ? "Running":"Stopped", p->job_buf);
		p = p->next;
	}
	return;
}

void child_block() {
	sigset_t mask;

	sigemptyset(&mask);
	sigaddset(&mask, SIGCHLD);
	sigprocmask(SIG_BLOCK, &mask, NULL);
	return;
}

void child_unblock() {
	sigset_t mask;

	sigemptyset(&mask);
	sigaddset(&mask, SIGCHLD);
	sigprocmask(SIG_UNBLOCK, &mask, NULL);
	return;
}

void sigint_handler(int sig) {
	child_block();
	if (forepid) {
		kill(-forepid, SIGINT);
		printf("\nkilled SIGINT to forepid=%d already\n", forepid);
	}
	else 
		printf("\nno forepid to int\n");
	child_unblock();	
	siglongjmp(env, 1);
}

void sigtstp_handler(int sig) {
	child_block();
	if (forepid) {
		kill(-forepid, SIGTSTP);
		printf("\nkilled SIGTSTP to forepid=%d already\n", forepid);
	}
	else
		printf("\nno forepid to tstp\n");
	child_unblock();	
	siglongjmp(env, 1);
}

void sigalarm_handler(int sig) {
	printf("get out\n");
	siglongjmp(env, 1);
}

void sigchld_handler(int sig) {
	pid_t pid;
	int status;
	struct job *p;
	if ((pid = waitpid(-1, &status, WUNTRACED)) > 0) {
		printf("\nhandler reaped, forepid=%d, r_pid=%d\n", forepid, pid);
		if (pid == forepid) {
			forepid = 0;
			printf("no fg: forepid=%d\n", forepid);
			fprintf(stderr, "fg: ");
		}
		if (WIFEXITED(status)) {
			deletejob(pid);
			fprintf(stderr, "job %d terminated normally with exit status=%d\n",
					pid, WEXITSTATUS(status));
		}
		else if (WIFSTOPPED(status)) {
			p = selectjob(pid);
			p->state = 0;
			fprintf(stderr, "job [%d] %d stopped by signal: Stopped\n",
					p->jid, pid);
		}
		else if (WIFSIGNALED(status)) {
			deletejob(pid);
			fprintf(stderr, "job %d terminated by ", pid);
			psignal(WTERMSIG(status), "signal");
		}
	}
	else if (errno != ECHILD)
		fprintf(stderr, "\nwaitpid error: %s\n", strerror(errno));
	printf("handler reaped pid=%d, ready to return\n", pid);
	return;
}

void eval(char *cmdline) {	
	char buf[MAXLINE];
	char *argv[MAXARGS];
	int bg, b;
	pid_t pid;

	strcpy(buf, cmdline);
	bg = parseline(buf, argv);
	if (argv[0] == NULL)
		return;
	if (b = builtin_cmd(argv)) {
		printf("do inside argument\n");
		return;
	}
	else {
		child_block();
		printf("do out argument\n");

		if ((pid = Fork()) == 0) {
			signal(SIGINT, SIG_DFL);
			signal(SIGTSTP, SIG_DFL);
			signal(SIGCHLD, SIG_DFL);
			signal(SIGALRM, SIG_DFL);
			child_unblock();
			if (execve(argv[0], argv, environ) < 0) {		
				printf("%s : Command not found.\n", 
					argv[0]);
				exit(0);
			}
		}
		addjob(pid, cmdline);
		setpgid(pid, pid);
		if (bg) {
			bg_job_info(pid);
			child_unblock();
			return;
		}
		if (!bg) {
			forepid = pid;
			child_unblock();
			printf("start fg_waitpid\n");
			fg_waitpid(pid);
			printf("end fg_waitpid\n");
			return;
		}
		child_unblock();
	}
	return;
}

pid_t j2p(int jid) {
	struct job *p = &head;
	int i = 0;

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
	struct job *p;
	
	if (pid < 1)
		return -1;
	if (p = selectjob(pid))
		return p->jid;
	return -1;
}

pid_t fbg_argv_to_pid(char **argv) {
	pid_t pid = 0;
	int jid = 0;
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
		if ((jid = atoi(p + 1)) < 1)
			return 0;
		if ((pid = j2p(jid)) < 1) {
			printf("no such process\n");
			return 0;
		}
		return pid;
	}
	if ((pid = atoi(p)) < 1)
		return 0;
	if (selectjob(pid) == NULL) {
		printf("no such process\n");
		return 0;
	}
	return pid;
}

void restart_job(pid_t pid) {	
	struct job *p;

	p = selectjob(pid);
	p->state = 1;
	kill(pid, SIGCONT);
	printf("killed SIGCONT to pid=%d\n", pid);
	return;
}

void bg_restart_job(pid_t pid) {	
	struct job *p;
	struct itimerval t;
	t.it_interval.tv_usec = 0;
	t.it_interval.tv_sec = 0;
	t.it_value.tv_usec = 100000;
	t.it_value.tv_sec = 0;

	p = selectjob(pid);
	p->state = 1;
	bg_job_info(pid);
	setitimer(ITIMER_REAL, &t, NULL);
	kill(pid, SIGCONT);
	printf("killed SIGCONT to pid=%d\n", pid);
	pause();
	return;
}

void bg_job_info(pid_t pid) {
	struct job *p;
	int i;

	if ((p = selectjob(pid)) == NULL) {
		printf("no such process\n");
		return;
	}
	printf("[%d] %d %s   %s", p->jid, p->pid, 
				p->state ? "Running":"Stopped", p->job_buf);
	return;
}

void background(char **argv) {
	pid_t pid = 0;

	if (pid = fbg_argv_to_pid(argv)) {
		bg_restart_job(pid);
	}
	else 
		fprintf(stderr, "usage: bg <%%jid> or bg <pid>\n");
	return;
}

void fg_waitpid(pid_t pid) {
	int status;
	pid_t r_pid = 0;
	struct job *p = NULL;

	if ((r_pid = waitpid(pid, &status, 0)) > 0) {
		printf("fg reaped, forepid=%d, r_pid=%d\n", forepid, r_pid);
		forepid = 0;
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
	else if (errno != ECHILD)
		fprintf(stderr, "waitpid error: %s\n", 
				strerror(errno));
	printf("fg reaped pid=%d, ready to return\n", r_pid);
	return;
}

void frontground(char **argv) {
	pid_t pid = 0;

	if (pid = fbg_argv_to_pid(argv)) {
		child_block();
		restart_job(pid);
		forepid = pid;
		child_unblock();
		fg_waitpid(pid);
	}
	else
		fprintf(stderr, "usage: fg <%%jid> or fg <pid>\n");
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
		frontground(argv);
		return 1;
	}
	if (!strcmp(argv[0], "bg")) {
		background(argv);
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

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
void sigint_handler(int sig);      /* delete */
void sigtstp_handler(int sig);     /* delete */
pid_t j2p(int jid);
int p2j(pid_t pid);
int fbg_argv_ok(char **argv, pid_t *pid_ptr, int *jid_ptr);
void fg_waitpid(pid_t pid);   /* delete */
void restart(pid_t pid);
void front_or_back(char **argv, int fg_or_not);
void child_block();
void child_unblock();
void control_block();
void control_unblock();

int main() {
	char cmdline[MAXLINE];
	
	control_block();
	shellpid = getpgrp();
	forepid = 0;
	printf("tcgetsid: %d\ngetsid=%d\ntcgetpgrp=%d\nmyshell pgrp=%d\n", 
		tcgetsid(STDIN_FILENO), getsid(getpid()), tcgetpgrp(STDIN_FILENO), getpgrp());
	
	if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
		printf("signal error\n");
		exit(-1);
	}
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

void control_block() {
	sigset_t mask;

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGTSTP);
	sigprocmask(SIG_BLOCK, &mask, NULL);
}

void control_unblock() {
	sigset_t mask;

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGTSTP);
	sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

void child_block() {
	sigset_t mask;

	sigemptyset(&mask);
	sigaddset(&mask, SIGCHLD);
	sigprocmask(SIG_BLOCK, &mask, NULL);
}

void child_unblock() {
	sigset_t mask;

	sigemptyset(&mask);
	sigaddset(&mask, SIGCHLD);
	sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

void sigint_handler(int sig) {
	if (forepid) {
		kill(-forepid, SIGINT);
		printf("int forepid ok\n");
	}
	else 
		printf("no forepid to int\n");
}

void sigtstp_handler(int sig) {
	if (forepid) {
		kill(-forepid, SIGTSTP);
		printf("tstp forepid ok\n");
	}
	else
		printf("no forepid to tstp\n");
}

void sigchld_handler(int sig) {
	pid_t pid;
	int status;
	struct job *p;
	if ((pid = waitpid(-1, &status, WUNTRACED)) > 0) {
		if (WIFEXITED(status)) {
			deletejob(pid);
			if (pid == forepid) {
				forepid = 0;
				setpgid(pid, pid);
				fprintf(stderr, "foreground job %d terminated normally with exit status=%d\n",
						pid, WEXITSTATUS(status));
				siglongjmp(env, 1);
			}
			else
				fprintf(stderr, "job %d terminated normally with exit status=%d\n",
						pid, WEXITSTATUS(status));
		}
		else if (WIFSTOPPED(status)) {
			p = selectjob(pid);
			p->state = 0;
			if (pid == forepid) {
				forepid = 0;
				setpgid(pid, pid);
				fprintf(stderr, "foreground job [%d] %d stopped by signal: Stopped\n",
						p->jid, pid);
				siglongjmp(env, 1);
			}
			else
				fprintf(stderr, "job [%d] %d stopped by signal: Stopped\n",
						p->jid, pid);
		}
		else if (WIFSIGNALED(status)) {
			deletejob(pid);
			if (pid == forepid) {
				forepid = 0;
				setpgid(pid, pid);
				fprintf(stderr, "foreground job %d terminated by ", pid);
				psignal(WTERMSIG(status), "signal");
				siglongjmp(env, 1);
			}
			else
			fprintf(stderr, "job %d terminated by ", pid);
			psignal(WTERMSIG(status), "signal");
		}
	}
	else if (errno != ECHILD)
		fprintf(stderr, "waitpid error: %s\n", strerror(errno));
	printf("handler reaped pid=%d, ready to return right now.\n", pid);
	return;
}

void eval(char *cmdline) {	
	char buf[MAXLINE];
	char *argv[MAXARGS];
	int bg;
	int i;
	struct job *p;
	pid_t pid, tcpid;

	strcpy(buf, cmdline);
	bg = parseline(buf, argv);
	if (argv[0] == NULL)
		return;
	if (!builtin_cmd(argv)) {
		child_block();

		if ((pid = Fork()) == 0) {
			child_unblock();
			control_unblock();
			if (signal(SIGCHLD, SIG_DFL) == SIG_ERR) {
				printf("signal error\n");
				exit(-1);
			}
			if (execve(argv[0], argv, environ) < 0) {		
				printf("%s : Command not found.\n", 
					argv[0]);
				exit(0);
			}
		}
		addjob(pid, cmdline);
		if (bg) {
			setpgid(pid, pid);
			p = selectjob(pid);
			printf("[%d] %d ", p->jid, p->pid);
			for (i = 0; p->argv[i] != NULL; i++)
				printf("%s ", p->argv[i]);
			printf("\n");
		}
		else {
			forepid = pid;
			child_unblock();
			printf("ready to fg_waitpid\n");
			if (!sigsetjmp(env, 0)) {
				while (1)
					sleep(50);
				printf("u never see me\n");
			}
			else
				printf("no fg_waitpid\n");
			return;
		}
		child_unblock();
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
	kill(pid, SIGCONT);
	p->state = 1;
	return;
}

void fg_waitpid(pid_t pid) {
	int status;
	pid_t r_pid;
	struct job *p;

	if ((r_pid = waitpid(pid, &status, 0)) > 0) {
		printf("fg_waitpid in, forepid=%d, r_pid=%d\n", forepid, r_pid);
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
	pid_t j2pid = 0;

	if (fbg_argv_ok(argv, &pid, &jid)) {
		if (pid) {
			if (selectjob(pid)) {
				child_block();
				restart_job(pid);
				if (fg_run) {
					forepid = pid;
					setpgid(pid, shellpid);
					child_unblock();
					printf("bg ready to fg\n");
					while (1) 
						sleep(50);
					printf("u never see me\n");
					return;
				}
				else {
					child_unblock();
					return;
				}
			}
			else {
				fprintf(stderr, "%d: No such process\n", pid);
				return;
			}
		}
		else {   
			j2pid = j2p(jid);
			if (selectjob(j2pid)) {
				child_block();
				restart_job(j2pid);
				if (fg_run) {
					forepid = j2pid;
					setpgid(j2pid, shellpid);
					child_unblock();
					printf("bg ready to fg\n");
					while (1)
						sleep(50);
					printf("u never see me\n");
					return;
				}
				else {
					child_unblock();
					return;
				}
			}
			else {
				fprintf(stderr, "%d: No such process\n", j2p(jid));
				return;
			}
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

#include "csapp.h"

void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri, char *filename, char *cgiargs);
void get_filetype(char *filename, char *filetype);
void sigchld_handler(int sig);
void serve_dynamic(char *method, int fd, char *filename, char *cgiargs);
void serve_static(char *method, int fd, char *filename, int filesize);
void clienterror(int fd, char *cause, char *errnum,
			char *shortmsg, char *longmsg);

int main(int argc, char **argv) {
	
	int listenfd, connfd, port, clientlen;
	struct sockaddr_in clientaddr;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(1);
	}
	port = atoi(argv[1]);

	listenfd = Open_listenfd(port);
	if (signal(SIGCHLD, sigchld_handler) == SIG_ERR) {
		printf("signal error\n");
		exit(1);
	}
	while (1) {
		clientlen = sizeof(clientaddr);
		connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
		doit(connfd);
		close(connfd);
	}
}

void doit(int fd) {
	
	int is_static, logfd;
	struct stat sbuf;
	char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
	char filename[MAXLINE], cgiargs[MAXLINE];
	rio_t rio;
	FILE *fp;

	logfd = open("./out_log", O_RDWR|O_CREAT|O_APPEND, 0);
	if (!(fp = fdopen(logfd, "a+"))) {
		printf("fdopen error: %s\n", strerror(errno));
		Close(logfd);
		return;
	}
	Rio_readinitb(&rio, fd);
	Rio_readlineb(&rio, buf, MAXLINE);
	Rio_writen(logfd, rio.rio_buf, strlen(rio.rio_buf));
	sscanf(buf, "%s %s %s", method, uri, version);
	read_requesthdrs(&rio);
	if (!strcasecmp(method, "POST")) {

	}
	else if ((!strcasecmp(method, "GET")) || (!strcasecmp(method, "HEAD"))) {
		is_static = parse_uri(uri, filename, cgiargs);
		fprintf(fp, "filename: %s\ncgiargs: %s\n\n", filename, cgiargs);
		fflush(fp);
		if (stat(filename, &sbuf) < 0) {
			clienterror(fd, filename, "404", "Not Found",
					"Tiny couldn't find this file");
			clienterror(logfd, filename, "404", "Not Found",
					"Tiny couldn't find this file");
		}
		else if (is_static) {
			if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
				clienterror(fd, filename, "403", "Forbidden",
						"Tiny couldn't read file");
				clienterror(logfd, filename, "403", "Forbidden",
						"Tiny couldn't read file");
			} else {
			serve_static(method, fd, filename, sbuf.st_size);
			serve_static(method, logfd, filename, sbuf.st_size);
			}
		}
		else {
			if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
				clienterror(fd, filename, "403", "Forbidden",
						"Tiny couldn't run the CGI program");
				clienterror(logfd, filename, "403", "Forbidden",
						"Tiny couldn't run the CGI program");
			} else {
			serve_dynamic(method, fd, filename, cgiargs);
			serve_dynamic(method, logfd, filename, cgiargs);
			}
		}
	}
	else {
		clienterror(fd, method, "501", "Not Implemented", 
				"Tiny does not implement this method");
		clienterror(logfd, method, "501", "Not Implemented", 
				"Tiny does not implement this method");
	}
	Close(logfd);
}

void clienterror(int fd, char *cause, char *errnum,
		char *shortmsg, char *longmsg)
{
	char buf[MAXLINE], body[MAXLINE];

	sprintf(body, "<html><title>Tiny Error</title>");
	sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
	sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
	sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
	sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

	sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
	Rio_writen(fd, buf, strlen(buf));
	sprintf(buf, "Content-type: text/html\r\n");
	Rio_writen(fd, buf, strlen(buf));
	sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
	Rio_writen(fd, buf, strlen(buf));
	Rio_writen(fd, body, strlen(body));
}

void read_requesthdrs(rio_t *rp) {
	char buf[MAXLINE];

	Rio_readlineb(rp, buf, MAXLINE);
	while(strcmp(buf, "\r\n")) {
		printf("%s", buf);
		Rio_readlineb(rp, buf, MAXLINE);
	}
	return;
}

int parse_uri(char *uri, char *filename, char *cgiargs) {
	char *p;

	if (!strstr(uri, "cgi-bin")) {
		strcpy(cgiargs, "");
		strcpy(filename, ".");
		strcat(filename, uri);
		if (uri[strlen(uri) - 1] == '/')
			strcat(filename, "home.html");
		return 1;
	}
	else {
		p = strchr(uri, '?');
		if (p) {
			*p = '\0';
			strcpy(cgiargs, p + 1);
		}
		else 
			strcpy(cgiargs, "");
		strcpy(filename, ".");
		strcat(filename, uri);
		return 0;
	}
}
		 
void serve_static(char *method, int fd, char *filename, int filesize) {
	int srcfd;
	char *srcp, filetype[MAXLINE], buf[MAXBUF];

	get_filetype(filename, filetype);
	sprintf(buf, "HTTP/1.0 200 OK\r\n");
	sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
	sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
	sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
	Rio_writen(fd, buf, strlen(buf));
	
	if (strcasecmp(method, "GET"))
		return;
	srcfd = Open(filename, O_RDONLY, 0);
	srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
	Close(srcfd);
	Rio_writen(fd, srcp, filesize);
	Munmap(srcp, filesize);
}

void get_filetype(char *filename, char *filetype) {
	if (strstr(filename, ".html"))
		strcpy(filetype, "text/html");
	else if (strstr(filename, ".gif"))
		strcpy(filetype, "image/gif");
	else if (strstr(filename, ".jpg"))
		strcpy(filetype, "image/jpeg");
	else
		strcpy(filetype, "text/plain");
}

void sigchld_handler(int sig) {
	pid_t pid;
	while ((pid = waitpid(-1, NULL, WNOHANG)) > 0)
		printf("child %d reaped\n", pid);
	if (pid && errno != ECHILD) {
		printf("waitpid error\n");
		exit(1);
	}
	return;
}

void serve_dynamic(char *method, int fd, char *filename, char *cgiargs) {
	char buf[MAXLINE];
	char *emptylist[] = { NULL };
	int status;

	sprintf(buf, "HTTP/1.0 200 OK\r\nServer: Tiny Web Server\r\n");
	Rio_writen(fd, buf, strlen(buf));
	
	if (strcasecmp(method, "GET"))
		return;
	if (Fork() == 0) {
		signal(SIGCHLD, SIG_DFL);
		setenv("QUERY_STRING", cgiargs, 1);
		Dup2(fd, STDOUT_FILENO);
		Execve(filename, emptylist, environ);
	}
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/shm.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv){
	if(argc != 2){
		printf("usage:%s <port>\n", argv[0]);
		exit(-1);
	}
	int s_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == s_fd){
		perror("socket");
		exit(-2);
	}
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons((unsigned short)atoi(argv[1]));
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	if(-1 == bind(s_fd, (struct sockaddr *)&server, sizeof(server))){
		perror("bind");
		exit(-3);
	}
	struct sockaddr client = {0};
	int clientlen = sizeof(client);
	char talk[40] = {0};
	char buf[1024] = {0};
	while(1){
		if(-1 == listen(s_fd, 1024)){
			perror("listen");
			exit(-4);
		}
		int con_fd = accept(s_fd, (struct sockaddr *)&client, &clientlen);
		if(-1 == con_fd){
			perror("accept");
			exit(-5);
		}
		/*print ip address of the connected client*/
		struct sockaddr_in *sap = (struct sockaddr_in*)&client;
		char *c_ad_str = inet_ntoa(sap->sin_addr);
		printf("client %s connected...\n", c_ad_str);
		pid_t pid = fork();
		if(-1 == pid){
			perror("fork");
			exit(-6);
		}
		if(0 == pid){
			if(-1 == read(con_fd, talk, sizeof(talk))){
				perror("read");
				exit(-7);
			}
			talk[strlen(talk)-1] = '\0';
			char *p = strstr(talk, "@");
			*p = '\0';
			if(-1 == mkfifo(talk, 0644)){
				if(errno == EEXIST);
				else{
					perror("mkfifo");
					exit(-8);
				}
			}
			if(-1 == mkfifo(p+1, 0644)){
				if(errno == EEXIST);
				else{
					perror("mkfifo");
					exit(-8);
				}
			}
			int fd1 = open(talk, O_RDWR);
			if(-1 == fd1){
				perror("open");
				exit(-10);
			}
			int fd2 = open(p+1, O_RDWR);
			if(-1 == fd2){
				perror("open");
				exit(-10);
			}
			pid_t pid2 = fork();
			if(-1 == pid2){
				perror("fork");
				exit(-9);
			}
			if(0 == pid2){
				int r1 = 0;
				char name[128] = {0};
				strcpy(name, talk);
				strcat(name, ": ");
				while((r1 = read(con_fd, buf, 1024)) > 0){
					write(fd2, name, strlen(name));
					write(fd2, buf, r1);
				}
				/*print client disconnected*/
				printf("client %s disconnected...\n", c_ad_str);
			}else{
				int r2 = 0;
				while((r2 = read(fd1, buf, 1024)) > 0){
					write(con_fd, buf, r2);
				}
			}
			exit(1);
		}else{
			close(con_fd);
		}
	}
	close(s_fd);
	return 0;
}

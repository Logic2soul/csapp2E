#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char **argv){
	if(argc != 3){
		printf("usage:%s <host> <port>\n", argv[0]);
		exit(-1);
	}
	int s_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == s_fd){
		perror("socket");
		exit(-2);
	}
	struct sockaddr_in server = {0};
	server.sin_family = AF_INET;
	server.sin_port = htons((unsigned short)atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &server.sin_addr);
	if(-1 == connect(s_fd, (struct sockaddr *)&server, sizeof(server))){
		perror("connect");
		exit(-3);
	}
	char p[4096] = {0};
	pid_t pid = fork();
	if(-1 == pid){
		perror("fork");
		exit(-1);
	}
	if(0 == pid){
		while(fgets(p, 4096, stdin)){
			write(s_fd, p, strlen(p)+1);
		}
	}else{
		int r = 0;
		while((r = read(s_fd, p, 4096)) > 0){
			write(1, p, r);
		}
	}
	close(s_fd);
	return 0;
}


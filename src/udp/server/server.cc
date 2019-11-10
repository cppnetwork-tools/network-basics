#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

void *get_in_addr(struct sockaddr *sa)
{
	if(sa->sa_family == AF_INET){
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

const int readUDPsocket(const int sockfd)
{
	int numBytes = 0;
	char msgbuf[100 + 1] = {0};
	char s[INET6_ADDRSTRLEN] = {0};
	struct sockaddr_storage their_addr = {0};
	socklen_t addr_len = 0;


	addr_len = sizeof their_addr;
	if((numBytes = recvfrom(sockfd, msgbuf, sizeof(msgbuf), 0, 
			(struct sockaddr *)&their_addr, &addr_len)) == -1){
		perror("server: recvfrom ");
		return (-1);
	}

	printf("server: got packet from %s \n", inet_ntop(their_addr.ss_family, 
		get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));

	msgbuf[numBytes] = '\0';
	printf("server: msg length [ %d ] \n", numBytes);
	printf("server: msg received [ %s ] \n", msgbuf);
	return numBytes;
}

const int createUDPsocket(const char * PORT)
{
	int sockfd = 0;
	int rv = 0;
	struct addrinfo hints = {0};
	struct addrinfo *servinfo = nullptr;
	struct addrinfo *walk = nullptr;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	if((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0){
		fprintf(stderr, "getaddrinfo: %s \n", gai_strerror(rv));
		exit(1);
	}

	for(walk = servinfo; walk != NULL; walk = walk->ai_next)
	{
		if((sockfd = socket(walk->ai_family, walk->ai_socktype, walk->ai_protocol)) == -1){
			perror("server: socket ");
			continue;
		}

		if(bind(sockfd, walk->ai_addr, walk->ai_addrlen) == -1){
			close(sockfd);
			perror("server: bind ");
			continue;
		}
		break;
	}

	if(walk == nullptr){
		fprintf(stderr, "server: failed to bind socket \n");
		exit(1);
	}

	freeaddrinfo(servinfo);

	printf("server: waiting for recvfrom ... \n");
	return sockfd;
}

int main(int argc, char **argv)
{
	int sockfd = 0;
	int opt = 0;
	char *port = nullptr;

	while((opt = getopt(argc, argv, "p:")) != -1)
	{
		switch(opt)
		{
			case 'p':
			port = strdup(optarg);
			break;
			case '?':
			fprintf(stderr, "unknown option %c \n", optopt);
			break;
		}
	}

	for(; optind < argc; ++optind){
		printf("extra arguments %s \n", argv[optind]);
	}


	sockfd = createUDPsocket(port);
	readUDPsocket(sockfd);

	close(sockfd);
	if(port) free(port);
	return 0;
}
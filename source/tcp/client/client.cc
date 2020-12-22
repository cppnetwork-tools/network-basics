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

const int connect_socket(const char *ip_addr, const char *port)
{
	int sockfd = 0;
	struct addrinfo hints = {0};
	struct addrinfo *servinfo = nullptr;
	struct addrinfo *walk = nullptr;
	char s[INET6_ADDRSTRLEN] = {0};
	int rv = 0;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if((rv = getaddrinfo(ip_addr, port, &hints, &servinfo)) != 0){
		fprintf(stderr, "getaddrinfo: %s \n", gai_strerror(rv));
		exit(1);
	}

	for(walk = servinfo; walk != nullptr; walk = walk->ai_next)
	{
		if((sockfd = socket(walk->ai_family, walk->ai_socktype, walk->ai_protocol)) == -1){
			perror("client: ");
			continue;
		}

		if(connect(sockfd, walk->ai_addr, walk->ai_addrlen) == -1){
			close(sockfd);
			perror("client: ");
			continue;
		}
		break;
	}

	if( walk == nullptr){
		fprintf(stderr, "client: failed to connect \n");
		return (-1);
	}

	inet_ntop(walk->ai_family, get_in_addr((struct sockaddr *) walk->ai_addr), s, sizeof s);
	printf("client: connected to %s \n", s);

	return sockfd;
}

int main(int argc, char **argv)
{	
	int sockfd = 0;
	int opt = 0;
	char *ip_addr = nullptr;
	char *port = nullptr;

	opterr = 0;
	
	while ((opt = getopt(argc, argv, "i:p:")) != -1)
	{
		switch(opt)
		{
			case 'i':
			printf("case i: %s \n", optarg);
			ip_addr = strdup(optarg);
			break;
			case 'p':
			printf("case p: %s \n", optarg);
			port = strdup(optarg);
			break;
			case '?':
			printf("unknown option: %c \n", optopt);
			break;

			default:
			abort();
		}
	}

	for(; optind < argc; ++optind){
		printf("extra arguments: %s \n", argv[optind]);
	}


	sockfd = connect_socket(ip_addr, port);


	int recvbytes = 0;

	char msgbuf[100+1] = {0};
	recvbytes = recv(sockfd, msgbuf, sizeof(msgbuf), 0);
	if(recvbytes < 0){
		perror("client: ");
	}else{
		printf("recv msg: %s \n", msgbuf);
	}

	if(ip_addr) free(ip_addr);
	if(port) free(port);

	return 0;
}
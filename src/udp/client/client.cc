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

typedef struct addrinfo addrinfo_t;

addrinfo_t *This = nullptr;

void setServerinfo(const addrinfo_t *serverinfo)
{
	if(!This){
		This = (addrinfo_t *) malloc(sizeof(addrinfo_t));
		memcpy((addrinfo_t *)This,(addrinfo_t *) serverinfo, sizeof(addrinfo_t));
	}
	else{
		free(This);
		This = (addrinfo_t *) malloc(sizeof(addrinfo_t));
		memcpy((addrinfo_t *)This,(addrinfo_t *) serverinfo, sizeof(addrinfo_t));
	}

}

addrinfo_t *getServerInfo()
{
	return (addrinfo_t *)This;
}

void *get_in_addr(struct sockaddr *sa)
{
	if(sa->sa_family == AF_INET){
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

const int sendtoServer(const int sockfd)
{
	int numBytes = 0;
	char msgbuf[100 + 1] = {0};
	addrinfo_t *serverinfo = nullptr;
	struct sockaddr_storage server_addr = {0};
	char s[INET6_ADDRSTRLEN] = {0};

	strncpy(msgbuf, "Hello world from udp client! ", sizeof msgbuf);
	
	serverinfo = getServerInfo();
	if(serverinfo) printf("client: serverinfo okay to use \n");
	else{
		printf("client: serverinfo is nullptr \n");
		exit(1);
	}

	if((numBytes = sendto(sockfd, msgbuf, strlen(msgbuf), 0, 
			serverinfo->ai_addr, serverinfo->ai_addrlen)) == -1){
		perror("client: sendto ");
		exit(1);
	}
	memcpy((struct sockaddr *)&server_addr, (struct sockaddr *) serverinfo->ai_addr, sizeof(struct sockaddr));

	printf("client: sent %d bytes to %s \n", numBytes, inet_ntop(server_addr.ss_family, (struct sockaddr *)&server_addr, s, sizeof s));

	return numBytes;
}

const int createUDPsocket(const char *ip_addr, const char *port)
{
	int sockfd = 0;
	int rv = 0;
	addrinfo_t hints = {0};
	addrinfo_t *servinfo = nullptr;
	addrinfo_t *walk = nullptr;


	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if((rv = getaddrinfo(ip_addr, port, &hints, &servinfo)) == -1){
		fprintf(stderr, "getaddrinfo: %s \n", gai_strerror(rv));
		exit(1);
	}

	for(walk = servinfo; walk != nullptr; walk = walk->ai_next){
		if((sockfd = socket(walk->ai_family, walk->ai_socktype, walk->ai_protocol)) == -1){
			perror("client: socket ");
			continue;
		}

		break;
	}

	if(walk == nullptr){
		fprintf(stderr, "client: failed to create socket \n");
		exit(1);
	}
	freeaddrinfo(servinfo);
	setServerinfo(walk);

	return sockfd;
}

int main(int argc, char **argv)
{
	int sockfd = 0;
	int opt = 0;
	char *ip_addr = nullptr;
	char *port = nullptr;

	while((opt = getopt(argc, argv, "i:p:")) != -1)
	{
		switch(opt)
		{
			case 'i':
			ip_addr = strdup(optarg);
			break;
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

	sockfd = createUDPsocket(ip_addr, port);

	sendtoServer(sockfd);
	close(sockfd);
	if(ip_addr) free(ip_addr);
	if(port) free(port);

	return 0;
}
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

constexpr int numberofConnections = 10;

void sigchld_handler(int s)
{
	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;
	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}

void *get_in_addr(struct sockaddr *sa)
{
	if(sa->sa_family == AF_INET){
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

const int create_tcp_socket(const char *PORT)
{
	int sockfd = 0;
	struct addrinfo hints;
	struct addrinfo *servinfo = nullptr; 
	struct addrinfo *walk = nullptr;
	int enable = 1;
	int rv = 0;


	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0){
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	} 

	for( walk = servinfo; walk != NULL; walk = walk->ai_next){
		if((sockfd = socket(walk->ai_family, walk->ai_socktype, walk->ai_protocol)) == -1){
			perror("server: socket ");
			continue;
		}

		if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1){
			perror("server: setsockopt ");
			exit(1);
		}

		if(bind(sockfd, walk->ai_addr, walk->ai_addrlen) == -1){
			close(sockfd);
			perror("server: bind ");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo);
	if(walk == NULL){
		fprintf(stderr, "server: failed to bind \n");
		exit(1);
	}

	return sockfd;
}

void acceptConnections(const int serverFd)
{
	socklen_t sin_size = 0;
	struct sockaddr_storage their_addr = {0};
	struct sigaction sa = {0};
	char s[INET6_ADDRSTRLEN] = {0};
	int clientConnected = 0;

	if(listen(serverFd, numberofConnections) == -1){
		perror("server: listen ");
		exit(1);
	}

	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if(sigaction(SIGCHLD, &sa, NULL) == -1){
		perror("sigaction ");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while(1){
		sin_size = sizeof their_addr;
		int newfd = accept(serverFd, (struct sockaddr *)&their_addr, &sin_size);
		if(newfd == -1){
			perror("server: accept ");
			continue;
		}

		inet_ntop(their_addr.ss_family, (struct sockaddr *)&their_addr, s, sizeof s);
		printf("server: got connection from %s \n", s);

		pid_t pid = fork();

		switch(pid)
		{
			case -1:
			perror("fork: ");
			break;
			case 0:
			{
				close(serverFd);	// child process doesn't need original socket descriptor fron parent
				const char *message = "you're connected to server";
				if(send(newfd, message, strlen(message), 0) == -1){
					perror("server: send ");
					close(newfd);
				}
			}
			break;
			default:
			clientConnected++;
			printf("number of current clients connected to server: %d \n", clientConnected);
		}
	}
}

int main(int argc, char **argv)
{
	int serverFd = 0;
	int opt = 0;
	char *n = nullptr;
	char *port = nullptr;



	while((opt = getopt(argc, argv, "n:p:")) != -1)
	{
		switch(opt)
		{
			case 'n':
			n = strdup(optarg);
			break;
			case 'p':
			port = strdup(optarg);
			break;
			case '?':
			printf("unknown option: %c \n", optopt);
			break;
		}
	}

	for(; optind < argc; ++optind){
		printf("extra arguments: %s \n", argv[optind]);
	}

	serverFd = create_tcp_socket(port);
	acceptConnections(serverFd);
	if(n) free(n);
	if(port) free(port);
	return 0;
}

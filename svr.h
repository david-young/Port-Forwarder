#ifndef __SVR_H__
#define __SVR_H__

#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include "binary-tree.h"
#include "helpers.h"

#define TRUE 		1
#define FALSE 		0
#define	RECORDTIME	10	/* sleep timer on thread that loops the server status */
#define EPOLL_QUEUE_LEN		256000
#define MAXPORT 		65535 /* remember that 0 is not a valid port */
#define MAXIP			255
#define DEFAULT_FILENAME	"servers.config"
#define MAX_RULES		100	/* max number of portforwarding rules */
#define BUFLEN			10240

/* Globals */
int i, y;
int svr_accept=0, svr_close=0, svr_error=0; /* record of accepted, completed clients as well as errors. */
int fd_i[EPOLL_QUEUE_LEN];
int	servers = 0;	/* number of stored servers. */
int fd_servers[MAX_RULES];
struct sockaddr_in 	forwardingrules[MAX_RULES];
struct sockaddr_in	forwardingrules_server[MAX_RULES];

/* Function prototypes */
void SystemFatal (const char* message);
int forwardsocket (int fd, BTree *tree);
int readconfigfile(char *filename);
int storeipport (char *ipport);
void close_fd (int);
void* looprecord(); /* uses the 3 integers below to print off efficiency */
int comp(void *p1, void *p2);

#endif


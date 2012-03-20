#include "binary-tree.h"

#define TRUE 		1
#define FALSE 		0
#define	RECORDTIME	10	/* sleep timer on thread that loops the server status */
#define EPOLL_QUEUE_LEN		256000
#define MAXPORT 		65535 /* remember that 0 is not a valid port */
#define MAXIP			255
#define DEFAULT_FILENAME	"servers.config"
#define MAX_RULES		100	/* max number of portforwarding rules */
#define BUFLEN			1024

/* Globals */
int 	i, y;
int 	svr_accept=0, svr_close=0, svr_error=0; /* record of accepted, completed clients as well as errors. */
int 	fd_i[EPOLL_QUEUE_LEN];
int	servers = 0;	/* number of stored servers. */
struct sockaddr_in 	forwardingrules[MAX_RULES];
int			fd_servers[MAX_RULES];
struct sockaddr_in	forwardingrules_server[MAX_RULES];

/* Function prototypes */
static void SystemFatal (const char* message);
static int forwardsocket (int fd, BTree *tree);
static int readconfigfile(char *filename);
static int storeipport (char *ipport);
void close_fd (int);
void* looprecord(); /* uses the 3 integers below to print off efficiency */
int comp(void *p1, void *p2);

typedef struct {
	int src; /* fd id */
	int dst;
} connection;


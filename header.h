#define TRUE 		1
#define FALSE 		0
#define	RECORDTIME	10	// sleep timer on thread that loops the server status
#define EPOLL_QUEUE_LEN		256000
#define MAXPORT 		65535 /* remember that 0 is not a valid port */
#define MAXIP			255
#define DEFAULT_FILENAME	"servers.config"
#define MAX_RULES		100	/* max number of portforwarding rules */

//Globals
int 	i, fd_server; //FIXME fd_server should be removed when code is changed
int 	svr_accept=0, svr_close=0, svr_error=0; // record of accepted, completed clients as well as errors.
int 	fd_i[EPOLL_QUEUE_LEN];
int	servers = 0;	//number of stored servers. 
struct sockaddr_in 	forwardingrules[MAX_RULES];
int			fd_servers[MAX_RULES];
char			forwardingrules_ip[MAX_RULES][15];//7 to 15 characters required for ip address. IPs aren't stored as network ips.

// Function prototypes
static void SystemFatal (const char* message);
static int forwardsocket (int fd);
static int readconfigfile(char *filename);
static int storeipport (char *ipport);
void close_fd (int);
void* looprecord(); // uses the 3 integers below to print off efficiency

typedef struct {
	int src; // fd id
	int dst;
} connection;


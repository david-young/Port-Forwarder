/*
	Daniel Smith
	A00587628
	COMP 8005
	Assignment 2
	February 18, 2012

	This program runs the select server. Clients will connect and
	send data to the server, in which the server will echo the data back.
	The client and server will constantly repeat this process until the
	iterations are up.

	This program was programmed to work on Linux and uses uses the GMP
	developer library. If you do not have it installed then type in the
	following command: "yum install gmp-devel". (Fedora)
	
	Compile the program by using the make file:
	"make"

	Coding Credit:

	The following code was written by Daniel Smith and Aman Abdulla.

	BUGS:
	Program will sometimes gets a Segmentation fault when a mass amount
	of clients connect at once. Just keep trying until it works. Select
	isn't that efficient handling that many clients anyways. - Daniel
*/


#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "header.h"

#define LISTENQ	256000
#define MAXLINE 4096

// Function Prototypes
static void SystemFatal(const char* );
void* looprecord(); // uses the 3 integers below to print off efficiency
static int ClearSocket ();

int numit; //numit is the number of iterations. Must match clients.
int 	svr_accept=0, svr_close=0, svr_error=0; // record of accepted, completed clients as well as errors.
int	buflen; //number of characters specified by user
int 	i, sockfd, fd_i[FD_SETSIZE], nready;

int main (int argc, char **argv)
{
	int maxi/*, bytes_to_read*/, arg;
	int listen_sd, new_sd, client_len, port, maxfd, client[FD_SETSIZE];
	struct sockaddr_in server, client_addr;
	//char *bp;
   	//ssize_t n;
   	fd_set rset, allset;
	pthread_t *record;

	switch(argc)
	{
		case 3:
			numit = atoi(argv[1]);
			buflen = atoi(argv[2]);// length of string sent back and forth per iteration
			port = SERVER_TCP_PORT;	// Use the default port
		break;
		case 4:
			numit = atoi(argv[1]);
			buflen = atoi(argv[2]);// length of string sent back and forth per iteration
			port = atoi(argv[3]);	// Get user specified port
		break;
		default:
			fprintf(stderr, "Usage: %s number_of_iterations length_of_string [port]\n", argv[0]);
			exit(1);
	}
	//char buf[buflen];

	// Create a stream socket
	if ((listen_sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		SystemFatal("Cannot Create Socket!");
	
	// set SO_REUSEADDR so port can be resused imemediately after exit, i.e., after CTRL-c
        arg = 1;
        if (setsockopt (listen_sd, SOL_SOCKET, SO_REUSEADDR, &arg, sizeof(arg)) == -1)
                SystemFatal("setsockopt");

	// Bind an address to the socket
	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY); // Accept connections from any client

	if (bind(listen_sd, (struct sockaddr *)&server, sizeof(server)) == -1)
		SystemFatal("bind error");
	
	// Listen for connections
	// queue up to LISTENQ connect requests
	listen(listen_sd, LISTENQ);
	pthread_create(&record, NULL, looprecord, NULL); //start thread which loops the server's connection status

	maxfd	= listen_sd;	// initialize
   	maxi	= -1;		// index into client[] array

	for (i = 0; i < FD_SETSIZE; i++)
           	client[i] = -1;             // -1 indicates available entry
 	FD_ZERO(&allset);
   	FD_SET(listen_sd, &allset);


	while (TRUE)
	{
   		rset = allset;               // structure assignment
		nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

      		if (FD_ISSET(listen_sd, &rset)) // new client connection
      		{
			client_len = sizeof(client_addr);
			if ((new_sd = accept(listen_sd, (struct sockaddr *) &client_addr, &client_len)) == -1)
			{
				++svr_error;
				SystemFatal("accept error");
			}
			++svr_accept;
//                      printf(" Remote Address:  %s\n", inet_ntoa(client_addr.sin_addr));//DEBUG

                        for (i = 0; i < FD_SETSIZE; i++)
			if (client[i] < 0)
            		{
				client[i] = new_sd;	// save descriptor
				break;
            		}
			if (i == FD_SETSIZE)
         		{
				++svr_error;
				//printf ("Too many clients\n"); /DEBUG spammy if using 50000+ clients
//            			exit(1); //FIXME should this be here?
    			}

			FD_SET (new_sd, &allset);     // add new descriptor to set
			if (new_sd > maxfd)
				maxfd = new_sd;	// for select

			if (i > maxi)
				maxi = i;	// new max index in client[] array

			if (--nready <= 0)
				continue;	// no more readable descriptors
     		 }

		for (i = 0; i <= maxi; i++)	// check all clients for data
     		{
			if ((sockfd = client[i]) < 0)
				continue;

			if (FD_ISSET(sockfd, &rset)) // has data
         		{

				if (ClearSocket()) 
				{
					// epoll will remove the fd from its set
					// automatically when the fd is closed
					++svr_close;
					close(sockfd);
					FD_CLR(sockfd, &allset);
               				client[i] = -1;
		    		}


/*
         			bp = buf;
				bytes_to_read = buflen;
				while ((n = read(sockfd, bp, bytes_to_read)) > 0)
				{

					bp += n;
					bytes_to_read -= n;
				}

				write(sockfd, buf, buflen);   // echo to client



				if (n == 0) // connection closed by client
            			{
					++svr_close;
//					printf(" Remote Address:  %s closed connection\n", inet_ntoa(client_addr.sin_addr)); //DEBUG
					close(sockfd);
					FD_CLR(sockfd, &allset);
               				client[i] = -1;
            			}

*/
       				if (--nready <= 0)
				{
            				break;        // no more readable descriptors
				}

			}
     		 }
   	}
	return(0);
}

static int ClearSocket ()
{
	int	n, bytes_to_read;
	char	*bp, buf[buflen];

	n = 0;
	bp = buf;
	bytes_to_read = buflen;
	while (n < buflen)
	{
		n = recv(sockfd, bp, bytes_to_read,0);
		if (n==0) //End of file. The remote has closed the connection.
		{
				
			fd_i[i] = 0;
			return 1; //close fd
		}
				
			bp += n;
			bytes_to_read -= n;
	}
				
	send(sockfd, buf, buflen,0);
	fd_i[i]++;
	if (fd_i[i] < numit)
		return 0;
	fd_i[i] = 0;
				
	return 1; //close fd
}

// Prints the error stored in errno and aborts the program.
static void SystemFatal(const char* message)
{
    perror (message);
    exit (EXIT_FAILURE);
}

void* looprecord()
{
	int svr_complete=0, j=0; // completed connections since last RECORDTIME
	char time[30]; //gettimeofday buffer
	struct timeval tv; // time value
	time_t curtime;

	while(TRUE)
	{
		j += svr_complete;
		svr_complete = svr_close - j;

		gettimeofday(&tv, NULL);
		curtime = tv.tv_sec;
		strftime(time,30,"%m-%d-%Y  %T.",localtime(&curtime));

		printf("%s%ld \tErrors: %i \tCompleted: %i \tActive: %i \tCompleted/Update(%is): %i\n", time,tv.tv_usec, svr_error, svr_close, svr_accept-svr_close, RECORDTIME, svr_complete);
		sleep(RECORDTIME);
	}
	return(0);
}

/*
	Daniel Smith
	A00587628
	David Young
	A00551119
	COMP 8005
	Final Project
	March 10, 2012

	This program runs the epoll port forwarding server. It will listen to
	clients that request certain ports. If the port is recognised then
	the data will be forwarded to the ip specified in the ip/port 
	config file.

	This program was programmed to work on Linux and uses uses the GMP
	developer library. If you do not have it installed then type in the
	following command (if you're using Fedora):
	$ yum install gmp-devel
	
	Compile the program by using the make file:
	$ make

	To run the program use this command:
	$ ./svr

	Coding Credit:

	The following code was written by Daniel Smith, David Young 
	and Aman Abdulla.
*/

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
#include "header.h"

int main (int argc, char* argv[]) 
{
	int arg; 
	int num_fds, fd_new, epoll_fd;
	static struct epoll_event events[EPOLL_QUEUE_LEN], event;
//	int port = SERVER_TCP_PORT;
	struct sockaddr_in remote_addr;
	socklen_t addr_size = sizeof(struct sockaddr_in);
	struct sigaction act;
	pthread_t record;
	char *filename;
	char ipbuf[20];

	switch(argc)
	{
		case 1:
			filename = DEFAULT_FILENAME;	// Use the default config filename
		break;
		case 2:
			filename = argv[1];	// Get user specified port
		break;
		default:
			fprintf(stderr, "Usage: %s [configfile \"default name is %s\"]\n", argv[0], DEFAULT_FILENAME);
			exit(1);
	}

	// store all the server ip and ports
	if (readconfigfile(filename))
		SystemFatal("failed to read configfile");

	if (servers<=0)
		SystemFatal("no forwarding rules");

	// set up the signal handler to close the server socket when CTRL-c is received
        act.sa_handler = close_fd;
        act.sa_flags = 0;
        if ((sigemptyset (&act.sa_mask) == -1 || sigaction (SIGINT, &act, NULL) == -1))
		SystemFatal("Failed to set SIGINT handler");
	//handle all ports supplied in the forwarding rules
	for(i = 0; i < servers; i++)
	{
		bzero(&fd_servers[i], sizeof(int));

	//FIXME change it for forwarding rules
		// Create the listening socket
		fd_servers[i] = socket (AF_INET, SOCK_STREAM, 0);
	    	if (fd_servers[i] == -1) 
			SystemFatal("socket");
    	
    		// set SO_REUSEADDR so port can be resused immediately after exit, i.e., after CTRL-c
	    	arg = 1;
	    	if (setsockopt (fd_servers[i], SOL_SOCKET, SO_REUSEADDR, &arg, sizeof(arg)) == -1) 
			SystemFatal("setsockopt");
    	
    		// Make the server listening socket non-blocking
	    	if (fcntl (fd_servers[i], F_SETFL, O_NONBLOCK | fcntl (fd_servers[i], F_GETFL, 0)) == -1) 
			SystemFatal("fcntl");
    	//FIXME requires fixing. forwarding rules are being set in storeipport()
    		// Bind to the specified listening port
//    	memset (&addr, 0, sizeof (struct sockaddr_in));//bzero took care of business
    	//addr.sin_family = AF_INET;
    	//addr.sin_addr.s_addr = htonl(INADDR_ANY);
    	//addr.sin_port = htons(port); //FIXME test when new port data handled
	    	if (bind (fd_servers[i], (struct sockaddr*) &forwardingrules[i], sizeof(forwardingrules[i])) == -1)
		{
			printf("%d %s %d\n", i, inet_ntop(AF_INET, &forwardingrules[i].sin_addr, ipbuf, 20), ntohs(forwardingrules[i].sin_port));
			SystemFatal("bind");
		}
    	
    		// Listen for fd_news; SOMAXCONN is 128 by default
	    	if (listen (fd_servers[i], SOMAXCONN) == -1) 
			SystemFatal("listen");

    		// Create the epoll file descriptor
	    	epoll_fd = epoll_create(EPOLL_QUEUE_LEN);
	    	if (epoll_fd == -1) 
			SystemFatal("epoll_create");
    	
    		// Add the server socket to the epoll event loop
	    	event.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLET;
	    	event.data.fd = fd_servers[i];
	    	if (epoll_ctl (epoll_fd, EPOLL_CTL_ADD, fd_servers[i], &event) == -1) 
			SystemFatal("epoll_ctl");
	}    	
	
	//start thread which loops the server's connection status
	pthread_create(&record, NULL, looprecord, NULL); 

	// Execute the epoll event loop
    	while (servers > 0) 
	{
		//struct epoll_event events[MAX_EVENTS];
		num_fds = epoll_wait (epoll_fd, events, EPOLL_QUEUE_LEN, -1);
		if (num_fds < 0) 
		{
			++svr_error;
			SystemFatal ("Error in epoll_wait!");
		}

		// FIXME ensure different ports can be used in here
		//	 it'll involve different server sockets
		for (i = 0; i < num_fds; i++) 
		{
	    		// Case 1: Error condition
	    		if (events[i].events & (EPOLLHUP | EPOLLERR)) 
			{
				++svr_error;
				fputs("epoll: EPOLLERR", stderr);
				close(events[i].data.fd);
				continue;
	    		}
	    		assert (events[i].events & EPOLLIN);

	    		// Case 2: Server is receiving a connection request
	    		if (events[i].data.fd == fd_server) 
			{
				//FIXME check if the port matches config file
				//socklen_t addr_size = sizeof(remote_addr);
				fd_new = accept (fd_server, (struct sockaddr*) &remote_addr, &addr_size);
				if (fd_new == -1) 
				{
		    			if (errno != EAGAIN && errno != EWOULDBLOCK) 
					{
						++svr_error;
						perror("accept");
		    			}
		    			continue;
				}

				// Make the fd_new non-blocking
				if (fcntl (fd_new, F_SETFL, O_NONBLOCK | fcntl(fd_new, F_GETFL, 0)) == -1) 
				{
					++svr_error;
					SystemFatal("fcntl");
				}
				
				// Add the new socket descriptor to the epoll loop
				event.data.fd = fd_new;
				++svr_accept;
				if (epoll_ctl (epoll_fd, EPOLL_CTL_ADD, fd_new, &event) == -1) 
				{
					++svr_error;
					SystemFatal ("epoll_ctl");
				}
				
//				printf(" Remote Address:  %s\n", inet_ntoa(remote_addr.sin_addr));//DEBUG
				continue;
	    		}

/*			FIXME this code deals with the data. Either
			forward it to server or client
*/			
/* 			FIXME how the if statement and forwardsocket
			is handled might need modification to match when
			the socket is closed. Socket should only be
			closed if server is offline or client/server
			send FIN.
*/
	    		// Case 3: One of the sockets has read data
			//check if fd is still connected
	    		if (forwardsocket(events[i].data.fd)) 
			{
				// epoll will remove the fd from its set
				// automatically when the fd is closed
				++svr_close;
				close (events[i].data.fd);
	    		}
		}
    	}
	close(fd_server);
	exit (EXIT_SUCCESS);
}

//FIXME write forwarding code. fd is the current file descriptor.
static int forwardsocket (int fd) 
{

return 0; //returning 0 = keep socket open. 1 = close socket.



/*	int	n, bytes_to_read;
	char	*bp, buf[buflen];

	n = 0;
	bp = buf;
	bytes_to_read = buflen;

	while (n < buflen)
	{
		n = recv (fd, bp, bytes_to_read, 0);
		if (n==0) //End of file. The remote has closed the connection.
		{
			fd_i[i] = 0;
			return 1; //close fd
		}

			bp += n;
			bytes_to_read -= n;
	}

	send (fd, buf, buflen, 0);
	fd_i[i]++;
	if (fd_i[i] < numit)
		return 0;
	fd_i[i] = 0;
	return 1; //close fd
*/
}

// Prints the error stored in errno and aborts the program.
static void SystemFatal(const char* message) 
{
    perror (message);
    exit (EXIT_FAILURE);
}

// close fd
void close_fd (int signo)
{
        close(fd_server);
	exit (EXIT_SUCCESS);
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
	return 0;
}

/*	Reads each line from the file specified and calls the storeipport
	function.
*/
int readconfigfile(char *filename)
{
	FILE *fp;
	int len; //length of characters in the current line being read
	char line[64];
	fp = fopen(filename, "r");
	if (fp == NULL)
		SystemFatal("configfile does not exist");
	while (fgets(line, sizeof line, fp) != NULL)
	{
		len = strlen(line);
		if( line[len-1] == '\n' )
			line[len-1] = 0;

		// attempt to store current line into forwarding rules
		if (storeipport(line))
			printf("<configfile:%s> incorrect syntax. \"192.168.0.1:80\"\n", line);
		else
			printf("%s successfully added to forwarding rules.\n", line);
	}
		// increment servers for every successful ipport added
	if (fclose(fp))
		SystemFatal("configfile failed to close");
	return 0;
}

/* 	verifies the config file's current line which supplies the ip and port in
	this format: 192.168.0.11:80. If the line is correct then store ip and
	port into list.
*/
int storeipport(char *ipport)
{
	unsigned ip1, ip2, ip3, ip4, port;
	int m;
	char fullip[15]; //7 to 15 characters required for ip address
	m = sscanf(ipport, "%3u.%3u.%3u.%3u:%u", &ip1, &ip2, &ip3, &ip4, &port);
	if (m != 5)  /* might be able to remove all these checks except for port checks because of inet_pton();*/
	{	
		printf("DEBUG_storeipport_1: %u.%u.%u.%u:%u <%s>", ip1, ip2, ip3, ip4, port, ipport); //DEBUG testing
		return 1;
	}
	
	if ((ip1 | ip2 | ip3 | ip4) > MAXIP || port > MAXPORT)
	{	
		printf("DEBUG_storeipport_2: %u.%u.%u.%u:%u <%s>", ip1, ip2, ip3, ip4, port, ipport); //DEBUG testing
		return 1;
	}

	if (strspn(ipport, "0123456789.:\n\r") < strlen(ipport))
	{	
		printf("DEBUG_storeipport_3: %u.%u.%u.%u:%u <%s>", ip1, ip2, ip3, ip4, port, ipport); //DEBUG testing
		return 1;
	}
	// UNTESTED
	// ip and port are good. Add data to list. 
	sprintf(forwardingrules_ip[servers], "%u.%u.%u.%u", ip1, ip2, ip3, ip4);
	bzero(&forwardingrules[servers], sizeof(struct sockaddr_in));
	forwardingrules[servers].sin_family = AF_INET;
	forwardingrules[servers].sin_port = htons(port);
	forwardingrules[servers].sin_addr.s_addr = htonl(INADDR_ANY);
//FIXME Might need this code for later
//	if (inet_pton(AF_INET, forwardingrules_ip[servers], &forwardingrules[servers].sin_addr) != 1)
//		return 1;

//DEBUG TESTING
//	printf("DEBUG_storeipport_4: <%u> <%s>\n", ntohs(forwardingrules[servers].sin_port), inet_ntop(AF_INET, &forwardingrules[servers].sin_addr, forwardingrules_ip[servers], sizeof(struct sockaddr_in)));//DEBUG testing
	servers++;

	return 0;
}


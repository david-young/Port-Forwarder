/*
	Daniel Smith
	A00587628
	COMP 8005
	Assignment 2
	February 18, 2012

	This program runs the multithreaded server. Clients will connect and
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

void* manageclient();
void* looprecord(); // uses the 3 integers below to print off efficiency

int 	svr_accept=0, svr_close=0, svr_error=0; // record of accepted, completed clients as well as errors.
int	new_sd, numit; //numit is the number of iterations. Must match clients.
struct	sockaddr_in client;
int	buflen; //number of characters specified by user

int main (int argc, char **argv)
{
//	int	n, bytes_to_read;
	int	sd, client_len, port;
	struct	sockaddr_in server;
	pthread_t *record;

//	char	*bp, buf[buflen];

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

	// Create a stream socket
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror ("Can't create a socket");
		exit(1);
	}

	// Bind an address to the socket
	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY); // Accept connections from any client

	if (bind(sd, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		perror("Can't bind name to socket");
		exit(1); 
	}

	// Listen for connections

	// queue up to 5 connect requests
	listen(sd, 5);
	pthread_create(&record, NULL, looprecord, NULL); //start thread which loops the server's connection status
	while (TRUE)
	{
		client_len= sizeof(client);
		if ((new_sd = accept (sd, (struct sockaddr *)&client, &client_len)) == -1)
		{
			fprintf(stderr, "Can't accept client\n");
			++svr_error;
//			exit(1); //FIXME should this be here?
		}
		else
		{

			char time[30]; //gettimeofday buffer
			struct timeval tv; // time value
			time_t curtime;

			gettimeofday(&tv, NULL);
			curtime = tv.tv_sec;
			strftime(time,30,"%m-%d-%Y  %T.",localtime(&curtime));
//			sprintf(returnedtime, "%s%ld",time,tv.tv_usec);
			//printf("%s", returnedtime);
			++svr_accept;
//			printf("%s%ld - Connected to %i - %s\n",time,tv.tv_usec, new_sd, inet_ntoa(client.sin_addr));// DEBUG this can get really spammy
			pthread_t *child;
			pthread_create(&child, NULL, manageclient, NULL);
		}
	}
	close(sd);
	return(0);
}

void* manageclient()
{
	int	new_sdx = new_sd;
	struct	sockaddr_in clientx = client;
	char	*bp, buf[buflen];
	int	n, bytes_to_read;
	int i = 0;

	
	for (; i<numit; i++)
	{
//		printf("Recieving large string from %s - ", inet_ntoa(clientx.sin_addr));
		bp = buf;
		bytes_to_read = buflen;
		while ((n = recv (new_sdx, bp, bytes_to_read, 0)) < buflen)
		{
			bp += n;
			bytes_to_read -= n;
		}
//		printf("%i - %s - %i\n", new_sdx, inet_ntoa(clientx.sin_addr), i+1);
//		printf("Recieving large string from %s - Sending large string - %i\n", inet_ntoa(clientx.sin_addr), i+1);
//		printf ("Sending large string - %i\n", i+1);
		send (new_sdx, buf, buflen, 0);
	}

	char time[30]; //gettimeofday buffer
	struct timeval tv; // time value
	time_t curtime;

	gettimeofday(&tv, NULL);
	curtime = tv.tv_sec;
	strftime(time,30,"%m-%d-%Y  %T.",localtime(&curtime));
	++svr_close;
//	printf("%s%ld - Closing %i - %s\n",time,tv.tv_usec, new_sdx, inet_ntoa(clientx.sin_addr));// DEBUG this can get really spammy
	close (new_sdx);
	return(0);
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


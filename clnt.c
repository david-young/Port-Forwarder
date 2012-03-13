/*
	Daniel Smith
	A00587628
	COMP 8005
	Assignment 2
	February 18, 2012

	This program connects to a server. Upon connection the client will
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

#include <time.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "header.h"

int numit; //numit is the number of iterations. Must match server.
int buflen; //number of characters specified by user


int main (int argc, char **argv)
{
	int i;
	int n, bytes_to_read;
	int sd, port;
	struct hostent	*hp;
	struct sockaddr_in server;
	char  *host, *bp;
//	msg_t 	msg, *mp;	//string sent to server.
//	char str[16];
//sbuf[BUFLEN], *sptr **pptr;
	switch(argc)
	{
		case 4:
			host =	argv[1];	// Host name
			numit = atoi(argv[2]);	// number of iterations
			buflen = atoi(argv[3]);// length of string sent back and forth per iteration
			port =	SERVER_TCP_PORT;
		break;
		case 5:
			host =	argv[1];
			numit = atoi(argv[2]);
			buflen = atoi(argv[3]);
			port =	atoi(argv[4]);	// User specified port
		break;
		default:
			fprintf(stderr, "Usage: %s host number_of_iterations length_of_string [port]\n", argv[0]);
			exit(1);
	}

	char data[buflen];
	char rbuf[buflen];
	// Create the socket
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Cannot create socket");
		exit(1);
	}
	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if ((hp = gethostbyname(host)) == NULL)
	{
		fprintf(stderr, "Unknown server address\n");
		exit(1);
	}
	bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);

	// Connecting to the server
	if (connect (sd, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		fprintf(stderr, "Can't connect to server\n");
		perror("connect");
		exit(1);
	}
	char time[30]; //gettimeofday buffer
	struct timeval tv; // time value
	time_t curtime;

	gettimeofday(&tv, NULL);
	curtime = tv.tv_sec;
	strftime(time,30,"%m-%d-%Y  %T.",localtime(&curtime));
	printf("%s%ld - Connected: Server Name: %s\n", time,tv.tv_usec, hp->h_name);//DEBUG can get spammy when running script
//	pptr = hp->h_addr_list;
	//printf("\t\tIP Address: %s\n", inet_ntop(hp->h_addrtype, *pptr, str, sizeof(str)));
	// data packet
	for (i = 0; i < buflen; i++)
	{
		data[i] = 'a';
	}
	data[i+1] = '\0';
//	printf("msg = \'a\'x%i\n", i);//DEBUG can get spammy when running script

	for (i = 0;i < numit; i++)
	{
	printf("Sending large string to %s - ", hp->h_name);//DEBUG very spammy
	//gets(sbuf); // get user's text
//	fgets (sbuf, BUFLEN, stdin);

	// Transmit data through the socket
//	send (sd, sbuf, BUFLEN, 0);
	// data packet

		send (sd, data, buflen, 0);
//	printf("%i.", i+1);
	printf("Receiving large string - %i\n", i+1); //DEBUG very spammy
		bp = rbuf;
		bytes_to_read = buflen;

	// client makes repeated calls to recv until no more data is expected to arrive.
		n = 0;
		while ((n = recv (sd, bp, bytes_to_read, 0)) < buflen)
		{
			bp += n;
			bytes_to_read -= n;
		}
	//printf ("%d\n%s\n", i, rbuf);
	}
//	char time[30]; //gettimeofday buffer
//	struct timeval tv; // time value
//	time_t curtime;

	gettimeofday(&tv, NULL);
	curtime = tv.tv_sec;
	strftime(time,30,"%m-%d-%Y  %T.",localtime(&curtime));
	printf("%s%ld - Closing connection with %s\n", time,tv.tv_usec, hp->h_name); //DEBUG can get spammy when running script
	fflush(stdout);
	close (sd);
	return (0);
}

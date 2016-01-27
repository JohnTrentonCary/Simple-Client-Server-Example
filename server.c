//**Included Libraries**
#include <stdlib.h>
#include <string.h>
//Header file contains declarations used in most I/O
#include <stdio.h>
//Contains definitions of a number of data types used in system calls
#include <sys/types.h>
//Includes a number of definitions of structures needed for sockets
#include <sys/socket.h>
//Contains constants and structures needed for internet domain addresses
#include <netinet/in.h>

//error is called when a system call fails.
//It displays a message about the error on stderr and then aborts the program
void error(char *msg){
	perror(msg);
	exit(1);
}

void dostuff (int sock)
{
	int n;
	char buffer[256];
	bzero(buffer,256);
	n = read(sock,buffer,255);
	if (n < 0) 
		error("ERROR reading from socket");

	printf("Here is the message: %s\n",buffer);
	n = write(sock,"I got your message",18);
	
	if (n < 0) 
		error("ERROR writing to socket");
}
int main(int argc, char *argv[]){
	//**Variable Declaration**

	//File descriptors, i.e. array subscripts into the file descripter table.
	int sockfd, newsockfd;
	//Stores the port number on which server accepts connections
	int portno;
	//Stores the size of the addess of the client.
	//This is needed for the accept system call.
	int clilen;
	//The return value for write() and read() calls.
	//It contains the number of characters written or read.
	int n;
	//Server reads characters from the socket connection into this buffer
	char buffer[256];
	//A structure containing an internet addess.
	//This structure is defined in <netinet.h>
	//Struct definition

	//		struct sockadd_in{
	//			short sin_family;
	//			u_short sin_port;
	//			struct in_addr sin_addr;
	//			char sin_zero[8];
	//		}

	//in_addr structure, defined in the same header file, 
	//contains only one field, a unsigned long called s_addr. 

	//Will contain the address of the server.
	struct sockaddr_in serv_addr;
	//Will contain the address of the client which connects to the server.
	struct sockaddr_in cli_addr;

	//**End of Variable Declaration**

	//Displays an error message if the user fails to pass in the port number
	// on which the server will accept connection as an arguement
	if(argc < 2){
		fprintf(stderr, "ERROR, no port provided\n");
		exit(1);
	}

	//Socket system call creates a new socket.
	//First agruement is the domain of the socket
	//	AF_INET: a constant used for the address domain for any two hosts on the internet
	//Second Arguement is the type of socket
	//	SOCK_STREAM: a constant for a stream socket in which characters are read in a continuous stream as if from a file or pipe.
	//Third arguement is the protocol.
	//Should almost always be 0
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0 )
		error("Error opening socket\n");

	//bzero sets all values in a buffer to zero.
	//Takes two arguements
	//	First: a pointer to the buffer
	//	Second: Size of the buffer
	bzero((char *) &serv_addr, sizeof(serv_addr));

	//Uses atoi to convert the value of arg[1] into an integer 
	// to be used a the port number which the server will listen for connections.
	portno = atoi(argv[1]);

	//sin_family should always be set to the symbolic constant AF_INET
	serv_addr.sin_family = AF_INET;

	//Must use htons() to convert a port number in host byte orer to a port number in network byte order
	serv_addr.sin_port = htons(portno);

	//Use INADDR_ANY to set the IP address of the machine on which the server is running
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	//bind() binds a socket to an address.
	//In this case the address of the current host and port number on which the server will run
	//Takes 3 Arguements
	//	First: The socket file descripter, the adress to which is bound and the size of the address to which it ibount
	// 	Second: a pointer to a structure of type sockaddr, but what is passed in is a structure of type aockaddr_in, so it must be cast correctly
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");

	//listen() allows the process to listen on the socket for connections.
	//First arguement is the socket file descriptor 
	//Second arguemtn is the size of the backlog queue
	//	which is the number of connection that can be waiting while the process is handling a particular connection
	listen(sockfd,5);

	//accept() causes the process to block until a client conects to the server.
	//Wakes up the process when a connection from a client has been successfully established.
	//Returns a new file descriptor and all communication on this connection should be done using the new file descriptor
	//Second arguement is a reference pointer to the address of the client on the other end
	//Third arguemtn is the size of this structure
	clilen = sizeof(cli_addr);

	printf("Would you like the server to accept multiple connection?(y/n)\n");
	char multiple;
	scanf("%c", &multiple);

	if(multiple == 'y'){
		while (1) {
			newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		
			if (newsockfd < 0)
				error("ERROR on accept");
		
			int pid = fork();
		
			if (pid < 0)
				error("ERROR on fork");
		
			if (pid == 0){
				close(sockfd);
				dostuff(newsockfd);
				exit(0);
			}
			else close(newsockfd);
		}
	}
	else {
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if(newsockfd < 0)
			error("ERROR on accept");

		//This code will only execute once a client has successfully connected to the server
		//Initializes the buffer.
		//Reads from the socket
		//read() will block until there is something for it to read in the socket
		//Will read either the total number of characters in the socket or 255, whichever is less and return the number
		bzero(buffer, 256);
		n = read(newsockfd, buffer, 255);
		if(n < 0)
			error("ERROR reading from socket");
		printf("Here is the message %s\n", buffer);

		//Writes to the client 
		//Last arguement of write is the size of the message
		n = write(newsockfd, "I got your message", 18);
		if(n < 0)
			error("ERROR writing to socket");
	}
	return 0;

}
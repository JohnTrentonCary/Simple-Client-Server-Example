//**Libraries start here**
//Refer to server.c 
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
//Defines hostent
#include <netdb.h>
//**Libraries End here

//Refer to server.c
void error(char *msg){
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[]){
	//Refer to server.c
	int sockfd, portno, n;
	//Contains the address of the server to which the client will connect 
	struct sockaddr_in serv_addr;
	//pointer to a struct of type hostent 
	//struct definition

	//struct hostent{
	//	char *h_name;						//Official ame of the host
	//	char **h_aliases;					//alias list
	//	int h_addrtype;						//host address type
	//	int h_length;						//length of address
	//	**h_addr_list;						//list of addresses from name server
	//	#define h_addr  h_addr_list[0];		//address, for backward compatiblity
	//}
	struct hostent *server;

	//refer to server.c
	char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    //argv[1] contains the name of the host on the internet
    server = gethostbyname(argv[1]);
    if(server == NULL){
    	fprintf(stderr, "ERROR no such host\n");
    	exit(0);
    }

    //Sets the fields in serv_addr
    //Because the ield server->h_addr is a character string we use bcopy
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);

    //connect() is called by the client to establish a connection with the server
    //3 Arguements
    //	First: socket file descriptor
    //	Second: The address of the host to which to connect to
    //	Third: Size of the address
    if(connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
    	error("ERROR connecting");

    //Prompts the user for a message 
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);

    //Writes said message to the socket
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,256);

    //Reads reply from the socket
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");

    //Prints out message from the socket
    printf("%s\n",buffer);
    return 0;

}
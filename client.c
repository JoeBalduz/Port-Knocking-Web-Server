#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <windows.h>
#include <time.h>  
#include "md5.h"

int getMyIP(unsigned char *myIP);

int main()
{

	WORD wVersionRequested = MAKEWORD(1,1);     // Stuff for WSA functions
	WSADATA wsaData;                            // Stuff for WSA functions


	int                  client_s;        		// Client socket descriptor
	struct sockaddr_in   server_addr;     		// Server Internet address
	int                  addr_len;        		// Internet address length
	char                 userInput[4096];   	// Output buffer for data
	int                  retcode;         		// Return code
	unsigned int		 port;			  		// Port number
	time_t				 seconds;		  		// Holds the time
	char				 minutesBuf[1000];  	// String to hold time in seconds
	char				 out_buf[4096];			// String to send to server
	char*				 concatString;	  		// Will be used to hold the password + timestamp
	struct				 MD5Context context;	// Required struct for md5
	unsigned char 		 checksum[16];			// Output from md5		
	int i;										// Iterator
	unsigned char myIP[5];						// Holds user's IP address. Size of 5 for 4 bytes + null character		
	char serverIP[1024];						// Holds the server's IP address
	
	
	printf("This is the client program.\n");
	
	printf("Enter in the server IP address: ");
	fgets(serverIP, sizeof(serverIP), stdin);
	serverIP[strlen(serverIP) - 1] = '\0';	//Getting rid of new line character

	/*This stuff initializes winsock*/
	WSAStartup(wVersionRequested, &wsaData);

	while(1)
	{		
		/*Create the socket*/
		client_s = socket(AF_INET, SOCK_DGRAM, 0);
		if (client_s < 0)
		{
			printf("*** ERROR - socket() failed \n");
			exit(-1);
		}
		
		memset(out_buf,0,strlen(out_buf));	//Clear output buffer
		
		printf("Enter in the port number: ");
		scanf("%u", &port);
		getchar();	//Removing newline character
		
		/*Fill-in servers socket's address information*/
		server_addr.sin_family = AF_INET;                 // Address family to use
		server_addr.sin_addr.s_addr = inet_addr(serverIP); // IP address to use
		server_addr.sin_port = htons(port);               // Port num to use
		
		
		printf("Enter in a message to send to the server: ");
		fgets(userInput, sizeof(userInput), stdin);
		userInput[strlen(userInput) - 1] = '\0';	//Getting rid of new line character
		
		if(getMyIP(myIP) < 0)
		{
			printf("*** ERROR - getMyIP() failed \n");
			exit(-1);
		}
		
		/*Gets the time in seconds, converts it to minutes, and converts it to a string*/
		seconds = time(NULL);
		seconds /= 60;	//Converts seconds to minutes
		sprintf(minutesBuf, "%lld", seconds);
		
		/*Adds the password, timestamp, and user's IP together*/
		concatString = (char*) malloc(strlen(userInput) + strlen(minutesBuf) + strlen(myIP) + 1);
		strcpy(concatString, userInput);
		strcat(concatString, minutesBuf);
		strcat(concatString, myIP);
		
		/*md5 hashing*/
		MD5Init (&context);
		MD5Update (&context, concatString, strlen (concatString));
		MD5Final (checksum, &context);
		
		/*Puts md5 output in the correct format*/
		for (i = 0; i < 16; i++)
		{
			sprintf (out_buf + strlen(out_buf), "%02x", (unsigned int) checksum[i]);
		}
		
		/*Send the message to the server*/
		retcode = sendto(client_s, out_buf, (strlen(out_buf) + 1), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
		if (retcode < 0)
		{
			printf("*** ERROR - sendto() failed \n");
			exit(-1);
		}
		
		/*Close the socket*/
		retcode = closesocket(client_s);
		if (retcode < 0)
		{
			printf("*** ERROR - closesocket() failed \n");
			exit(-1);
		}
		free(concatString);
	}

	/*Cleans-up winsock*/
	WSACleanup();

	return(0);
}

/*Gets the user's IP address in hex*/
int getMyIP(unsigned char *myIP)
{
    char buff[1024];

    if(gethostname(buff, sizeof(buff)) == SOCKET_ERROR)
    {
      return -1;
    }

    struct hostent *host = gethostbyname(buff);
    if(host == NULL)
    {
      return -1;
    }
	
	/*Puts the IP address in the correct format*/
	sprintf(myIP, "%02x%02x%02x%02x", 
		((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b1, 
		((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b2, 
		((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b3, 
		((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b4);

    return 0;
}
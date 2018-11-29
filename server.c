#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>
#include <stddef.h>     
#include <process.h>  
#include "md5.h"

#define PORT_NUM 8888
#define PASSWORD "password\0"

void web_server(void *blank);

HANDLE mutex;			
int timer = 0;	//Timer for thread
bool threadAlive = false;


int main()
{

	WORD wVersionRequested = MAKEWORD(1,1);  	// Stuff for WSA functions
	WSADATA wsaData;                            // Stuff for WSA functions
	
	int                  server_s;        		// Server socket descriptor
	struct sockaddr_in   server_addr;     		// Server Internet address
	struct sockaddr_in   client_addr;     		// Client Internet address
	struct in_addr       client_ip_addr;  		// Client IP address
	int                  addr_len;        		// Internet address length
	char                 in_buf[4096];    		// Input buffer for data
	int                  retcode;         		// Return code
	time_t				 seconds;		  		// Holds the time
	char				 minutesBuf[1000];		// String to hold time in seconds
	char 				 buf[4096];		  		// String to hold output from md5		
	char*				 concatString;	  		// Will be used to hold the password + timestamp
	unsigned int 		 thread_id;       		// Thread id assigned by Windows
	struct 				 MD5Context context;	// Required struct for md5
	unsigned char 		 checksum[16];			// Output from md5
	int i;								  		// Iterator
	char senderIP[5];							// Holds sender's IP address. Size of 5 for 4 bytes + null character
	
	
	printf("This is the server program.\n");
	
	/*This stuff initializes winsock*/
    WSAStartup(wVersionRequested, &wsaData);
	
	/*Creates the mutex*/
	mutex = CreateMutex(NULL, FALSE, NULL);
	if(mutex == NULL)
	{
		printf("CreateMutex error: %d\n", GetLastError());
		return 1;
	}
	
	/*Creates the socket*/
	server_s = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_s < 0)
    {
		printf("*** ERROR - socket() failed \n");
        exit(-1);
    }
	
	/*Fill in my socket's address information*/
	server_addr.sin_family = AF_INET;                 // Address family to use
    server_addr.sin_port = htons(PORT_NUM);           // Port number to use
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Listen on any IP address
    retcode = bind(server_s, (struct sockaddr *)&server_addr,
    sizeof(server_addr));
    if (retcode < 0)
    {
		printf("*** ERROR - bind() failed \n");
        exit(-1);
    }
	
	/*Receiving loop*/
	while(1)
	{
		/*Waiting to receive a message from the client*/
		printf(">>> Waiting for client. <<<\n");
		memset(buf,0,strlen(buf));	//Clear expected message buffer
		memset(senderIP, 0, strlen(senderIP));
		addr_len = sizeof(client_addr);
		retcode = recvfrom(server_s, in_buf, sizeof(in_buf), 0,
		(struct sockaddr *)&client_addr, &addr_len);
		if (retcode < 0)
		{
			printf("*** ERROR - recvfrom() failed \n");
			exit(-1);
		}

		/*Gets the time in seconds, converts it to minutes, and converts it to a string*/
		seconds = time(NULL);
		seconds /= 60;	//Converts seconds to minutes
		sprintf(minutesBuf, "%lld", seconds);
		
		/*Copy the four-byte client IP address into an IP address structure*/
		memcpy(&client_ip_addr, &client_addr.sin_addr.s_addr, 4);
		
		/*Put the sender's IP address in the correct format*/
		sprintf(senderIP, "%02x%02x%02x%02x", 
			client_ip_addr.S_un.S_un_b.s_b1,
			client_ip_addr.S_un.S_un_b.s_b2,
			client_ip_addr.S_un.S_un_b.s_b3,
			client_ip_addr.S_un.S_un_b.s_b4);
		
		/*Adds the password, timestamp, and sender's IP together*/
		concatString = (char*) malloc(strlen(PASSWORD) + strlen(minutesBuf) + strlen(senderIP) + 1);
		strcpy(concatString, PASSWORD);
		strcat(concatString, minutesBuf);
		strcat(concatString, senderIP);
		
		/*md5 hashing*/
		MD5Init (&context);
		MD5Update (&context, concatString, strlen (concatString));
		MD5Final (checksum, &context);	
		
		/*Output the received message*/
		printf("Received from client: %s \n", in_buf);
		
		/*Puts md5 output in the correct format*/
		for (i = 0; i < 16; i++)
		{
			sprintf (buf + strlen(buf), "%02x", (unsigned int) checksum[i]);
		}
		
		printf("Expected from client: %s\n", buf);
		
		/*Checks if the message is correct. If it is, the weblite process can be started*/
		if(strcmp(in_buf, buf) == 0)
		{
			if(!threadAlive)
			{
				/*Creates the thread that will create the webserver*/
				if ((thread_id = _beginthread(web_server,4096,(void *)0)) < 0)
				{
				  printf("Unable to create thread # %d, id = %u \n", i, thread_id);
				  exit(1);
				}
				printf("New thread created!\n");
			}
			else
			{
				WaitForSingleObject(mutex, INFINITE);	//Grab the lock
				timer = 10;
				ReleaseMutex(mutex); //Release the lock
				printf("Timer reset!\n");
			}
		}
	}
	
	/*Closes open socket*/
	retcode = closesocket(server_s);
	if (retcode < 0)
	{
		printf("*** ERROR - closesocket() failed \n");
		exit(-1);
	}
	
	/*Cleans up winsock*/
	WSACleanup();
	
	return 0;
}


void web_server(void *blank)
{
	STARTUPINFO si;								  //Stuff for CreateProcess
    PROCESS_INFORMATION pi;					      //Stuff for CreateProcess
	
	/*Initializes CreateProcess stuff*/
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
	
	threadAlive = true;
	timer = 10;

	/*Creates the weblite process*/
	if( !CreateProcess( NULL,   // No module name (use command line)
		"weblite",        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi )           // Pointer to PROCESS_INFORMATION structure
	) 
	{
		printf( "CreateProcess failed (%d).\n", GetLastError() );
		threadAlive = false;
		_endthread();
	}
	
	printf("Web server is now up.\n");
	
	/*Loop to keep the webserver alive*/
	while(1)
	{
		if(timer <= 0)
		{
			break;
		}
		else
		{
			WaitForSingleObject(mutex, INFINITE);	//Grab the lock
			timer--;
			ReleaseMutex(mutex);	//Release the lock
		}
		
		Sleep(1000);
	}
	
	
	/*Kills the weblite process*/
	if(!TerminateProcess(pi.hProcess, 0))
	{
		printf("Error :(\n");
	}

	/*CreateProcess ending stuff*/
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	
	printf("Web server is now down.\n");
	printf("Killing thread.\n");
	
	threadAlive = false;
	_endthread();
}




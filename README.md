# Port-Knocking-Web-Server

This port knocking web server is comprised of two parts. The client and the server. The client sends a "knock" to the server to start up 
a web server process. The web server will remain open for 10 seconds before being closed. If the client sends a "knock" to the server 
while the web server is still up, the timer will be reset to 10. This port knocking web server is made to be reasonably secure against 
playback attacks. The client and the server must be on the same network.

# How the Programs Work

This program works by having the client send a "knock" to the server to bring up a web server. This "knock" needs to be sent to the
correct port on the server. This port is a secret known only the client and the server. It is however possible that an attacker may 
sniff packets to see what port a client is sending messages to. Because of this, it is assumed that a possible attacker knows the 
correct port on the server to "knock" to bring up the web server. To combat this, we have a shared secret between the client and the 
server. The client must now send a message that contains that secret to the server. The web server will then only be brought up if the 
secret is received by the server. This secret can't just be sent over in plain text since an attacker could sniff the packet and see
exactly what the secret message is. Because of this, we use an MD5 hashing function. There was no specific reason I chose the MD5
hashing function other than it does what I want it to do. It turns plain text to a signature. Now an attacker won't be able to know
what the shared secret is. This is good, however I this method is vulnerable to playback attacks. It doesn't matter that the attacker
doesn't know the shared secret since the exact packet can just sniffed and sent to the server. I then chose to add a timestamp to this
shared secret. This timestamp is the taken in minutes in UNIX time. This was done so that messages expire after 1 minute. The message
sent to the server is now "shared_secret + timestamp". I originally chose the timestamp to be in seconds but I ran into some problems. 
When testing the client and the server on the same machine, the port knocking worked perfectly. However, once I moved on to using two 
different machines, I ran into a problem where the UNIX time on my two machines were off by a couple of seconds. This could be fixed by
syncing both of their clocks up, however I did not think doing it that way was a good idea. I switced to minutes since that timestamp
wouldn't have to be as accurate. This does however now leave some time for an attacker to playback a sent message. 

# Running the Program
This program is made to work for Windows. Right now, the client and the server must be run on two different machines that are on the 
same network. I plan on adding a change in the future that will allow the two programs to be run on the same machine. The programs can 
be compiled by bringing up command prompt for VS. The following commands will compile the programs.

For the server:

cl server.c weblite.c md5.c wsock32.lib

For the client:

cl client.c md5.c wsock32.lib

The server and the client can then be executed from their respective machines. Once the server is running, it will be listening for an
incoming message from port 8888. Once the client is running, it will ask you for the server's IP address. This can be found by using the
command ipconfig /all on the server machine. The program will then ask you for the port number. The port I have chosen is 8888. If you 
want the port to be different, change the define of PORT in server.c to what you want the port to be and recompile the server. You will
then be asked to send a message to the server. The message that must be sent is the string "password" without the quotation marks. This
will send the message to the server can cause the web server to be brought up. On the server side, you'll what the expected message is
to open the web server and what was actually received. If an incorrect message is sent, the web server will not be brought up. The 
client side of the program operates in a loop to allow you to continuously send messages to the server. It will not ask for the IP 
again, so if the wrong IP address was entered, the program will have to be closed and opened back up again.


# Port-Knocking-Web-Server

This port knocking web server is comprised of two parts. The client and the server. The client sends a "knock" to the server to start up a
web server process. The web server will remain open for 10 seconds before being closed. If the client sends a "knock" to the server while
the web server is still up, the timer will be reset to 10. This port knocking web server is made to be reasonably secure against playback
attacks. The client and the server must be on the same network.

# Running the Program
This program is made to work for Windows. Right now, the client and the server must be run on two different machines that are on the same
network. I plan on adding a change in the future that will allow the two programs to be run on the same machine. The programs can be
compiled by bringing up command prompt for VS. The following commands will compile the programs.

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
to open the web server and what was actually received. If an incorrect message is sent, the web server will not be brought up. The client
side of the program operates in a loop to allow you to continuously send messages to the server. It will not ask for the IP again, so if
the wrong IP address was entered, the program will have to be closed and opened back up again.


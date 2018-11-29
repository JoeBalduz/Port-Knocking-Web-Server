# Port-Knocking-Web-Server

This port knocking web server is comprised of two parts. The client and the server. The client sends a "knock" to the server to start up a
web server process. The web server will remain open for 10 seconds before being closed. If the client sends a "knock" to the server while
the web server is still up, the timer will be reset to 10.

# Running the Program
This program is made to work for Windows. It can be compiled by following the commands below.

For the server:

cl server.c weblite.c md5.c wsock32.lib

For the client:

cl client.c md5.c wsock32.lib

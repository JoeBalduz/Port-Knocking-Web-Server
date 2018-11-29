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
exactly what the secret message is. Because of this, we use an MD5 hashing function. I did not create this MD5 hashing function. 
Information about the creators and copyright information can be found in md5.c. There was no specific reason I chose the MD5
hashing function other than it does what I want it to do. It turns plain text to a signature. Now an attacker won't be able to know
what the shared secret is. This is good, however I this method is vulnerable to playback attacks. It doesn't matter that the attacker
doesn't know the shared secret since the exact packet can just sniffed and sent to the server. I then chose to add a timestamp to this
shared secret. This timestamp is the taken in minutes in UNIX time. This was done so that messages expire after 1 minute. The message
sent to the server is now "shared_secret + timestamp". I originally chose the timestamp to be in seconds but I ran into some problems. 
When testing the client and the server on the same machine, the port knocking worked perfectly. However, once I moved on to using two 
different machines, I ran into a problem where the UNIX time on my two machines were off by a couple of seconds. This could be fixed by
syncing both of their clocks up, however I did not think doing it that way was a good idea. I switced to minutes since that timestamp
wouldn't have to be as accurate. This does however now leave some time for an attacker to playback a sent message. I then thought that
it would be a good idea to also include the client's IP address in the message. I chose to add the IP address since this is something 
that the server could verify.The message sent to the server is now "shared_secret +  timestamp + ip_address". That message then gets put
into the MD5 hashing function before being sent to the server. This makes the web servermore secure against playback attacks, however an 
attacker could spoof the clients IP address.

I will now go into how the message is sent and verified by the client and the server. The client will create the message described above
and put it into the MD5 hasing function to generate a signature. This signature is then sent to the server. As soon as the server 
receives the message, it will get a timestamp of the time in minutes in UNIX time. It then attached that timestamp to the shared secret.
The server also then gets the client's IP address and also attaches that to string that now contains the shared secret and the time 
stamp. That string is then put into the same MD5 hashing function so that a signature can be generated. That signature is then compared
to the signature sent by the client. If they are the same, the web server is brought up. If they aren't the same, nothing happens.

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

I have included some test html files and a testable download file. The pages can be accessed on your browser once the port knock has 
been completed by going to [server_machine_ip_address]:8080/test_files/[file_name]. After 10 seconds have passed, the web server will
be shut down and you will have to do another port knock to access those files again.

The web server used in this program is called weblite. It was created by Dr. Ken Christensen, a professor at the University of South
Florida. More information about the program can be found in weblite.c. I created this program in a way that allows you to replace
the web server that is being used. All that you have to do to change the web server being used is to go into server.c and find the 
web_server function. Inside of that function there is a comment that says /*Creates the weblite process*/. You will see that underneath
that is the string weblite. Simply change that string to your desired web server and recompile the server program. Your chosen web 
server must be in the same directory as the server program.

# Known Bugs
There is a bug where the client will send the message to an incorrect IP address. I think that this has to do with the length of the IP
address entered in by the user. This shouldn't be too difficult to fix and I will fix it once I have time.

Authors: Vivek Reddy Kasireddy & Jahnavi Prudhivi
Assignment: Project 2 
Date: 2/16/24 

Below are the instructions on how to compile and run the code. 
To compile the entire code, use makefile: 
    make 

This will make both 
    gcc http_server.c -o http_server
    gcc http_client.c -o http_client
________________________________________________________________

To run the client: ./client [-p] server_url port_number
'-p': Optional flag to print the round-trip time (RTT).
'server_url': The URL or IP address of the server.
'port_number': The port number the server is listening on.
________________________________________________________________

To run the server: ./server <Server Port>
'-p': Optional flag to print the round-trip time (RTT).
'port_number': The port number the server will be listening to.
________________________________________________________________

Additional notes on how the content that the client recieves in stored: 

Using the commands ./client [-p] server_url port_number > output.txt it dumps 
all of the content that the client recieves into a text file which is easier to 
see all of the content as sometimes it can be cut off in the actual termnial where 
you run the client code.

The TA has checked over both the code for client and server to see if there are any 
major errors. I initially had problems with the content of www.google.com not 
completely sent to the client even though it was running but was able to fix this error. 
My other challenge was parsing the TMDG file correctly so the entire content is printed 
out correctly and that the client is able to know that the TMDG.html is not part of the 
server_url but rather the file itself since the linux server is there to specifically work 
for the retreiving the TMDG.html file to the client. The TAs have also said my RTT values 
look good and that the program is running without any errors. I have run the program both 
on my own local machine and the linux server and tested to see if both are working as they 
should. The server is specifically designed to retrieve the TMDG.html file, which is located 
on a Linux server. Ensure that the server URL provided includes only the server address and not 
the file path. The project has been thoroughly tested by both the author and the TA to ensure 
proper functionality. Any encountered issues, such as incomplete content retrieval or parsing 
errors, have been addressed.
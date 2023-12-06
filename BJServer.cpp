#include<iostream>
#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>     // gethostbyname
#include <unistd.h>    // read, write, close
#include <strings.h>     // bzero
#include <string.h>     // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <sys/uio.h>      // writev
#include <cstdlib>


using namespace std;

int USERS_CONNECTED = 0;

/**
 * Struct to pass arguments through to the readClient function which is called
 * by the thread thus is limited arguments.
 */
struct thread_args {
    int sd;
    int repetition;
};

/**
 * readClient(int sd)
 * 
 * readClient measures the time it takes for the server to read all messages
 * coming from the client. General flow of the function is as follows:
 *  1) Get current time - this is the start time
 *  2) Read incoming msg and track how many times read needed to be called
 *  3) Get current time - this is the end time
 *  4) Calculate elapsed time
 *  5) Sends message to client 
 *  6) Displays elapsed time
*/
void* readClient(void* voidArgs) {
    
    char databuf[1500];
    struct thread_args *args = (thread_args*) voidArgs;

    read(args->sd, databuf, 1500);
     cout << args->sd << endl;
    //Send count to client
    char temp[1500] = "welcome";
    strcpy(databuf, temp);
    write(args->sd, databuf, sizeof(databuf));
   
    while (1) {
        if (USERS_CONNECTED > 2) {
            databuf[0] = 'd';
            write(args->sd, databuf, sizeof(databuf));
        }
    }
    //close(args->sd);

    return;
}

/**
 * int main (int argc, char *argv[])
 * 
 * Main driver of the program. The function creates the socket and listens for
 * any connections.  Once a connection is made, a thread is made for that
 * connection so multiple connection can be ran simultaneouslly through
 * multi-threading.
 */
int main (int argc, char *argv[]) {

    // Create the socket
    int server_port = atoi(argv[1]); //Sets port to first arg
    int repetition = atoi(argv[2]); //Sets repetitions to second arg

    //acceptSock is a data structure that stores the server's address and port
    sockaddr_in acceptSock;
    bzero((char*) &acceptSock, sizeof(acceptSock));  // zero out the data structure
    acceptSock.sin_family = AF_INET;   // using IP
    acceptSock.sin_addr.s_addr = htonl(INADDR_ANY); // listen on any address this computer has
    acceptSock.sin_port = htons(server_port);  // set the port to listen on

    //serverSd is the actual socket itself
    int serverSd = socket(AF_INET, SOCK_STREAM, 0); // creates a new socket for IP using TCP

    const int on = 1;

    setsockopt(serverSd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(int));  // this lets us reuse the socket without waiting for hte OS to recycle it

    // Bind the socket

    bind(serverSd, (sockaddr*) &acceptSock, sizeof(acceptSock));  // bind the socket using the parameters we set earlier
    
    // Listen on the socket
    int n = 5;
    listen(serverSd, n);  // listen on the socket and allow up to n connections to wait.

    // Accept the connection as a new socket

    sockaddr_in newsock;   // place to store parameters for the new connection
    socklen_t newsockSize = sizeof(newsock);
 
    while (1) {
	    int newSd = accept(serverSd, (sockaddr *)&newsock, &newsockSize);  // grabs the new connection and assigns it a temporary socket
        
        //Creates thread and thread_arg struct
        pthread_t new_thread;
        struct thread_args *args = new thread_args;
        args->repetition = repetition;
        args->sd = newSd;

        USERS_CONNECTED++;
        pthread_create(&new_thread, NULL, readClient, (void*) args);

    }
    return 0;

}



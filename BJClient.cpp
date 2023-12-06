#include <iostream>
#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>     // gethostbyname
#include <unistd.h>    // read, write, close
#include <strings.h>     // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <sys/uio.h>      // writev

using namespace std;

int main(int argc, char *argv[]) {

    //Assigns CL args to vars
    int server_port = atoi(argv[1]);
    int bufsize = 1500;
    char * server_name = argv[2];

    //Server IP
    struct hostent* host = gethostbyname(server_name);

    //Creates socket address and zeroes out data structures
    sockaddr_in sendSockAddr;
    bzero((char*) &sendSockAddr, sizeof(sendSockAddr));

    //Assigns address and port
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(server_port);

    //Creates socket description
    int clientSd = socket(AF_INET, SOCK_STREAM, 0);

    //Connects socket
    int connectStatus = connect(clientSd, (sockaddr*)&sendSockAddr, sizeof(sendSockAddr));
    if (connectStatus < 0) {
        cerr << "Failed to connect to the server" << endl;
    }

    char databuf[1500] = "Hello";

    write( clientSd, databuf, bufsize ); 

    read(clientSd, databuf, sizeof(databuf));
    cout << databuf << endl;

    while (1) {
        read(clientSd, databuf, sizeof(databuf));
        if (databuf[0] == 'd') {
            cout << "DISCONNETING" << endl;
            break;
        }
    }

    close(clientSd);
}
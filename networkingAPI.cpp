#include "networkingAPI.h"
#include <iostream>
#include <string>
#include <unistd.h> 
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

void networkingAPI::sendMessage(int sd, string input){
    //Send length of string first
    size_t length = input.length();
    write(sd, &length, sizeof(length));

    //Send the actual string data
    write(sd, input.c_str(), length);
}

string networkingAPI::receiveMessage(int sd){
    size_t length;
    read(sd, &length, sizeof(length));
    char databuf[length + 1];
    read(sd, databuf, length);
    databuf[length] = '\0';
    string message(databuf);
    return message;
}

void networkingAPI::sendHitOrStand(int sd, string input){
    write(sd, input.c_str(), input.length());
}

string networkingAPI::receiveHitOrStand(int sd){
    string message = "";
    char databuf[2];
    read(sd, &databuf, 2);
    databuf[1] = '\0';
    message += databuf;
    cout << "Message received: " + message << endl;
    //cout << "Char being read: " + message[0] << endl;
    if (message[0] == 'h'){
        return "Hit";
    } else {
        return "Stand";
    }
}

int networkingAPI::connectToServer(int server_port, char* server_name){
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
    } else {
        cout << "CONNECTED TO SERVER" << endl;
    }

    return clientSd;
}
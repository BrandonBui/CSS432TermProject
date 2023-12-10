#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include    <netdb.h>     // gethostbyname
#include    <unistd.h>    // read, write, close
#include   <strings.h>     // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <sys/uio.h>      // writev
#include <iostream>
#include <string>
#include "networkingAPI.h"

using namespace std;

void playBlackJack(int clientSd);

int main(int argc, char *argv[]){
    
    //initialize server address and file name
    string server_ip = argv[1];
    int server_port = atoi(argv[2]);



    //Create socket attributes
    struct hostent* host = gethostbyname(server_ip.c_str());
    sockaddr_in sendSockAddr;
    bzero((char*)&sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(server_port);

    //Create socket and connect to server
    int clientSd = socket(AF_INET, SOCK_STREAM, 0);
    int connectStatus = connect(clientSd, (sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
    if (connectStatus < 0){
        cerr << "Failed to connect to the server" << endl;
        return 0;
    }

    cout << "Sucessfully connected." << endl;

    //Create data buffer
    char databuf[5000];

    
    playBlackJack(clientSd);
    

    close(clientSd);

    return 0;
}

void playBlackJack(int clientSd){
    bool donePlaying = false;
    string message = "";
    string input = "";
    message += networkingAPI::receiveMessage(clientSd);
    cout << message << endl;

    networkingAPI::sendMessage(clientSd, "buffer"); //this is a buffer message

    message = "";
    message += networkingAPI::receiveMessage(clientSd);
    cout << message << endl;

    networkingAPI::sendMessage(clientSd, "buffer"); //this is a buffer message

    while (!donePlaying){
        message = "";
        message += networkingAPI::receiveMessage(clientSd);
        //cout << "Message was: ";
        //cout << message << endl;
        if (message == "You Busted"){
            donePlaying = true;
        } else {
            cout << "Hit or Stand? Type 'h' for Hit or 's' for Stand" << endl;
            bool properInput = false;
            while (!properInput){
                cin >> input;
                if (input == "h"){
                    properInput = true;
                } else if (input == "s"){
                    properInput = true;
                    donePlaying = true;
                } else {
                    cout << "Please input 'h' for Hit or 's' for Stand" << endl;
                }
            }
            networkingAPI::sendHitOrStand(clientSd, input);

            if (input == "h"){
                message = "";
                message += networkingAPI::receiveMessage(clientSd);
                cout << message << endl;
            }
            
        }
    }

    message = "";
    message += networkingAPI::receiveMessage(clientSd);  //(this is to see if you won or lost)

    cout << message << endl;

}
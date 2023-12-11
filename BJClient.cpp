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
#include <string>
#include "networkingAPI.h"

using namespace std;

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

int main(int argc, char *argv[]) {

    //Assigns CL args to vars
    int server_port = atoi(argv[1]);
    char * server_name = argv[2];
    char * lobbyAndName = argv[3];

    int clientSd = networkingAPI::connectToServer(server_port, server_name);
    //write(clientSd, lobbyAndName, sizeof(lobbyAndName));
    networkingAPI::sendMessage(clientSd, lobbyAndName);

    while (1) {
        string response;
        response = networkingAPI::receiveMessage(clientSd);
        cout << response << endl;

        //Check if message requires response
        if (response.substr(0, 7) == "CURRENT") {
            response = "";
            cin >> response;
            networkingAPI::sendMessage(clientSd, response);
        }

        if (response == "STARTING GAME") {
            break;
        }
        
    }

    playBlackJack(clientSd);



    close(clientSd);
}
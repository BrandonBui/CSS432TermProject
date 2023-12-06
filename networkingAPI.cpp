#include "networkingAPI.h"
#include <iostream>
#include <string>
#include <unistd.h> 

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
    read(sd, &databuf, 1000);
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
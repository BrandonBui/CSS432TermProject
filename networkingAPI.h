#ifndef NETWORKINGAPI_H_
#define NETWORKINGAPI_H_
#include <string>

using namespace std;

class networkingAPI{
    public:
        static void sendMessage(int sd, string input);
        static string receiveMessage(int sd);
        static void sendHitOrStand(int sd, string input);
        static string receiveHitOrStand(int sd);
        static int connectToServer(int server_port, char* server_name);
    private:

};
#endif
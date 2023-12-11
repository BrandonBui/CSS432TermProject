#ifndef LOBBY_H_
#define LOBBY_H_

#include <iostream>
#include <vector>
#include <map>
#include <string>

using namespace std;

class LobbyManager
{
    public:
        //Initialize a lobby manager with lobbyCountInput lobbies already initialized.
        LobbyManager(int lobbyCountInput, vector<map<int, string>*> &lobbyList);
        //~LobbyManager();

        //Trigger: TCP socket receives a connection and is sent a lobby number
        //If the lobby number is valid, add the connection sd and username pair to the lobby.
        void AddPlayer(int lobbyNum, int sd, string username, vector<map<int, string>*> &lobbyList);
        //Trigger: TCP socket receives a disconnect message from an established lobbied player.
        //Removes the player's sd from the given lobby
        void RemovePlayer(int lobbyNum, int sd, vector<map<int, string>*> &lobbyList);

        //Trigger: UDP socket receives a message to create a lobby
        //Creates a new lobby (int/string map pointer) and returns the new lobby number. Send this to the UDP requester.
        int CreateLobby(vector<map<int, string>*> &lobbyList);
        //Trigger: TCP socket receives a Ready message from an lobbied player
        //Sets status of that connection's sd to Ready if not already done. Need to notify other players in the same lobby. 
        void ReadyPlayer(int sd);

        //Trigger: TCP socket receives a Unready message from an lobbied player
        //Sets status of that connection's sd to Not Ready if not already done. Need to notify other players in the same lobby.
        void UnreadyPlayer(int sd, vector<map<int, string>*> &lobbyList);

        //Given a player's sd, respond with the lobby num that the player belongs to
        int GetLobbyNum(int sd, vector<map<int, string>*> &lobbyList);

        //Trigger: UDP socket receives a request to display lobby information
        //Responds with lobbies, player names, and ready status 
        string PrintLobbyInfo(vector<map<int, string>*> &lobbyList);

        bool allPlayersReady(int lobbyNum, vector<map<int, string>*> &lobbyList);

    private:
        int lobbyCount;
        //vector<map<int, string>*> lobbies;
        map<int, bool> isReady;
};


#endif


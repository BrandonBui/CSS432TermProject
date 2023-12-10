#include "lobby.h"

LobbyManager::LobbyManager(int lobbyCountInput)
{
    lobbyCount = lobbyCountInput;
    for (int i = 0; i < lobbyCount; i++)
    {
        lobbies.push_back(new map<int, string>);
    }

    //Can remove - only here for testing purposes
    cout << "INITIALIZED!" << endl;
}
LobbyManager::~LobbyManager()
{
    for (int i = 0; i < lobbyCount; i++)
    {
        delete(lobbies[i]);
    }
}

//
void LobbyManager::AddPlayer(int lobbyNum, int sd, string username)
{
    if (lobbyNum <= lobbyCount - 1)
    {
        lobbies[lobbyNum]->insert(pair<int, string>(sd, username));
        isReady.insert(pair<int, bool>(sd, false));
    }
}

void LobbyManager::RemovePlayer(int lobbyNum, int sd)
{
    lobbies[lobbyNum] -> erase(sd);
}

int LobbyManager::CreateLobby()
{
    lobbies.push_back(new map<int, string>);
    lobbyCount++;
    return lobbyCount - 1;
}

void LobbyManager::ReadyPlayer(int sd)
{
    isReady[sd] = true;
}

int LobbyManager::GetLobbyNum(int sd)
{
    int output;
    for (int i = 0; i <= lobbyCount - 1; i++)
    {
        if (lobbies[i]->find(sd) != lobbies[i]->end()) output = i;
    }
    return output;
}

string LobbyManager::PrintLobbyInfo()
{
    string output = "";

    output += "LOBBIES: \n";
    for (int i = 0; i < lobbyCount; i++)
    {
        output += "Lobby " + to_string(i) + ": \n";
        for (auto k = lobbies[i]->begin(); k != lobbies[i]->end(); k++)
        {
            output += "     Player " + k->second + " - ";
            if (isReady[k->first]) output += " Ready";
            else output += " Not Ready";
            output += "\n";
        }
    }
    return output;
}
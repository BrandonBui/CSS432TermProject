#include "lobby.h"

LobbyManager::LobbyManager(int lobbyCountInput, vector<map<int, string>*> &lobbies)
{
    lobbyCount = lobbyCountInput;
    for (int i = 0; i < lobbyCount; i++)
    {
        lobbies.push_back(new map<int, string>);
    }

    //Can remove - only here for testing purposes
    cout << "INITIALIZED!" << endl;
}
// LobbyManager::~LobbyManager()
// {
//     for (int i = 0; i < lobbyCount; i++)
//     {
//         delete(lobbies[i]);
//     }
// }

//
void LobbyManager::AddPlayer(int lobbyNum, int sd, string username, vector<map<int, string>*> &lobbies)
{
    if (lobbyNum <= lobbyCount - 1)
    {
        lobbies[lobbyNum]->insert(pair<int, string>(sd, username));
        isReady.insert(pair<int, bool>(sd, false));
    }
}

void LobbyManager::RemovePlayer(int lobbyNum, int sd, vector<map<int, string>*> &lobbies)
{
    lobbies[lobbyNum] -> erase(sd);
}

int LobbyManager::CreateLobby(vector<map<int, string>*> &lobbies)
{
    lobbies.push_back(new map<int, string>);
    lobbyCount++;

    return lobbyCount - 1;
}

void LobbyManager::ReadyPlayer(int sd)
{
    isReady[sd] = true;
}

int LobbyManager::GetLobbyNum(int sd, vector<map<int, string>*> &lobbies)
{
    int output;
    for (int i = 0; i <= lobbyCount - 1; i++)
    {
        if (lobbies[i]->find(sd) != lobbies[i]->end()) output = i;
    }

    return output;
}

string LobbyManager::PrintLobbyInfo(vector<map<int, string>*> &lobbies)
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

bool LobbyManager::allPlayersReady(int lobbyNum, vector<map<int, string>*> &lobbies)
{
    for (auto i = lobbies[lobbyNum]->begin(); i != lobbies[lobbyNum]->end(); i++)
    {
        if (!isReady[i->first]) return false;
    }
;
    return true;
}
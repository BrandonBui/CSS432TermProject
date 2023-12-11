#include <iostream>
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
#include <vector>
#include <algorithm>
#include <array>
#include <ctime>
#include <random>
#include <cassert>
#include "lobby.h"
#include "networkingAPI.h"

using namespace std;

struct Player {
    int SD;
    string username;
    bool inLobby;
};

struct Lobby {
    int LobbyID;
    bool lobbyFull;
    bool inGame;
    vector<Player*> players;
};

vector<Player*> ALL_PLAYERS = {};
vector<Lobby*> LOBBIES = {};

enum class CardRank{
    rank_2,
    rank_3,
    rank_4,
    rank_5,
    rank_6,
    rank_7,
    rank_8,
    rank_9,
    rank_10,
    rank_jack,
    rank_queen,
    rank_king,
    rank_ace,

    max_ranks
};

enum class CardSuit{
    club,
    diamond,
    heart,
    spade,

    max_suits
};

struct Card{
    CardRank rank{};
    CardSuit suit{};
};

using Deck = array<Card, 52>;
using Index = Deck::size_type;

Deck createDeck()
{
    Deck deck{};

    // We could initialize each card individually, but that would be a pain.  Let's use a loop.

    Index index{ 0 };

    for (int suit{ 0 }; suit < static_cast<int>(CardSuit::max_suits); ++suit)
    {
        for (int rank{ 0 }; rank < static_cast<int>(CardRank::max_ranks); ++rank)
        {
            deck[index].suit = static_cast<CardSuit>(suit);
            deck[index].rank = static_cast<CardRank>(rank);
            ++index;
        }
    }

    return deck;
}

void shuffleDeck(Deck& deck)
{
    // mt is static so it only gets seeded once.
    static mt19937 mt{ static_cast<mt19937::result_type>(time(nullptr)) };

    shuffle(deck.begin(), deck.end(), mt);
}

int getCardValue(const Card& card){
    switch(card.rank){
        case CardRank::rank_2:      return 2;
        case CardRank::rank_3:      return 3;
        case CardRank::rank_4:      return 4;
        case CardRank::rank_5:      return 5;
        case CardRank::rank_6:      return 6;
        case CardRank::rank_7:      return 7;
        case CardRank::rank_8:      return 8;
        case CardRank::rank_9:      return 9;
        case CardRank::rank_10:
        case CardRank::rank_jack:
        case CardRank::rank_queen:
        case CardRank::rank_king:   return 10;
        case CardRank::rank_ace:    return 11;

        default:
            assert(false && "Should never happen");
    }
}

void playBlackjack(const Deck& deck, vector<int>& playerList)
{
    cout << "Now playing Blackjack!" << endl;
    // Index of the card that will be drawn next. This cannot overrun
    // the array, because a player will lose before all cards are used up.
    vector<int> playerValues(playerList.size());
    int currentPlayer = 0;
    int nextCardIndex = 0;
    int dealerValue = 0;
    string message = "";
    while (currentPlayer < playerList.size()){
        // Create the dealer and give them 1 card.
        dealerValue = getCardValue(deck.at(nextCardIndex++));

        // The dealer's card is face up, the player can see it.
        cout << "The dealer is showing: " << dealerValue << '\n';
        message += "The dealer is showing: " + to_string(dealerValue);
        networkingAPI::sendMessage(playerList[currentPlayer], message);

        // Create the player and give them 2 cards.
        playerValues[currentPlayer] = getCardValue(deck.at(nextCardIndex)) + getCardValue(deck.at(nextCardIndex + 1));
        nextCardIndex += 2;

        networkingAPI::receiveMessage(playerList[currentPlayer]);//buffer receive

        cout << "You have: " << playerValues[currentPlayer] << '\n';
        message = "";
        message += "You have: " + to_string(playerValues[currentPlayer]);
        networkingAPI::sendMessage(playerList[currentPlayer], message);

        networkingAPI::receiveMessage(playerList[currentPlayer]);//buffer receive

        bool donePlaying = false;

        while (!donePlaying){
            if (playerValues[currentPlayer] > 21){
            // This can happen even before the player had a choice if they drew 2
            // aces.
                networkingAPI::sendMessage(playerList[currentPlayer], "You Busted");
                donePlaying = true;
            } else {
                networkingAPI::sendMessage(playerList[currentPlayer], "Hit or Stand?");
                message = "";
                message += networkingAPI::receiveHitOrStand(playerList[currentPlayer]);
                if (message == "Hit"){
                    int cardValue = getCardValue(deck.at(nextCardIndex));
                    playerValues[currentPlayer] += cardValue;
                    cout << "You were dealt a " + to_string(cardValue) + " and now have " + to_string(playerValues[currentPlayer]) << endl;
                    nextCardIndex++;

                    message = "";
                    message += "You were dealt a " + to_string(cardValue) + " and now have " + to_string(playerValues[currentPlayer]);
                    networkingAPI::sendMessage(playerList[currentPlayer], message);
                } else {// The Player stands
                    donePlaying = true;
                }
            }
        }

        currentPlayer++;

    }

    while (dealerValue < 17){
        int cardValue = getCardValue(deck.at(nextCardIndex++));
        dealerValue += cardValue;
        cout << "The dealer turned up a " << cardValue << " and now has " << dealerValue << '\n';
    }

    message = "";
    for (int i = 0; i < playerList.size(); i++){
        //Case 1: Dealer busted and you busted
        if (dealerValue > 21 && playerValues[i] > 21){
            message = "The dealer busted...but you busted too so did you really win? (I don't think so)";
        }
        else if (dealerValue > 21 && playerValues[i] < 21){//Case 2: Dealer busted and you didn't bust
            message = "The dealer busted. You win!";
        }
        else if (dealerValue > 21 && playerValues[i] == 21){//Case 3: Dealer busted and you got blackjack
            message = "The dealer busted and you got blackjack, nice one!";
        }
        else if (dealerValue < 21 && playerValues[i] > 21){//Case 4: Dealer didn't bust and you busted
            message = "You busted. GG go next.";
        }
        else if (dealerValue > playerValues[i]){//Case 5: Neither busted and dealer won
            message = "The dealer had a score of " + to_string(dealerValue) + " and you have " + to_string(playerValues[i]) + ". You lost.";
        }
        else if (dealerValue < playerValues[i]){//Case 6: Neither busted and you won
            message = "The dealer had a score of " + to_string(dealerValue) + " and you have " + to_string(playerValues[i]) + ". You won!";
        }
        else if (dealerValue == playerValues[i]){//Case 7: Neither busted and you tied with the dealer
            message = "Both you and the dealer had a score of " + to_string(dealerValue) + ". Good job I guess?";
        }
        
        networkingAPI::sendMessage(playerList[i], message);
    }
   

    
}

struct game_thread_args {
    vector<struct Player*> gamePlayers;
};

void* gameThread(void* threadArgs) {
    struct game_thread_args *args = (game_thread_args*) threadArgs;
    
    vector<int> playerSD;
    //Loop through all players in game and add their SD to the vector
    for (struct Player* player : args->gamePlayers) {
        playerSD.push_back(player->SD);
        networkingAPI::sendMessage(player->SD, "STARTING GAME");
    }

    Deck d = createDeck();
    shuffleDeck(d);
    shuffleDeck(d);

    playBlackjack(d, playerSD);

    return NULL;
}

void* tcpGameManager(void*) {

    while (1) {
        //Go through each player and see if they want to join a lobby or create a new lobby
        for (struct Player* player : ALL_PLAYERS) {
            if (!player->inLobby) {
                string msg = "CURRENT LOBBIES\n---------------\n";
                for (struct Lobby* lobby : LOBBIES) {
                    msg += "Lobby ID: " + to_string(lobby->LobbyID) + "\n";
                    if (lobby->lobbyFull) {
                        msg += "Lobby Full? YES\n";
                    } else {
                        msg += "Lobby Full? NO\n";
                    }
                    msg += "Players: ";
                    for (struct Player* player : lobby->players) {
                        msg += player->username + ", ";
                    }
                    msg = msg.substr(0, msg.length() - 1) + "\n";
                }
                msg += "\nRESPOND WITH LOBBY ID TO JOIN OR C TO CREATE A NEW LOBBY";
                networkingAPI::sendMessage(player->SD, msg);

                string response = networkingAPI::receiveMessage(player->SD);
                //If player wants to create a new lobby
                if (response == "C") {
                    //Create new lobby
                    struct Lobby* newLobby = new Lobby;
                    newLobby->LobbyID = LOBBIES.size();
                    newLobby->lobbyFull = false;
                    newLobby->inGame = false;
                    newLobby->players.push_back(player); //add them to newly created lobby
                    //Update player to be in lobby
                    player->inLobby = true;

                    LOBBIES.push_back(newLobby);
                    msg = "Created and added to lobby " + to_string(newLobby->LobbyID);
                    networkingAPI::sendMessage(player->SD, msg);
                //If the user responded with a number
                } else {
                    //Get the lobby with matching ID
                    struct Lobby* lobby = LOBBIES[stoi(response)];
                    //Add player to lobby
                    lobby->players.push_back(player);
                    //Check if lobby full
                    if (lobby->players.size() == 2) {
                        lobby->lobbyFull = true;
                    }
                    player->inLobby = true;

                    msg = "Joined lobby " + to_string(lobby->LobbyID);
                    networkingAPI::sendMessage(player->SD, msg);
                }
            }
        }

        //Go through each lobby 
        for (struct Lobby* lobby : LOBBIES) {
            if (lobby->lobbyFull && !lobby->inGame) {
                //Start game
                pthread_t game_thread;
                struct game_thread_args *gameArgs = new game_thread_args;
                vector<struct Player*> players = lobby->players;
                gameArgs->gamePlayers = players;
                pthread_create(&game_thread, NULL, gameThread, (void*) gameArgs);

                lobby->inGame = true;
            }
        }
    }

    // struct tcp_thread_args *args = (tcp_thread_args*) threadArgs;

    // while (1) {
    //     //Check if any lobbies have two players, if there is then start the game       
    //     cout << args->lobbies[0]->size() << endl;
    //     for (int i = 0; i < args->lobbies.size(); i++) {
            
    //         if (args->lobbies[i]->size() == 2) {
    //             //create thread
    //             pthread_t game_thread;
    //             struct game_thread_args *gameArgs = new game_thread_args;

    //             gameArgs->lobbies = args->lobbies;
    //             gameArgs->lobbyID = i;

    //             pthread_create(&game_thread, NULL, gameThread, (void*) gameArgs);
    //         }
    //     }
    // }
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
 
    //vector<int> playerList;

    //vector<map<int, string>*> lobbies;

    //Initializes the lobby manager and creates 1 lobby
    //LobbyManager lobbyMgr(1, lobbies);
   
    //Create struct to store arguments needed inside udp thread
    // pthread_t udp_thread;
    // struct udp_thread_args *udpArgs = new udp_thread_args;
    // //Store a reference to the lobby manager initialized above
    // udpArgs->lobbyMgr = &lobbyMgr;
    // //Creates thread to manage any udp messages
    // pthread_create(&udp_thread, NULL, udpMessageManager, (void*) udpArgs);

    
    pthread_t tcp_thread;
    pthread_create(&tcp_thread, NULL, tcpGameManager, NULL);

    while (1) {
	    //Listens for incoming player connections
        //An incoming TCP connection means that a player is intending to join a lobby
        //The player is expected to know which lobby they are joining
        int newSd = accept(serverSd, (sockaddr *)&newsock, &newsockSize);  // grabs the new connection and assigns it a temporary socket
        
        char buff[1024]; //initialize it to 1024 bytes

        //Read player's hello message which should consist of their desired lobby number and username
        //Message will be formatted lobbyNum#username
        read(newSd, buff, sizeof(buff));
        
        //Convert the char[] to a string
        string username(buff);
        cout << username << endl;
        //Make pair w/ SD as one item, and their username
        struct Player* newPlayer = new Player;
        newPlayer->SD = newSd;
        newPlayer->username = username;
        newPlayer->inLobby = false;

        ALL_PLAYERS.push_back(newPlayer);
    }
    return 0;

}



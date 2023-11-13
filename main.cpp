#include <algorithm> // std::shuffle
#include <array>
#include <ctime>
#include <iostream>
#include <random>
#include <cassert>


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

struct Player
{
    int score{};
};

using Deck = std::array<Card, 52>;
using Index = Deck::size_type;

// Maximum score before losing.
constexpr int g_maximumScore{ 21 };

// Minimum score that the dealer has to have.
constexpr int g_minimumDealerScore{ 17 };

void printCard(const Card& card){
    switch (card.rank){
        case CardRank::rank_2:      std::cout << '2';   break;
        case CardRank::rank_3:      std::cout << '3';   break;
        case CardRank::rank_4:      std::cout << '4';   break;
        case CardRank::rank_5:      std::cout << '5';   break;
        case CardRank::rank_6:      std::cout << '6';   break;
        case CardRank::rank_7:      std::cout << '7';   break;
        case CardRank::rank_8:      std::cout << '8';   break;
        case CardRank::rank_9:      std::cout << '9';   break;
        case CardRank::rank_10:     std::cout << 'T';   break;
        case CardRank::rank_jack:   std::cout << 'J';   break;
        case CardRank::rank_queen:  std::cout << 'Q';   break;
        case CardRank::rank_king:   std::cout << 'K';   break;
        case CardRank::rank_ace:    std::cout << 'A';   break;
        default:
            std::cout << '?';
            break;
    }

    switch (card.suit){
        case CardSuit::club:       std::cout << 'C';   break;
        case CardSuit::diamond:    std::cout << 'D';   break;
        case CardSuit::heart:      std::cout << 'H';   break;
        case CardSuit::spade:      std::cout << 'S';   break;
        default:
            std::cout << '?';
            break;
    }
}

// We'll need these many more times, create an aliases.
using Deck = std::array<Card, 52>;
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

void printDeck(const Deck deck){
    for(const auto& card: deck){
        printCard(card);
        std::cout << ' ';
    }
}

void shuffleDeck(Deck& deck)
{
    // mt is static so it only gets seeded once.
    static std::mt19937 mt{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };

    std::shuffle(deck.begin(), deck.end(), mt);
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

bool playerWantsHit()
{
    while (true)
    {
        std::cout << "(h) to hit, or (s) to stand: ";

        char ch{};
        std::cin >> ch;

        switch (ch)
        {
            case 'h':
                return true;
            case 's':
                return false;
        }
    }
}

// Returns true if the player went bust. False otherwise.
bool playerTurn(const Deck& deck, Index& nextCardIndex, Player& player)
{
    while (true)
    {
        if (player.score > g_maximumScore)
        {
            // This can happen even before the player had a choice if they drew 2
            // aces.
            std::cout << "You busted!\n";
            return true;
        }
        else
        {
            if (playerWantsHit())
            {
                int cardValue{ getCardValue(deck.at(nextCardIndex++)) };
                player.score += cardValue;
                std::cout << "You were dealt a " << cardValue << " and now have " << player.score << '\n';
            }
            else
            {
                // The player didn't go bust.
                return false;
            }
        }
    }
}

// Returns true if the dealer went bust. False otherwise.
bool dealerTurn(const Deck& deck, Index& nextCardIndex, Player& dealer)
{
    // Draw cards until we reach the minimum value.
    while (dealer.score < g_minimumDealerScore)
    {
        int cardValue{ getCardValue(deck.at(nextCardIndex++)) };
        dealer.score += cardValue;
        std::cout << "The dealer turned up a " << cardValue << " and now has " << dealer.score << '\n';

    }

    // If the dealer's score is too high, they went bust.
    if (dealer.score > g_maximumScore)
    {
        std::cout << "The dealer busted!\n";
        return true;
    }

    return false;
}

bool playBlackjack(const Deck& deck)
{
    // Index of the card that will be drawn next. This cannot overrun
    // the array, because a player will lose before all cards are used up.
    Index nextCardIndex{ 0 };

    // Create the dealer and give them 1 card.
    Player dealer{ getCardValue(deck.at(nextCardIndex++)) };

    // The dealer's card is face up, the player can see it.
    std::cout << "The dealer is showing: " << dealer.score << '\n';

    // Create the player and give them 2 cards.
    Player player{ getCardValue(deck.at(nextCardIndex)) + getCardValue(deck.at(nextCardIndex + 1)) };
    nextCardIndex += 2;

    std::cout << "You have: " << player.score << '\n';

    if (playerTurn(deck, nextCardIndex, player))
    {
        // The player went bust.
        return false;
    }

    if (dealerTurn(deck, nextCardIndex, dealer))
    {
        // The dealer went bust, the player wins.
        return true;
    }

    return (player.score > dealer.score);
}

int main()
{
    auto deck{ createDeck() };

    shuffleDeck(deck);

    shuffleDeck(deck);

    if (playBlackjack(deck))
    {
        std::cout << "You win!\n";
    }
    else
    {
        std::cout << "You lose!\n";
    }

    return 0;
}

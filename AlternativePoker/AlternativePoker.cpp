#include <iostream>
#include <ctime>

const unsigned cardTypeStringSize = 4;
const unsigned suitStringSize = 2;
const unsigned cardTypesSize = 8;
const unsigned suitsSize = 4;
const unsigned deckSize = cardTypesSize * suitsSize;
const unsigned CHIP_VALUE = 10;
const char cardTypes[][cardTypeStringSize] = { "7", "8", "9", "10", "J", "Q", "K", "A" };
const char suits[][suitStringSize] = { "C", "D", "H", "S" };

struct Card
{
    char cardType[cardTypeStringSize];
    char suit[suitStringSize];
};
struct Player
{
    Card cards[cardTypeStringSize];
    int money = 1000;
    int currentBet = 0;
    bool isActive = true;
    int totalContribution = 0;
};

Card deck[deckSize];

unsigned playerCount = 0;
const unsigned cardsForEachPlayer = 3;

unsigned long seed = 0;

unsigned long lcg(unsigned long& seed) 
{
    seed = (1664525 * seed + 1013904223) % 4294967296;
    return seed;
}

int strLen(const char str[])
{
    int len = 0;
    while (*str)
    {
        len++;
        str++;
    }
    return len;
}
void copystr(const char* src, char* dest)
{
    while (*src)
    {
        *dest = *src;
        src++;
        dest++;
    }
    *dest = '\0';
}
void inflateDeck()
{
    unsigned counter = 0;
    for (int i = 0; i < cardTypesSize; i++) {
        for (int j = 0; j < suitsSize; j++) {
            copystr(cardTypes[i], deck[counter].cardType);
            copystr(suits[j], deck[counter++].suit);
        }
    }
}
void shuffleDeck()
{
    // Fisher-Yates algorithm
    for (int i = deckSize - 1; i >= 1; i--)
    {
        unsigned int j = lcg(seed) % (i + 1); // Generate a random index between 0 and i
        // Swap deck[i] and deck[j]
        Card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}
void dealCards(Player* &players)
{
    unsigned cardIndex = 0;
    for (int i = 0; i < playerCount; i++)
    {
        for (int j = 0; j < cardsForEachPlayer; j++)
        {
            players[i].cards[j] = deck[cardIndex];
            cardIndex++;
        }
    }
}

void releaseMemory(Player* &players)
{
    delete[] players;
}

void printPlayersCards(Player* players)
{
    for (int i = 0; i < playerCount; i++)
    {
        std::cout << "Player " << i + 1 << "'s cards:\n";
        for (int j = 0; j < cardsForEachPlayer; j++)
        {
            std::cout << players[i].cards[j].cardType;
            std::cout << players[i].cards[j].suit << ' ';
        }
        std::cout << "\n";
    }
}

int main() {
    seed = std::time(0);

    std::cout << "How many players are playing(2-9)?\n";
    std::cin >> playerCount;

    inflateDeck();
    shuffleDeck();

    Player* players = new Player[playerCount];

    dealCards(players);
    printPlayersCards(players);

    releaseMemory(players);

    return 0;
}
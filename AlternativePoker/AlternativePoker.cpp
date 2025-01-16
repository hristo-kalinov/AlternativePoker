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
    bool isActive = true;
    bool hasActed = false;
    int totalContribution = 0;
};

Card deck[deckSize];

unsigned playerCount = 0;
const unsigned cardsForEachPlayer = 3;

unsigned long seed = std::time(0);

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

void releaseMemory(Player* &players)
{
    delete[] players;
}

bool allPlayersHaveActed(Player* players)
{
    for (int i = 0; i < playerCount; i++)
    {
        if (players[i].hasActed == false)
            return false;
    }
    return true;
}

bool onePlayerActive(Player* players) 
{
    unsigned activePlayerCount = 0;
    for (int i = 0; i < playerCount; i++)
    {
        if (players[i].isActive)
        {
            activePlayerCount++;
        }
    }
    if (activePlayerCount == 1)
        return true;
    return false;
}
int main() {
    std::cout << "How many players are playing(2-9)?\n";
    std::cin >> playerCount;
    inflateDeck();
    shuffleDeck();

    Player* players = new Player[playerCount];

    dealCards(players);
    printPlayersCards(players);

    unsigned pot = 0;
    int currentBet = 0;
    bool isBettingComplete = false;
    for (int i = 0; i < playerCount; i++)
    {
        players[i].totalContribution += CHIP_VALUE;
        players[i].isActive = true;
        players[i].money -= CHIP_VALUE;
        pot += CHIP_VALUE;
        currentBet += CHIP_VALUE;
    }

    unsigned lastPlayerToRaise = 0;
    while (!isBettingComplete)
    {
        for (int i = 0; i < playerCount; i++)
        {
            if (!players[i].isActive || players[i].hasActed)
                continue;
            std::cout << "\nPlayer " << i + 1 << ", it's your turn.\n";
            std::cout << "Pot: " << pot << "\n";
            std::cout << "Current bet: " << currentBet << "\n";
            std::cout << "Your total contribution: " << players[i].totalContribution << "\n";
            std::cout << "Your money: " << players[i].money << "\n";
            std::cout << "Your cards: ";
            for (int j = 0; j < cardsForEachPlayer; j++) {
                std::cout << players[i].cards[j].cardType << players[i].cards[j].suit << " ";
            }
            std::cout << "\n";

            std::cout << "Choose an action: (c) Call, (r) Raise, (f) Fold\n";
            char action;
            std::cin >> action;

            while (action != 'c' && action != 'r' && action != 'f') {
                std::cout << "Invalid action. Please choose (c) Call, (r) Raise, or (f) Fold: ";
                std::cin >> action;
            }

            if (action == 'c')
            {
                int amountToCall = currentBet - players[i].totalContribution;
                players[i].money -= amountToCall;
                pot += amountToCall;
                players[i].hasActed = true;
            }
            if (action == 'r')
            {
                unsigned amountToRaise = 0;
                std::cout << "Amount to Raise: ";
                std::cin >> amountToRaise;

                while (amountToRaise > players[i].money || amountToRaise <= 0) {
                    std::cout << "Invalid amount. You can raise up to " << players[i].money << ": ";
                    std::cin >> amountToRaise;
                }

                currentBet += amountToRaise;
                pot += amountToRaise;

                players[i].totalContribution += amountToRaise;
                players[i].money -= amountToRaise;
                isBettingComplete = false;
                for (int j = 0; j < playerCount; j++) {
                    if (players[j].isActive && !players[j].hasActed) {
                        players[j].hasActed = false;
                    }
                }
                players[i].hasActed = true;
            }
            if (action == 'f')
            {
                players[i].isActive = false;
                players[i].hasActed = true;
            }
            if (allPlayersHaveActed(players))
            {
                isBettingComplete = true;
                break;
            }
            if (onePlayerActive(players))
            {
                isBettingComplete = true;
                break;
            }

        }
    }



    releaseMemory(players);

    return 0;
}
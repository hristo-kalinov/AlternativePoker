#include <iostream>
#include <ctime>

const unsigned cardTypeStringSize = 4;
const unsigned suitStringSize = 2;
const unsigned cardTypesSize = 8;
const unsigned cardsForEachPlayer = 3;
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
    Card cards[cardsForEachPlayer];
    int money = 1000;
    bool isActive = true;
    bool hasActed = false;
    int totalContribution = 0;
};

Card deck[deckSize];
unsigned playerCount = 0;
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
    for (int i = 0; i < cardTypesSize; i++)
    {
        for (int j = 0; j < suitsSize; j++)
        {
            copystr(cardTypes[i], deck[counter].cardType);
            copystr(suits[j], deck[counter++].suit);
        }
    }
}

void shuffleDeck()
{
    for (int i = deckSize - 1; i >= 1; i--)
    {
        unsigned int j = lcg(seed) % (i + 1);
        Card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

void dealCards(Player*& players)
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
            std::cout << players[i].cards[j].cardType << players[i].cards[j].suit << ' ';
        }
        std::cout << "\n";
    }
}

void releaseMemory(Player*& players)
{
    delete[] players;
}

bool allPlayersHaveActed(Player* players)
{
    for (int i = 0; i < playerCount; i++)
    {
        if (players[i].hasActed == false) return false;
    }
    return true;
}

bool onePlayerActive(Player* players)
{
    unsigned activePlayerCount = 0;
    for (int i = 0; i < playerCount; i++)
    {
        if (players[i].isActive) activePlayerCount++;
    }
    return activePlayerCount == 1;
}

void bettingLoop(Player* players, unsigned& pot)
{
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

    while (!isBettingComplete)
    {
        for (int i = 0; i < playerCount; i++)
        {
            if (!players[i].isActive || players[i].hasActed) continue;
            std::cout << "\nPlayer " << i + 1 << ", it's your turn.\n";
            std::cout << "Pot: " << pot << "\n";
            std::cout << "Current bet: " << currentBet << "\n";
            std::cout << "Your total contribution: " << players[i].totalContribution << "\n";
            std::cout << "Your money: " << players[i].money << "\n";
            std::cout << "Your cards: ";
            for (int j = 0; j < cardsForEachPlayer; j++)
            {
                std::cout << players[i].cards[j].cardType << players[i].cards[j].suit << " ";
            }
            std::cout << "\n";

            std::cout << "Choose an action: (c) Call, (r) Raise, (f) Fold\n";
            char action;
            std::cin >> action;

            while (action != 'c' && action != 'r' && action != 'f')
            {
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

                while (amountToRaise > players[i].money || amountToRaise <= 0)
                {
                    std::cout << "Invalid amount. You can raise up to " << players[i].money << ": ";
                    std::cin >> amountToRaise;
                }

                currentBet += amountToRaise;
                pot += amountToRaise;
                players[i].totalContribution += amountToRaise;
                players[i].money -= amountToRaise;
                isBettingComplete = false;
                for (int j = 0; j < playerCount; j++)
                {
                    if (players[j].isActive && !players[j].hasActed) players[j].hasActed = false;
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
}

bool stringcmp(const char* str1, const char* str2)
{
    while (*str1 && *str2)
    {
        if (*str1 != *str2) return false;
        str1++;
        str2++;
    }
    return (*str1 == '\0' && *str2 == '\0');
}

unsigned getCardScore(char* cardType)
{
    if (stringcmp(cardType, "7")) return 7;
    if (stringcmp(cardType, "8")) return 8;
    if (stringcmp(cardType, "9")) return 9;
    if (stringcmp(cardType, "10") || stringcmp(cardType, "J") || stringcmp(cardType, "Q") || stringcmp(cardType, "K")) return 10;
    if (stringcmp(cardType, "A")) return 11;
    return 0;
}

unsigned getHandScore(Card* cards)
{
    if (stringcmp(cards[0].cardType, cards[1].cardType) && stringcmp(cards[1].cardType, cards[2].cardType))
    {
        if (stringcmp(cards[0].cardType, "7")) return 34;
        return getCardScore(cards[0].cardType) * 3;
    }

    bool containsSevenOfClubs = false;
    unsigned sevenOfClubsPos = 0;
    unsigned sevenOfClubsValue = 11;
    for (int i = 0; i < cardsForEachPlayer; i++)
    {
        if (stringcmp(cards[i].cardType, "7") && stringcmp(cards[i].suit, "C"))
        {
            containsSevenOfClubs = true;
            sevenOfClubsPos = i;
        }
    }

    if (containsSevenOfClubs)
    {
        Card card1 = (sevenOfClubsPos == 0) ? cards[1] : cards[0];
        Card card2 = (sevenOfClubsPos == 1) ? cards[2] : cards[1];
        if (sevenOfClubsPos == 0)
        {
            card1 = cards[1];
            card2 = cards[2];
        }
        else if (sevenOfClubsPos == 1)
        {
            card1 = cards[0];
            card2 = cards[2];
        }
        else if (sevenOfClubsPos == 2)
        {
            card1 = cards[0];
            card2 = cards[1];
        }
        int card1Score = getCardScore(card1.cardType);
        int card2Score = getCardScore(card2.cardType);

        if (stringcmp(card1.cardType, card2.cardType)) return card1Score * 2 + sevenOfClubsValue;
        if (stringcmp(card1.suit, card2.suit)) return card1Score + card2Score + sevenOfClubsValue;

        if (card1Score > card2Score) return card1Score + sevenOfClubsValue;
        else return card2Score + sevenOfClubsValue;
    }

    if (stringcmp(cards[0].cardType, cards[1].cardType) && stringcmp(cards[1].cardType, cards[2].cardType))
    {
        return getCardScore(cards[0].cardType) * 3;
    }

    if (stringcmp(cards[0].suit, cards[1].suit) && stringcmp(cards[1].suit, cards[2].suit))
    {
        return getCardScore(cards[0].cardType) + getCardScore(cards[1].cardType) + getCardScore(cards[2].cardType);
    }
    if (stringcmp(cards[0].suit, cards[1].suit))
    {
        return getCardScore(cards[0].cardType) + getCardScore(cards[1].cardType);
    }
    if (stringcmp(cards[0].suit, cards[2].suit))
    {
        return getCardScore(cards[0].cardType) + getCardScore(cards[2].cardType);
    }
    if (stringcmp(cards[1].suit, cards[2].suit))
    {
        return getCardScore(cards[1].cardType) + getCardScore(cards[2].cardType);
    }

    unsigned sevensCount = 0;
    unsigned acesCount = 0;

    for (int i = 0; i < cardsForEachPlayer; i++)
    {
        if (stringcmp(cards[i].cardType, "A")) acesCount++;
        if (stringcmp(cards[i].cardType, "7")) sevensCount++;
    }

    if (acesCount >= 2) return 22;
    if (acesCount >= 2) return 23;

    else
    {
        unsigned highestScore = 0;
        for (int i = 0; i < cardsForEachPlayer; i++)
        {
            if (getCardScore(cards[i].cardType) > highestScore) highestScore = getCardScore(cards[i].cardType);
        }
        return highestScore;
    }
}

void stringcpy(char* dest, const char* src)
{
    while (*src)
    {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
}

void testHandScore()
{
    Card cards[3];

    // Test case 1: (9 бройки)
    stringcpy(cards[0].cardType, "7"); stringcpy(cards[0].suit, "H");
    stringcpy(cards[1].cardType, "9"); stringcpy(cards[1].suit, "D");
    stringcpy(cards[2].cardType, "9"); stringcpy(cards[2].suit, "C");
    std::cout << "Test 1: Expected 9, Got " << getHandScore(cards) << "\n";

    // Test case 2: (11 бройки)
    stringcpy(cards[0].cardType, "10"); stringcpy(cards[0].suit, "S");
    stringcpy(cards[1].cardType, "10"); stringcpy(cards[1].suit, "D");
    stringcpy(cards[2].cardType, "A"); stringcpy(cards[2].suit, "C");
    std::cout << "Test 2: Expected 11, Got " << getHandScore(cards) << "\n";

    // Test case 3: (10 бройки)
    stringcpy(cards[0].cardType, "J"); stringcpy(cards[0].suit, "S");
    stringcpy(cards[1].cardType, "Q"); stringcpy(cards[1].suit, "H");
    stringcpy(cards[2].cardType, "10"); stringcpy(cards[2].suit, "C");
    std::cout << "Test 3: Expected 10, Got " << getHandScore(cards) << "\n";

    // Test case 4: (19 бройки)
    stringcpy(cards[0].cardType, "K"); stringcpy(cards[0].suit, "H");
    stringcpy(cards[1].cardType, "9"); stringcpy(cards[1].suit, "H");
    stringcpy(cards[2].cardType, "Q"); stringcpy(cards[2].suit, "C");
    std::cout << "Test 4: Expected 19, Got " << getHandScore(cards) << "\n";

    // Test case 5: (22 бройки)
    stringcpy(cards[0].cardType, "A"); stringcpy(cards[0].suit, "S");
    stringcpy(cards[1].cardType, "A"); stringcpy(cards[1].suit, "H");
    stringcpy(cards[2].cardType, "10"); stringcpy(cards[2].suit, "C");
    std::cout << "Test 5: Expected 22, Got " << getHandScore(cards) << "\n";

    // Test case 6: (21 бройки)
    stringcpy(cards[0].cardType, "A"); stringcpy(cards[0].suit, "H");
    stringcpy(cards[1].cardType, "K"); stringcpy(cards[1].suit, "H");
    stringcpy(cards[2].cardType, "10"); stringcpy(cards[2].suit, "C");
    std::cout << "Test 6: Expected 21, Got " << getHandScore(cards) << "\n";

    // Test case 7: (22 бройки)
    stringcpy(cards[0].cardType, "8"); stringcpy(cards[0].suit, "S");
    stringcpy(cards[1].cardType, "A"); stringcpy(cards[1].suit, "D");
    stringcpy(cards[2].cardType, "7"); stringcpy(cards[2].suit, "C");
    std::cout << "Test 7: Expected 22, Got " << getHandScore(cards) << "\n";

    // Test case 8: (29 бройки)
    stringcpy(cards[0].cardType, "10"); stringcpy(cards[0].suit, "H");
    stringcpy(cards[1].cardType, "9"); stringcpy(cards[1].suit, "H");
    stringcpy(cards[2].cardType, "J"); stringcpy(cards[2].suit, "H");
    std::cout << "Test 8: Expected 29, Got " << getHandScore(cards) << "\n";

    // Test case 9: (30 бройки)
    stringcpy(cards[0].cardType, "Q"); stringcpy(cards[0].suit, "C");
    stringcpy(cards[1].cardType, "Q"); stringcpy(cards[1].suit, "H");
    stringcpy(cards[2].cardType, "Q"); stringcpy(cards[2].suit, "D");
    std::cout << "Test 9: Expected 30, Got " << getHandScore(cards) << "\n";

    // Test case 10: (31 бройки)
    stringcpy(cards[0].cardType, "7"); stringcpy(cards[0].suit, "C");
    stringcpy(cards[1].cardType, "K"); stringcpy(cards[1].suit, "H");
    stringcpy(cards[2].cardType, "K"); stringcpy(cards[2].suit, "D");
    std::cout << "Test 10: Expected 31, Got " << getHandScore(cards) << "\n";

    // Test case 11: (21 бройки)
    stringcpy(cards[0].cardType, "7"); stringcpy(cards[0].suit, "C");
    stringcpy(cards[1].cardType, "K"); stringcpy(cards[1].suit, "H");
    stringcpy(cards[2].cardType, "Q"); stringcpy(cards[2].suit, "D");
    std::cout << "Test 11: Expected 21, Got " << getHandScore(cards) << "\n";

    // Test case 12: (32 бройки)
    stringcpy(cards[0].cardType, "7"); stringcpy(cards[0].suit, "C");
    stringcpy(cards[1].cardType, "A"); stringcpy(cards[1].suit, "H");
    stringcpy(cards[2].cardType, "K"); stringcpy(cards[2].suit, "H");
    std::cout << "Test 12: Expected 32, Got " << getHandScore(cards) << "\n";

    // Test case 13: (34 бройки)
    stringcpy(cards[0].cardType, "7"); stringcpy(cards[0].suit, "C");
    stringcpy(cards[1].cardType, "7"); stringcpy(cards[1].suit, "H");
    stringcpy(cards[2].cardType, "7"); stringcpy(cards[2].suit, "D");
    std::cout << "Test 13: Expected 34, Got " << getHandScore(cards) << "\n";

    // Test case 14: (33 бройки)
    stringcpy(cards[0].cardType, "A"); stringcpy(cards[0].suit, "C");
    stringcpy(cards[1].cardType, "A"); stringcpy(cards[1].suit, "H");
    stringcpy(cards[2].cardType, "A"); stringcpy(cards[2].suit, "D");
    std::cout << "Test 14: Expected 33, Got " << getHandScore(cards) << "\n";
}

int main()
{
    testHandScore();

    std::cout << "How many players are playing(2-9)?\n";
    while (true)
    {
        std::cin >> playerCount;
        if (playerCount >= 2 && playerCount <= 9) break;
        std::cerr << "Invalid number of players. Please enter a number between 2 and 9.\n";
    }

    inflateDeck();
    shuffleDeck();

    Player* players = new Player[playerCount];
    dealCards(players);
    printPlayersCards(players);

    unsigned pot = 0;
    bettingLoop(players, pot);
    releaseMemory(players);

    return 0;
}
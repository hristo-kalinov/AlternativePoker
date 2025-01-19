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
    unsigned long long multiplier = 1664525;
    unsigned long long increment = 1013904223;
    unsigned long long modulus = 4294967296;

    seed = (multiplier * seed + increment) % modulus;
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
    if (stringcmp(cardType, "10") || stringcmp(cardType, "J") || 
        stringcmp(cardType, "Q") || stringcmp(cardType, "K")) return 10;
    if (stringcmp(cardType, "A")) return 11;
    return 0; //shouldn't happen
}

unsigned getHandScore(Card* cards)
{
    if (stringcmp(cards[0].cardType, cards[1].cardType) && stringcmp(cards[1].cardType, cards[2].cardType)) //if all cards are the same
    {
        if (stringcmp(cards[0].cardType, "7")) //if all are 7, return 34
            return 34;
        return getCardScore(cards[0].cardType) * 3; //else return cardValue*3
    }

    bool containsSevenOfClubs = false;
    unsigned sevenOfClubsPos = 0;
    unsigned sevenOfClubsValue = 11;
    for (int i = 0; i < cardsForEachPlayer; i++) //if card contains 7 of clubs, enter special checking mode
    {
        if (stringcmp(cards[i].cardType, "7") && stringcmp(cards[i].suit, "C"))
        {
            containsSevenOfClubs = true;
            sevenOfClubsPos = i;
        }
    }

    if (containsSevenOfClubs)
    {
        //Determine which card is which
        Card card1;
        Card card2;
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

        //two cards of the same type
        if (stringcmp(card1.cardType, card2.cardType)) 
            return card1Score * 2 + sevenOfClubsValue;
        //two cards of the same suit
        if (stringcmp(card1.suit, card2.suit)) 
            return card1Score + card2Score + sevenOfClubsValue;

        //high card
        if (card1Score > card2Score)
            return card1Score + sevenOfClubsValue;
        else 
            return card2Score + sevenOfClubsValue;
    }
    if (stringcmp(cards[0].suit, cards[1].suit) && stringcmp(cards[1].suit, cards[2].suit)) //All suits are the same
        return getCardScore(cards[0].cardType) + getCardScore(cards[1].cardType) + getCardScore(cards[2].cardType);
    if (stringcmp(cards[0].suit, cards[1].suit)) //first two cards are the same
        return getCardScore(cards[0].cardType) + getCardScore(cards[1].cardType);
    if (stringcmp(cards[0].suit, cards[2].suit)) //second two
        return getCardScore(cards[0].cardType) + getCardScore(cards[2].cardType);
    if (stringcmp(cards[1].suit, cards[2].suit)) //first and third
        return getCardScore(cards[1].cardType) + getCardScore(cards[2].cardType);


    unsigned sevensCount = 0;
    unsigned acesCount = 0;
    for (int i = 0; i < cardsForEachPlayer; i++)
    {
        if (stringcmp(cards[i].cardType, "A")) acesCount++;
        if (stringcmp(cards[i].cardType, "7")) sevensCount++;
    }

    if (acesCount >= 2) 
        return 22;
    if (sevensCount >= 2) 
        return 23;

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

void printPlayersCards(Player* players)
{
    for (int i = 0; i < playerCount; i++)
    {
        if (!players[i].isActive)
            continue;
        std::cout << "Player " << i + 1 << "'s cards:\n";
        for (int j = 0; j < cardsForEachPlayer; j++)
        {
            std::cout << players[i].cards[j].cardType << players[i].cards[j].suit << ' ';
        }
        std::cout << " (Score: " << getHandScore(players[i].cards) << ")\n";
    }
}

void determineWinner(Player* players, unsigned pot)
{
    unsigned highestScore = 0;
    unsigned playerWithHighScore = 0;
    for (int i = 0; i < playerCount; i++)
    {
        unsigned currScore = getHandScore(players[i].cards);
        if (currScore > highestScore)
        {
            highestScore = currScore;
            playerWithHighScore = i;
        }
    }

    unsigned numberOfPlayersWithHighestScore = 0;
    for (int i = 0; i < playerCount; i++)
    {
        unsigned currScore = getHandScore(players[i].cards);
        if (currScore == highestScore)
            numberOfPlayersWithHighestScore++;
    }

    if (numberOfPlayersWithHighestScore == 1)
    {
        std::cout << "Player " << playerWithHighScore + 1 << " wins!";
        players[playerWithHighScore].money += pot;
    }
    if (numberOfPlayersWithHighestScore > 1)
    {
        std::cout << "It is a tie\n";
        int sumToPay = pot / 2;
        for (int i = 0; i < playerCount; i++)
        {
            if (getHandScore(players[i].cards) < highestScore)
            {
                char choice;
                std::cout << "Player " << i + 1 << ", would you like to enter the tie? You have to pay " << sumToPay << "(y\\n): ";
                std::cin >> choice;
                if (choice == 'y')
                {
                    players[i].money -= sumToPay;
                    pot += sumToPay;
                    players[i].isActive = true;
                }
                else
                {
                    players[i].isActive = false;
                }
            }
            else
                players[i].isActive = true;

        }

        shuffleDeck();
        dealCards(players);
        printPlayersCards(players);

        bettingLoop(players, pot);
        determineWinner(players, pot);
    }
}

int main()
{
    std::cout << "How many players are playing(2-9)?\n";
    while (true)
    {
        std::cin >> playerCount;
        if (playerCount >= 2 && playerCount <= 9) break;
        std::cerr << "Invalid number of players. Please enter a number between 2 and 9.\n";
    }


    inflateDeck();
    Player* players = new Player[playerCount];

    while (true)
    {
        for (int i = 0; i < playerCount; i++)
            players[i].isActive = true;
        shuffleDeck();
        dealCards(players);
        printPlayersCards(players);

        unsigned pot = 0;
        bettingLoop(players, pot);
        determineWinner(players, pot);
        char choice;
        std::cout << "Start a new game? (y/n): ";
        std::cin >> choice;
        if (choice != 'y')
            break;
    }
    releaseMemory(players);

    return 0;
}
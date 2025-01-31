#include <iostream>
#include <fstream>
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
        if (players[i].isActive) activePlayerCount++;
    }
    return activePlayerCount == 1;
}

void initializeBetting(Player* players, unsigned& pot, int& currentBet) 
{
    currentBet = CHIP_VALUE;
    for (int i = 0; i < playerCount; i++)
    {
        if (players[i].money >= CHIP_VALUE)
        {
            players[i].totalContribution += CHIP_VALUE;
            players[i].isActive = true;
            players[i].money -= CHIP_VALUE;
            pot += CHIP_VALUE;
        }
        else
        {
            players[i].isActive = false;
            players[i].hasActed = true;
            players[i].totalContribution = 0;
        }
    }
}

void displayPlayerStatus(const Player& player, int playerIndex, unsigned pot, int currentBet) 
{
    std::cout << "\nPlayer " << playerIndex + 1 << ", it's your turn.\n";
    std::cout << "Pot: " << pot << "\n";
    std::cout << "Current bet: " << currentBet << "\n";
    std::cout << "Your total contribution: " << player.totalContribution << "\n";
    std::cout << "Your money: " << player.money << "\n";
    std::cout << "Your cards: ";
    for (int j = 0; j < cardsForEachPlayer; j++) 
    {
        std::cout << player.cards[j].cardType << player.cards[j].suit << " ";
    }
    std::cout << "\n";
}

char getPlayerAction() 
{
    char action;
    std::cout << "Choose an action: (c) Call, (r) Raise, (f) Fold\n";
    std::cin >> action;
    while (action != 'c' && action != 'r' && action != 'f')
    {
        std::cout << "Invalid action. Please choose (c) Call, (r) Raise, or (f) Fold: ";
        std::cin >> action;
    }
    return action;
}

void handleCall(Player& player, unsigned& pot, int currentBet) 
{
    int amountToCall = currentBet - player.totalContribution;
    if (amountToCall > player.money) 
    {
        std::cout << "Player cannot afford to call and must fold.\n";
        player.isActive = false;
    }
    else 
    {
        player.money -= amountToCall;
        pot += amountToCall;
        player.totalContribution += amountToCall;
        player.hasActed = true;
    }
}

void handleRaise(Player* players, Player& player, unsigned& pot, int& currentBet) 
{
    unsigned amountToRaise = 0;
    std::cout << "Amount to Raise: ";

    std::cin >> amountToRaise;

    while (amountToRaise > player.money || amountToRaise <= 0) 
    {
        std::cout << "Invalid amount. You can raise up to " << player.money << ": ";
        std::cin >> amountToRaise;
    }

    currentBet += amountToRaise;
    pot += amountToRaise;
    player.totalContribution += amountToRaise;
    player.money -= amountToRaise;

    for (int j = 0; j < playerCount; j++) 
    {
        if (players[j].isActive && players[j].hasActed)
            players[j].hasActed = false;
    }
    player.hasActed = true;
}

void handleFold(Player& player) 
{
    player.isActive = false;
    player.hasActed = true;
}

bool isBettingComplete(Player* players) 
{
    return allPlayersHaveActed(players) || onePlayerActive(players);
}

void bettingLoop(Player* players, unsigned& pot) 
{
    int currentBet = 0;
    bool bettingComplete = false;

    // Initialize betting
    initializeBetting(players, pot, currentBet);

    while (!bettingComplete) 
    {
        for (int i = 0; i < playerCount; i++) 
        {
            if (!players[i].isActive || players[i].hasActed) continue;

            displayPlayerStatus(players[i], i, pot, currentBet);
            char action = getPlayerAction();

            if (action == 'c') 
            {
                handleCall(players[i], pot, currentBet);
            }
            else if (action == 'r') 
            {
                handleRaise(players, players[i], pot, currentBet);
            }
            else if (action == 'f') 
            {
                handleFold(players[i]);
            }

            if(isBettingComplete(players)) 
            {
                bettingComplete = true;
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

bool allCardsSameType(Card* cards)
{
    return stringcmp(cards[0].cardType, cards[1].cardType) && stringcmp(cards[1].cardType, cards[2].cardType);
}

bool allCardsAreSevens(Card* cards)
{
    return stringcmp(cards[0].cardType, "7");
}

bool checkForSevenOfClubs(Card* cards, unsigned& sevenOfClubsPos)
{
    for (int i = 0; i < cardsForEachPlayer; i++)
    {
        if (stringcmp(cards[i].cardType, "7") && stringcmp(cards[i].suit, "C"))
        {
            sevenOfClubsPos = i;
            return true;
        }
    }
    return false;
}

unsigned handleSevenOfClubsCase(Card* cards, unsigned sevenOfClubsPos)
{
    Card card1, card2;
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

    if (stringcmp(card1.cardType, card2.cardType))
        return card1Score * 2 + 11;

    if (stringcmp(card1.suit, card2.suit))
        return card1Score + card2Score + 11;

    if (card1Score > card2Score)
        return card1Score + 11;
    else
        return card2Score + 11;
}

bool allSuitsSame(Card* cards)
{
    return stringcmp(cards[0].suit, cards[1].suit) && stringcmp(cards[1].suit, cards[2].suit);
}

unsigned getTotalScoreForAllCards(Card* cards)
{
    return getCardScore(cards[0].cardType) + getCardScore(cards[1].cardType) + getCardScore(cards[2].cardType);
}

bool firstTwoCardsSameSuit(Card* cards)
{
    return stringcmp(cards[0].suit, cards[1].suit);
}

bool firstAndThirdCardsSameSuit(Card* cards)
{
    return stringcmp(cards[0].suit, cards[2].suit);
}

bool secondAndThirdCardsSameSuit(Card* cards)
{
    return stringcmp(cards[1].suit, cards[2].suit);
}

unsigned countAces(Card* cards)
{
    unsigned acesCount = 0;
    for (int i = 0; i < cardsForEachPlayer; i++)
    {
        if (stringcmp(cards[i].cardType, "A"))
            acesCount++;
    }
    return acesCount;
}

unsigned countSevens(Card* cards)
{
    unsigned sevensCount = 0;
    for (int i = 0; i < cardsForEachPlayer; i++)
    {
        if (stringcmp(cards[i].cardType, "7"))
            sevensCount++;
    }
    return sevensCount;
}

unsigned getHighestCardScore(Card* cards)
{
    unsigned highestScore = 0;
    for (int i = 0; i < cardsForEachPlayer; i++)
    {
        if (getCardScore(cards[i].cardType) > highestScore)
            highestScore = getCardScore(cards[i].cardType);
    }
    return highestScore;
}

unsigned getHandScore(Card* cards)
{
    if (allCardsSameType(cards))
    {
        if (allCardsAreSevens(cards))
            return 34;
        return getCardScore(cards[0].cardType) * 3;
    }

    unsigned sevenOfClubsPos;
    bool containsSevenOfClubs = checkForSevenOfClubs(cards, sevenOfClubsPos);

    if (containsSevenOfClubs)
    {
        return handleSevenOfClubsCase(cards, sevenOfClubsPos);
    }

    if (allSuitsSame(cards))
    {
        return getTotalScoreForAllCards(cards);
    }

    if (firstTwoCardsSameSuit(cards))
    {
        return getCardScore(cards[0].cardType) + getCardScore(cards[1].cardType);
    }

    if (firstAndThirdCardsSameSuit(cards))
    {
        return getCardScore(cards[0].cardType) + getCardScore(cards[2].cardType);
    }

    if (secondAndThirdCardsSameSuit(cards))
    {
        return getCardScore(cards[1].cardType) + getCardScore(cards[2].cardType);
    }

    unsigned acesCount = countAces(cards);
    unsigned sevensCount = countSevens(cards);

    if (acesCount >= 2)
        return 22;
    if (sevensCount >= 2)
        return 23;

    return getHighestCardScore(cards);
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
        if (!players[i].isActive)
            continue;
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
        if (!players[i].isActive)
            continue;
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
                    players[i].hasActed = false;
                }
                else
                {
                    players[i].isActive = false;
                    players[i].hasActed = true;
                }
            }
            else
            {
                players[i].isActive = true;
                players[i].hasActed = false; 
            }

        }

        shuffleDeck();
        dealCards(players);
        printPlayersCards(players);

        bettingLoop(players, pot);
        determineWinner(players, pot);
    }
}

void createFile(Player* players)
{
    const char* filename = "money.txt";
    std::ofstream file(filename);
    for (int i = 0; i < 9; i++)
    {
        file << players[i].money << std::endl;
    }
}

void readFile(Player* players)
{
    const char* filename = "money.txt";
    std::ifstream file(filename);
    if (!file)
    {
        createFile(players);
        return;
    }
    for (int i = 0; i < playerCount; i++)
    {
        file >> players[i].money;
    }
}

void writeFile(Player* players)
{
    const char* filename = "money.txt";
    std::ofstream file(filename);
    for (int i = 0; i < playerCount; i++) 
    {
        file << players[i].money << std::endl;
    }
    file.close();
}

void initializeGame()
{
    std::cout << "How many players are playing (2-9)?\n";
    while (true)
    {
        char playerCountCh;
        std::cin >> playerCount;
        if (playerCount >= 2 && playerCount <= 9) break;
        std::cerr << "Invalid number of players. Please enter a number between 2 and 9.\n";
    }
}

void playGame(Player* players)
{
    while (true)
    {
        for (int i = 0; i < playerCount; i++)
        {
            players[i].isActive = true;
            players[i].hasActed = false;
        }
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
        {
            writeFile(players);
            break;
        }
    }
    
}
int main()
{
    initializeGame();
    inflateDeck();
    Player* players = new Player[playerCount];
    readFile(players);
    playGame(players);
    releaseMemory(players);

    return 0;
}
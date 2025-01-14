#include <iostream>

const unsigned cardTypesSize = 8;
const unsigned suitsSize = 4;
const unsigned deckSize = cardTypesSize*suitsSize*2; //32 for card type, 32 for suits

const char cardTypes[][3] = {"7", "8", "9", "10", "J", "Q", "K", "A"};
const char suits[][2] = {"C", "D", "H", "S"};
char deck[deckSize][3];

unsigned playerCount = 0;
const unsigned cardsForEachPlayer = 3;

unsigned long seed = 123456789;

unsigned long lcg(unsigned long& seed) 
{
    seed = (1664525 * seed + 1013904223) % 4294967296;
    return seed;
}
void copystr(const char src[], char dest[]) 
{
    int i = 0;
    while (src[i] != '\0') 
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}
void appendstr(const char src[], char dest[]) 
{
    int dest_len = 0;
    while (dest[dest_len] != '\0') 
    {
        dest_len++;
    }

    int src_len = 0;
    while (src[src_len] != '\0') 
    {
        dest[dest_len] = src[src_len];
        dest_len++;
        src_len++;
    }

    dest[dest_len] = '\0';
}

//Fisher-Yates
void shuffleDeck() 
{
    for (int i = deckSize - 2; i >= 2; i -= 2) 
    {
        unsigned int j = lcg(seed) % (i / 2 + 1) * 2;

        // Swap card type
        char temp[3];
        copystr(deck[i], temp);
        copystr(deck[j], deck[i]);
        copystr(temp, deck[j]);

        // Swap suit
        copystr(deck[i + 1], temp);
        copystr(deck[j + 1], deck[i + 1]);
        copystr(temp, deck[j + 1]);
    }
}

void dealCards(char*** &playersCards)
{
    for (int i = 0; i < playerCount; i++)
    {
        playersCards[i] = new char* [cardsForEachPlayer];

        for (int j = 0; j < cardsForEachPlayer; j++)
        {
            playersCards[i][j] = new char[4];
        }
    }

    unsigned cardIndex = 0;
    for (int i = 0; i < cardsForEachPlayer; i++)
    {
        for (int j = 0; j < playerCount; j++)
        {
            char card[4];
            copystr(deck[cardIndex++], card);
            appendstr(deck[cardIndex++], card);
            copystr(card, playersCards[j][i]);
        }
    }
}
void inflateDeck()
{
    unsigned counter = 0;
    for (int i = 0; i < cardTypesSize; i++) {
        for (int j = 0; j < suitsSize; j++) {
            copystr(cardTypes[i], deck[counter++]);
            copystr(suits[j], deck[counter++]);
        }
    }
}

void releaseMemory(char*** &playersCards)
{
    for (int i = 0; i < playerCount; i++)
    {
        for (int j = 0; j < cardsForEachPlayer; j++)
        {
            delete[] playersCards[i][j];
        }
        delete[] playersCards[i];
    }
    delete[] playersCards;
}

int main() {
    std::cout << "How many players are playing(2-9)?\n";
    std::cin >> playerCount;
    inflateDeck();
    shuffleDeck();

    char*** playersCards = new char** [playerCount];

    dealCards(playersCards);
    for (int i = 0; i < playerCount; i++)
    {
        std::cout << "Player " << i+1 << "'s cards:\n";
        for (int j = 0; j < cardsForEachPlayer; j++)
        {
            std::cout << playersCards[i][j] << ' ';
        }
        std::cout << "\n";
    }
    
    releaseMemory(playersCards);

    return 0;
}
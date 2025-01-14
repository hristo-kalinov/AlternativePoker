#include <iostream>

const unsigned cardTypesSize = 8;
const unsigned suitsSize = 4;
const unsigned deckSize = cardTypesSize*suitsSize*2; //32 for card type, 32 for suits

const char cardTypes[][3] = {"7", "8", "9", "10", "J", "Q", "K", "A"};
const char suits[][2] = {"C", "D", "H", "S"};
char deck[deckSize][3];
unsigned long seed = 1343782137;

unsigned long lcg(unsigned long& seed) {
    seed = (1664525 * seed + 1013904223) % 4294967296;
    return seed;
}


void copystr(const char src[], char dest[]) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

//Fisher-Yates
void shuffleDeck(unsigned long& seed) {
    for (int i = deckSize - 2; i >= 2; i -= 2) {
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

int main() {
    unsigned counter = 0;

    for (int i = 0; i < cardTypesSize; i++) {
        for (int j = 0; j < suitsSize; j++) {
            copystr(cardTypes[i], deck[counter++]);
            copystr(suits[j], deck[counter++]);
        }
    }

    std::cout << "Deck before shuffling:\n";
    for (int i = 0; i < deckSize; i += 2) {
        std::cout << deck[i] << deck[i + 1] << " ";
    }
    std::cout << std::endl;

    shuffleDeck(seed);

    std::cout << "Deck after shuffling:\n";
    for (int i = 0; i < deckSize; i += 2) {
        std::cout << deck[i] << deck[i + 1] << " ";
    }
    std::cout << std::endl;

    return 0;
}
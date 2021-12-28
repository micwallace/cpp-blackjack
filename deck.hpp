#include <iostream>
#include <tuple>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
using namespace std;

string cardTypes[13] = {"Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten", "Jack", "Queen", "King", "Ace"};
string cardSuits[4] = {"Diamonds", "Hearts", "Clubs", "Spades"};

vector<tuple<string, string>> getAllCards(int nDecks){
    std::vector<tuple<string, string>> cards;

    for (int i=0; i < (sizeof(cardTypes) / sizeof(*cardTypes)); i++){
        for (int x=0; x < (sizeof(cardSuits) / sizeof(*cardSuits)); x++){
            for (int n=0; n < nDecks; n++){
                cards.push_back(make_tuple(cardTypes[i], cardSuits[x]));
            }
        }
    }

    return cards;
}

void shuffleCards(vector<tuple<string, string>> &cards){
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    auto rng = std::default_random_engine { seed };

    shuffle(cards.begin(), cards.end(), rng);
    shuffle(cards.begin(), cards.end(), rng);
}

int cardScore(string cardType){
    if (cardType == "Two") return 2;
    if (cardType == "Three") return 3;
    if (cardType == "Four") return 4;
    if (cardType == "Five") return 5;
    if (cardType == "Six") return 6;
    if (cardType == "Seven") return 7;
    if (cardType == "Eight") return 8;
    if (cardType == "Nine") return 9;
    if (cardType == "Ten" || cardType == "Jack" ||
        cardType == "Queen" || cardType == "King") return 10;
    if (cardType == "Ace") return 11;
    return 0;
}

tuple<string, string, int> drawCard(){
    vector<tuple<string, string>> cards = getAllCards(4);

    shuffleCards(cards);

    string type = get<0>(cards[0]);
    string suit = get<1>(cards[0]);
    int score = cardScore(type);

    return make_tuple(type, suit, score);
}
#include <iostream>
#include "game.hpp"
using namespace std;

string helpTxts[] = {"Type command to continue:",
                       "help - view this cheetsheet",
                       "new <name> <player> - create a new game",
                       "delete <name> - delete game",
                       "join <game> <player> - join game",
                       "hit <game> <player> - take a card from the deck",
                       "stand <game> <player> - move onto guest player or end the game",
                       "show <game> - view game data and score"};

void helpText(){
    for (int i=0; i < (sizeof(helpTxts) / sizeof(*helpTxts)); ++i){
        cout << helpTxts[i] << "\n";
    }
    cout << "\n";
}

string * parseCommand(string command){

    static string parts[3];
    parts[0] = "";
    parts[1] = "";
    parts[2] = "";

    int curCommandIndex = 0;
    char lastChar = '\0';

    for (int i=0; i < sizeof(command); i++){

        if (curCommandIndex > 2 || command[i] == '\0')
            break;

        if (command[i] == ' '){
            if (lastChar == ' '){
                lastChar = command[i];
                continue;
            } else {
                curCommandIndex++;
            }
        } else {
            parts[curCommandIndex] += command[i];
        }
    }

    cout << "\n\n";

    return parts;
}

bool checkParams(string* parts, int num){

    if (num==1 && parts[1] == ""){
        cout << "Game name parameter required\n\n";
        return false;
    }

    if (num==2 && (parts[1] == "" || parts[2] == "")){
        cout << "Game name and player parameters required\n\n";
        return false;
    }

    return true;
}

blackjack::Blackjack game;

void processCommand(string command){

    string* parts = parseCommand(command);
    string action = parts[0];

    if (action == "help") {
        helpText();
        return;
    }
    if (action == "new"){
        if (!checkParams(parts, 2)) return;
        game.createGame(parts[1], parts[2]);
        return;
    }
    if (action == "join"){
        if (!checkParams(parts, 2)) return;
        game.joinGame(parts[1], parts[2]);
        return;
    }
    if (action == "delete"){
        if (!checkParams(parts, 1)) return;
        game.deleteGame(parts[1]);
        return;
    }
    if (action == "hit"){
        if (!checkParams(parts, 2)) return;
        game.hit(parts[1], parts[2]);
        return;
    }
    if (action == "stand"){
        if (!checkParams(parts, 2)) return;
        game.stand(parts[1], parts[2]);
        return;
    }
    if (action == "show"){
        if (!checkParams(parts, 1)) return;
        game.showGame(parts[1]);
        return;
    }

    cout << "Invalid command!! Type help to view commands again!\n\n";
}

void commandLoop(){
    string command;
    getline(cin, command);
    processCommand(command);
    commandLoop();
}

int main() {
    cout << "Welcome to Blackjack!\n\n";
    helpText();
    commandLoop();
    return 0;
}
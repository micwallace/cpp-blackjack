#include <iostream>
#include <tuple>
#include <vector>
#include "lib/json.hpp"
#include "deck.hpp"

// for convenience
using json = nlohmann::json;
using namespace std;

namespace blackjack {

    class Blackjack {

        private: json gameState = json({});

        public: int gameExists(string name){
            return gameState.contains(name);
        }

        public: json getGame(string name){
            if (!gameState.contains(name))
                return NULL;

            return gameState[name];
        }

        // TODO: Print card array
        public: void printScore(json game){
            cout << "--- score ---\n";
            cout << "Host: " << game["hostPlayer"].get<string>() << "\t" << "\t" << game["hostScore"].get<int>() << "\n";
            cout << "Guest: " << game["guestPlayer"].get<string>() << "\t" << "\t" << game["guestScore"].get<int>() << "\n\n";
        }

        public: void printStatus(json game){

            int hs = game["hostScore"].get<int>();
            int gs = game["guestScore"].get<int>();

            if (hs > 21){
                cout << game["hostPlayer"].get<string>() << " is bust, " <<
                        game["guestPlayer"].get<string>() << " has won the game!!";
            } else if (gs > 21){
                cout << game["guestPlayer"].get<string>() << " is bust, " <<
                        game["hostPlayer"].get<string>() << " has won the game!!";
            } else if (game["status"].get<std::string>() == "finished"){
                if (hs > gs){
                    cout << game["hostPlayer"].get<string>() << " has won the game!!";
                } else if (gs > hs) {
                    cout << game["guestPlayer"].get<string>() << " has won the game!!";
                } else {
                    cout << "The game is a tie!!";
                }
            } else {
                if (game["turn"].get<string>() == "host"){
                    cout << game["hostPlayer"].get<string>() << "'s turn...";
                } else {
                    cout << game["guestPlayer"].get<string>() << "'s turn...";
                }
            }
            cout << "\n\n";
        }

        private: json getInitialState(string name, string hostName){
            return json({
                {"name", name},
                {"hostPlayer", hostName},
                {"hostScore", 0},
                {"hostHand", json::array()},
                {"guestPlayer", ""},
                {"guestScore", 0},
                {"guestHand", json::array()},
                {"turn", "host"},
                {"status", "waiting"}
            });
        }

        public: bool createGame(string name, string hostName){
            if (gameExists(name)){
                cout << "Game already exists!\n\n";
                return false;
            }
            gameState[name] = getInitialState(name, hostName);

            cout << "Starting new game with name: " << name << "\n\n";

            return true;
        }

        public: bool joinGame(string name, string guestName){
            if (!gameExists(name)){
                cout << "Game does not exist!\n\n";
                return false;
            }
            if (gameState[name]["guestPlayer"] != ""){
                cout << "Game already has a guest player!\n\n";
                return false;
            }

            if (gameState[name]["hostPlayer"] == guestName){
                cout << "Host player has the same name, choose a different one!\n\n";
                return false;
            }

            gameState[name]["guestPlayer"] = guestName;
            gameState[name]["status"] = "playing";

            cout << "Player " << guestName << " has joined game " << name << "\n\n";

            return true;
        }

        public: bool deleteGame(string name){
            if (!gameExists(name)){
                cout << "Game does not exist!\n\n";
                return false;
            }

            gameState.erase(name);

            cout << "Game successfully deleted!\n\n";

            return true;
        }

        public: bool showGame(string name){
            if (!gameExists(name)){
                cout << "Game does not exist\n\n";
                return false;
            }
            json game = getGame(name);
            cout << game.dump() << "\n\n";
            printScore(game);
            printStatus(game);
            return true;
        }

        private: bool isPlayersTurn(string name, string player){
            json game = getGame(name);
            if (game["turn"] == "host" && game["hostPlayer"] == player) return true;
            if (game["turn"] == "guest" && game["guestPlayer"] == player) return true;
            return false;
        }

        private: bool isGameFinished(json game){
            if (game["status"] == "finished") return true;
            return false;
        }

        private: int * getScoreAndAces(json cards){

            static int sa[2];
            sa[0] = 0;
            sa[1] = 0;
            string c;

            for (json::iterator it = cards.begin(); it != cards.end(); ++it) {
                json card = *it;
                c = card[0].get<std::string>();
                if (c == "Ace") sa[1]++;
                sa[0] += cardScore(c);
            }

            return sa;
        }

        private: int getAceReducedScore(json cards){

            int* sa = getScoreAndAces(cards);
            int score = sa[0];
            int aces = sa[1];

            while (score > 21 && aces > 0){
                score -= 10;
                aces -= 1;
            }

            return score;
        }

        public: bool hit(string name, string player){

            if (!gameExists(name)){
                cout << "Game does not exist!\n\n";
                return false;
            }

            json game = getGame(name);

            if (game["status"].get<std::string>() == "waiting"){
                cout << "Waiting for guest!\n\n";
                return false;
            }

            if (!isPlayersTurn(name, player)){
                cout << "Not your turn!\n\n";
                return false;
            }

            if (game["status"].get<std::string>() == "finished"){
                cout << "Game Over!\n\n";
                return false;
            }

            tuple<string, string, int> card = drawCard();
            string type = get<0>(card);
            string suit = get<1>(card);
            int cScore = get<2>(card);

            string handKey = game["turn"].get<std::string>() + "Hand";
            string scoreKey = game["turn"].get<std::string>() + "Score";

            int score = cScore + game[scoreKey].get<int>();

            json curHand = game[handKey];
            curHand.push_back(json({type, suit}));

            if (score > 21)
                score = getAceReducedScore(curHand);

            gameState[name][handKey] = curHand;
            gameState[name][scoreKey] = score;

            if (score > 21)
                gameState[name]["status"] = "finished";

            cout << "Card drawn: " << type << " " << suit << " (" << cScore << ") \n\n";

            printScore(gameState[name]);

            printStatus(gameState[name]);

            return true;
        }

        public: bool stand(string name, string player){

            if (!gameExists(name)){
                cout << "Game does not exist!\n\n";
                return false;
            }
            if (!isPlayersTurn(name, player)){
                cout << "Not your turn!\n\n";
                return false;
            }

            json game = getGame(name);

            if (game["status"].get<std::string>() == "finished"){
                cout << "Game Over!\n\n";
                return false;
            }

            if (game["turn"] == "host"){
                gameState[name]["turn"] = "guest";
            } else {
                gameState[name]["status"] = "finished";
            }

            printScore(gameState[name]);

            printStatus(gameState[name]);

            return true;
        }

    };

}
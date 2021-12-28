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

        public: void printGame(string name){
            json game = getGame(name);

            cout << "-------------- SCORE --------------\n";
            cout << "Host:\t\t" << game["hostScore"].get<int>() << "\n";
            cout << "Guest:\t\t" << game["guestScore"].get<int>() << "\n\n";
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
            return true;
        }

        public: bool joinGame(string name, string guestName){
            if (!gameExists(name)){
                cout << "Game does not exist!\n\n";
                return false;
            }
            if (gameState[name]["guestPlayer"] != ""){
                cout << "Game already has a guest player!";
                return false;
            }

            gameState[name]["guestPlayer"] = guestName;
            gameState[name]["status"] = "playing";
            return true;
        }

        public: bool deleteGame(string name){
            if (!gameExists(name)){
                cout << "Game does not exist!\n\n";
                return false;
            }

            gameState.erase(name);

            cout << "Game successfully deleted!\n\n";
        }

        private: bool isPlayersTurn(string name, string player){
            json game = getGame(name);
            if (game["turn"] == "host" && game["hostPlayer"] == player) return true;
            if (game["turn"] == "guest" && game["guestPlayer"] == player) return true;
            return false;
        }

        public: bool hit(string name, string player){

            if (!gameExists(name)){
                cout << "Game does not exist!\n\n";
                return false;
            }
            if (!isPlayersTurn(name, player)){
                cout << "Not your turn!\n\n";
                return false;
            }

            json game = getGame(name);

            if (game["status"].get<std::string>() == "waiting"){
                cout << "Waiting for guest!\n\n";
                return false;
            }

            tuple<string, string, int> card = drawCard();
            string type = get<0>(card);
            string suit = get<1>(card);
            int score = get<2>(card);

            // TODO: process ace score

            string handKey = game["turn"].get<std::string>() + "Hand";
            string scoreKey = game["turn"].get<std::string>() + "Score";

            gameState[name][handKey].push_back(json({type, suit}));
            gameState[name][scoreKey] = score + game[scoreKey].get<int>();

            cout << type << " " << suit << " (" << score << ") \n\n";

            printGame(name);

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

            return true;
        }

    };

}
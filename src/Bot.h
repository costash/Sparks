#ifndef BOT_H_
#define BOT_H_

#include "State.h"

/*
    This struct represents your bot in the game of Ants
*/
struct Bot
{
    State state;
    
    Bot();

    void playGame();    //plays a single game of Ants

    void makeMoves();   //makes moves for a single turn

//    void exploreFood(std::vector<bool>);     //explore for food
    void exploreFood();

//    void exploreMap ( std::vector<bool> );    //explore map
    void exploreMap();

    void endTurn();     //indicates to the engine that it has made its moves
};

#endif //BOT_H_

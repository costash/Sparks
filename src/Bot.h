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

    void exploreFood(); //assign ants to food and move to it

    void exploreMap();  //explore map with unused ants

    void endTurn();     //indicates to the engine that it has made its moves

	void getToBorder();

	void sendToBorder();
};

#endif //BOT_H_

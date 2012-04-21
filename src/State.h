#ifndef STATE_H_
#define STATE_H_

#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <stdint.h>
#include <map>

#include "Timer.h"
#include "Bug.h"
#include "Square.h"
#include "Location.h"

using namespace std;

/*
    constants
*/
const int TDIRECTIONS = 4;
const char CDIRECTIONS[4] = {'N', 'E', 'S', 'W'};
const int DIRECTIONS[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };      //{N, E, S, W}

/*
    struct to store current state information
*/
struct State
{
    /*
        Variables
    */
    int rows, cols,
        turn, turns,
        noPlayers;
    double attackradius, spawnradius, viewradius;
    double loadtime, turntime;
    std::vector<double> scores;
    bool gameover;
    int64_t seed;

    std::vector<std::vector<Square> > grid;
    std::vector<Location> myAnts, enemyAnts, myHills, enemyHills, food;
    std::vector<bool> used;     //wether an ant has a target or not
    std::vector <int> mission;

    Timer timer;
    Bug bug;


	struct explore {
		Location loc;
		int depth;
		int direction;
	};

	struct expBorder {
		int antIndex;
		Location loc;
		int depth;
		int antType;
		int direction ;
	};

    /*
        Functions
    */
    State();
    ~State();

    void setup();
    void reset();

	void newTurn();
	void initHistory();
	void updateHistory();
	void setReachableTiles();
    void printHistory();





    void updateLastVisit();
    void makeMove(const Location &loc, int direction);
    double distance(const Location &loc1, const Location &loc2);
    Location getLocation(const Location &startLoc, int direction);
    void updateVisionInformation();
};

std::ostream& operator<<(std::ostream &os, const State &state);
std::istream& operator>>(std::istream &is, State &state);

#endif //STATE_H_

#ifndef SQUARE_H_
#define SQUARE_H_

#include <vector>

/*
    struct for representing a square in the grid.
*/
struct Square
{
    bool isVisible, isWater, isHill, isFood;
    int ant, hillPlayer, indexAnt;  //indexAnt is index for myAnts vector

    int lastVisit;  //-1 if never visited; 0 if ant is there; other positive values
    std::vector<int> deadAnts;

    int history;
    int border;

    Square()
    {
        isVisible = isWater = isHill = isFood = 0;
        ant = hillPlayer = indexAnt = lastVisit = -1;
    };

    //resets the information for the square except water information
    void reset()
    {
        isVisible = 0;
        isHill = 0;
        isFood = 0;
        ant = hillPlayer = -1;
        deadAnts.clear();
    };
};

#endif //SQUARE_H_

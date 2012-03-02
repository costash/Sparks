#include "Bot.h"
#include "BfsQueueElement.h"

using namespace std;

//constructor
Bot::Bot()
{

};

//plays a single game of Ants.
void Bot::playGame()
{
    //reads the game parameters and sets up
    cin >> state;
    state.setup();
    endTurn();

    //continues making moves while the game is not over
    while(cin >> state)
    {
        state.updateVisionInformation();
        makeMoves();
        endTurn();
    }
};

//makes the bots moves for the turn
void Bot::makeMoves()
{
    state.bug << "turn " << state.turn << ":" << endl;
    state.bug << state << endl;

    //picks out moves for each ant
    std::vector<bool> used( state.myAnts.size(), 0);
    exploreFood(used);


    state.bug << "time taken: " << state.timer.getTime() << "ms" << endl << endl;
};

//explore for food
void Bot::exploreFood(std::vector<bool> used)
{
    Location foodLoc, currentLoc, newLoc;
    std::queue<BfsQueueElement> foodQueue;
    std::vector<std::vector<bool> > visited( state.rows, std::vector<bool>(state.cols, 0) );
    std::vector<bool> eaten( state.food.size(), 0);
    BfsQueueElement elem;

    for(int i = 0; i < (int) state.food.size(); ++i)
    {
        foodLoc = state.food[i];
        foodQueue.push(BfsQueueElement( foodLoc, i));
        visited[foodLoc.row][foodLoc.col] = 1;
    }

    while ( !foodQueue.empty() )
    {
        elem = foodQueue.front();
        currentLoc = elem.loc;
        foodQueue.pop();

        if(eaten[elem.root])
            continue;   //if an ant is sent to food, the BFS is ended

        for ( int d = 0; d < TDIRECTIONS; ++d )
        {
            newLoc = state.getLocation(currentLoc, d);

            if (!visited[newLoc.row][newLoc.col])
                if(state.grid[newLoc.row][newLoc.col].isVisible)
                {
                    if(!state.grid[newLoc.row][newLoc.col].isWater)
                    {
                        foodQueue.push( BfsQueueElement( newLoc, elem.root) );
                        visited[newLoc.row][newLoc.col] = 1;
                    }

                    if(state.grid[newLoc.row][newLoc.col].ant == 0 &&
                        used[state.grid[newLoc.row][newLoc.col].inMyAnts] == 0) //our ant is here and it's not used
                    {
                        int antDir;
                        switch(d)
                        {
                            case 0: antDir = 2; break;
                            case 1: antDir = 3; break;
                            case 2: antDir = 0; break;
                            case 3: antDir = 1; break;
                        }

                        if( state.grid[currentLoc.row][currentLoc.col].ant == -1 )
                        {
                            state.makeMove( newLoc, antDir);
                            used[state.grid[newLoc.row][newLoc.col].inMyAnts] = 1;
                            eaten[elem.root] = 1;
                        }

                    }
            }
        }
    }
};

//finishes the turn
void Bot::endTurn()
{
    if(state.turn > 0)
        state.reset();
    state.turn++;

    cout << "go" << endl;
};

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
        state.updateLastVisit();
        makeMoves();
        endTurn();
    }
};

//makes the bots moves for the turn
void Bot::makeMoves()
{
    state.bug << "turn " << state.turn << ":" << endl;
    state.bug << state << endl;

    exploreFood();
    exploreMap();


    state.bug << "time taken: " << state.timer.getTime() << "ms" << endl << endl;
};

//assign ants to food and move to it
void Bot::exploreFood()
{
    Location foodLoc, currentLoc, newLoc;
    std::queue<BfsQueueElement> foodQueue;
    std::vector<std::vector<bool> > visited( state.rows, std::vector<bool>(state.cols, 0) );
    std::vector<bool> eaten( state.food.size(), 0);
    BfsQueueElement elem;

    for( int i = 0; i < (int) state.food.size(); ++i )
    {
        foodLoc = state.food[i];
        foodQueue.push( BfsQueueElement( foodLoc, i ) );
        visited[foodLoc.row][foodLoc.col] = 1;
    }

    while( !foodQueue.empty() )
    {
        elem = foodQueue.front();
        currentLoc = elem.loc;
        foodQueue.pop();

        if( eaten[elem.root] )
            continue;   //if an ant is sent to food, the BFS is ended

        for( int d = 0; d < TDIRECTIONS; ++d )
        {
            newLoc = state.getLocation(currentLoc, d);

            if( !visited[newLoc.row][newLoc.col] &&
                state.grid[newLoc.row][newLoc.col].isVisible )
            {
                if( !state.grid[newLoc.row][newLoc.col].isWater )
                {
                    foodQueue.push( BfsQueueElement( newLoc, elem.root ) );
                    visited[newLoc.row][newLoc.col] = 1;
                }

                if( state.grid[newLoc.row][newLoc.col].ant == 0 &&
                    state.used[state.grid[newLoc.row][newLoc.col].inMyAnts] == 0 )
                    //our ant is here and it's not used
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
                        state.used[state.grid[newLoc.row][newLoc.col].inMyAnts] = 1;
                        eaten[elem.root] = 1;

                        state.grid[newLoc.row][newLoc.col].lastVisit = 0;
                    }

                }
            }
        }
    }
};

//explore map with unused ants
void Bot::exploreMap()
{
    std::queue<Location> antQueue;
    Location sLoc, cLoc, nLoc;
    for( int i = 0; i < state.used.size(); ++i )
    {
        sLoc = state.myAnts[i];
        antQueue.push( sLoc );
        int sDir, oldestVisit = 0;

        std::vector<std::vector<bool> > visited(state.rows, std::vector<bool>(state.cols, 0));
        visited[sLoc.row][sLoc.col] = 1;
        bool exitBfs = false;

        //TODO improve function with more clear directions for each ant
        while( !antQueue.empty() && !exitBfs )
        {
            cLoc = antQueue.front();
            antQueue.pop();

            for( int d = 0; d < TDIRECTIONS; ++d )
            {
                nLoc = state.getLocation( cLoc, d );

                if( !visited[nLoc.row][nLoc.col] &&
                    state.grid[nLoc.row][nLoc.col].isVisible )
                {
                    antQueue.push( nLoc );
                }
                visited[nLoc.row][nLoc.col] = 1;


                if( cLoc.row == sLoc.row && cLoc.col == sLoc.col &&
                    state.grid[nLoc.row][nLoc.col].isWater == 0 &&
                    state.grid[nLoc.row][nLoc.col].ant != 0 )
                {
                    //found a not visited place right from the start
                    if( state.grid[nLoc.row][nLoc.col].lastVisit == -1 )
                    {
                        sDir = d;
                        exitBfs = true;
                        state.grid[nLoc.row][nLoc.col].lastVisit = 0;
                        state.makeMove( sLoc, sDir );
                        break;
                    }

                    //try to find the oldest not visited place near start
                    if( state.grid[nLoc.row][nLoc.col].lastVisit > oldestVisit )
                    {
                        oldestVisit = state.grid[nLoc.row][nLoc.col].lastVisit;
                        sDir = d;
                    }
                }
            }
        }
        //TODO this is just a temporary sollution for exiting the loop and move
        if( !exitBfs )
        {
            nLoc = state.getLocation( sLoc, sDir );
            if( state.grid[nLoc.row][nLoc.col].isWater == 0 &&
                    state.grid[nLoc.row][nLoc.col].ant != 0 )
            {
                state.grid[nLoc.row][nLoc.col].lastVisit = 0;
                state.makeMove( sLoc, sDir );
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

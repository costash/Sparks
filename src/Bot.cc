#include "Bot.h"

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
   /* for(int ant = 0; ant < (int) state.myAnts.size(); ant++)
    {
        int d = rand() % TDIRECTIONS;
        Location loc = state.getLocation(state.myAnts[ant], d);
        
        while(state.grid[loc.row][loc.col].isWater)
        {
            d = rand() % TDIRECTIONS;
            loc = state.getLocation(state.myAnts[ant], d);
        }
        state.makeMove( state.myAnts[ant], d );
    }*/
    explore();

    state.bug << "time taken: " << state.timer.getTime() << "ms" << endl << endl;
};

//explore for food
void Bot::explore()
{
    Location foodLoc, currentLoc, newLoc;
    std::queue<Location> foodQueue;
    std::vector<std::vector<bool> > visited( state.rows, std::vector<bool>(state.cols, 0) );
       
    for(int i = 0; i < (int) state.food.size(); ++i)
    {
        foodLoc = state.food[i];
        foodQueue.push(foodLoc);
        visited[foodLoc.row][foodLoc.col] = 1;
    }
    
    while ( !foodQueue.empty() ) 
    {
        currentLoc = foodQueue.front();
        foodQueue.pop();
            
        for ( int d = 0; d < TDIRECTIONS; ++d )
        {
            newLoc = state.getLocation(currentLoc, d);
                
            if (!visited[newLoc.row][newLoc.col])
            	if(state.grid[newLoc.row][newLoc.col].isVisible)
            	{
	            	if(!state.grid[newLoc.row][newLoc.col].isWater &&
	            	   state.grid[newLoc.row][newLoc.col].ant == -1	)
            		{
                    //TODO Insert in a matrix here, the level...
                		foodQueue.push(newLoc);
                		visited[newLoc.row][newLoc.col] = 1;
            		}
            		
            		if(state.grid[newLoc.row][newLoc.col].ant == 0)	//our ant is here
            		{
            			int antDir;
            			switch(d)
            			{
            				case 0: antDir = 2;break;
            				case 1: antDir = 3;break;
            				case 2: antDir = 0;break;
            				case 3: antDir = 1;break;
            			}
            			state.makeMove( newLoc, antDir);
            			//TODO: Find a way to stop if every piece of food has an ant coming
            		}
        	}
	}
    }
    
}

//finishes the turn
void Bot::endTurn()
{
    if(state.turn > 0)
        state.reset();
    state.turn++;

    cout << "go" << endl;
};

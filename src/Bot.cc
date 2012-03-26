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
	srand(state.seed);
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

	state.updateHistory();
	state.setReachableTiles();
    exploreFood();
//  exploreMap();
    exploreMap3();

	if (state.turn == state.turns)
		state.printHistory();

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
                    state.used[state.grid[newLoc.row][newLoc.col].indexAnt] == 0 )
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
                        state.used[state.grid[newLoc.row][newLoc.col].indexAnt] = 1;
                        eaten[elem.root] = 1;

                        state.grid[newLoc.row][newLoc.col].lastVisit = 0;
                    }

                }
            }
        }
    }
};




void Bot::exploreMap3()
{
	FILE *out  = fopen("debug3.txt","a");
	int MAX, SUM, depth, steps = 11, direction;
	State::explore Element;
    queue< State::explore > Inspect;
    Location sLoc, cLoc, nLoc;

    for( unsigned int i = 0; i < state.used.size(); i++ )
    {
    	if (state.used[i] == true)
			continue;

		MAX = 0;
        sLoc = state.myAnts[i];

		fprintf(out,"Furnica %d - coord x=%d , y=%d \t", i, sLoc.row, sLoc.col);

        Element.loc = sLoc;
        Element.depth = 1;

        vector<vector<bool> > visited (state.rows, vector<bool>(state.cols, 0));
		visited[sLoc.row][sLoc.col] = true;

        for (int d=0; d<4; d++)
        {
			nLoc = state.getLocation(sLoc, d);
			if (state.grid[nLoc.row][nLoc.col].isWater == false)
			{
				Element.loc = nLoc;
				Element.direction = d;
				Inspect.push(Element);
			}
        }

        while (Inspect.size())
        {

			fprintf(out,"intra in while\n");

			Element = Inspect.front();
			cLoc = Element.loc;
			depth = Element.depth;
			Inspect.pop();

			fprintf(out,"Element: ");
			fprintf(out,"\t Coord: x=%d \t y=%d\n", cLoc.row , cLoc.col);
			fflush(out);

			if ( depth == steps-1)
			{
				SUM = 0;
				for( int d = 0; d < TDIRECTIONS; ++d )
				{
					nLoc = state.getLocation(cLoc, d);
					if (state.grid[nLoc.row][nLoc.col].isWater == false &&
						visited[nLoc.row][nLoc.col] == true)
					{
						SUM += state.grid[nLoc.row][nLoc.col].history;
					}
				}

				fprintf(out,"Suma : %d \t Coordonate: x=%d \t y=%d \n",SUM, cLoc.row, cLoc.col);

				if (SUM>MAX)
				{
					direction = Element.direction;
					MAX = SUM;
				}

				continue;
			}

			depth++;

            for( int d=0; d < TDIRECTIONS; ++d )
            {
                nLoc = state.getLocation( cLoc, d );
				fprintf(out,"Coordonate: x=%d \t y=%d \t", nLoc.row, nLoc.col);
				fprintf(out,"------ vizitat: %d ",(int)visited[nLoc.row][nLoc.col]);

				if (state.grid[nLoc.row][nLoc.col].isWater == false &&
					visited[nLoc.row][nLoc.col] == false)
				{
					visited[nLoc.row][nLoc.col] = true ;
					Element.loc = nLoc;
					Element.depth = depth;
					Inspect.push(Element);
					state.grid[cLoc.row][cLoc.col].history = 0;
					fprintf(out," ----- history: %d ", state.grid[cLoc.row][cLoc.col].history);
					fprintf(out," ----- added -> step %d",depth);
					fflush(out);
				}
				fprintf(out,"\n");
            }
        }

		fprintf(out,"Furnica muta : %d", direction);
        state.makeMove( sLoc, direction);

    }
    fclose(out);
}

//explore map with unused ants
void Bot::exploreMap()
{
    std::queue<Location> antQueue;
    Location sLoc, cLoc, nLoc;
    for( unsigned int i = 0; i < state.used.size(); ++i )
    {
        sLoc = state.myAnts[i];
        antQueue.push( sLoc );
        int sDir, oldestVisit = 0;

        std::vector<std::vector<bool> > visited(state.rows, std::vector<bool>(state.cols, 0));
        visited[sLoc.row][sLoc.col] = 1;
        bool exitBfs = false;

        //find directions for each ant
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

        //this is just a temporary solution for exiting the loop and move
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

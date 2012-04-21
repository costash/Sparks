#include "Bot.h"
#include "BfsQueueElement.h"
#include <time.h>

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

	state.newTurn();
	state.updateHistory();
	state.setReachableTiles();
	exploreFood();
	exploreMap();
	sendToBorder();

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
                    int antDir = (d + 2) % 4;	// direction to go

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
}

void Bot::exploreMap()
{
	FILE *out  = fopen("z_moves.txt","a");
	int MAX, SUM, depth, steps = 11, direction = 0;
	State::explore Element;
    queue< State::explore > Inspect;
    Location sLoc, cLoc, nLoc;

    for( unsigned int i = 0; i < state.used.size(); i++ )
    {
    	if (state.used[i] == true)
			continue;

		state.used[i] = true;

		MAX = 0;
        sLoc = state.myAnts[i];

//		fprintf(out,"Furnica %d - coord x=%d , y=%d \t", i, sLoc.row, sLoc.col);
        Element.depth = 1;

        vector<vector<bool> > visited (state.rows, vector<bool>(state.cols, 0));
		visited[sLoc.row][sLoc.col] = true;

		/**	Add the main directions N, S, E, W for starting the BFS explore
		 **/
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


        while (!Inspect.empty())
        {

			Element = Inspect.front();
			cLoc = Element.loc;
			depth = Element.depth;
			Inspect.pop();

//			fprintf(out,"Element: ");
//			fprintf(out,"\t Coord: x=%d \t y=%d\n", cLoc.row , cLoc.col);
//			fflush(out);

			depth++;

			/**	If maximum number of steps was reached we look for the last
			 ** visited tile on the 11th step
			 **/
			if ( depth == steps)
			{
				SUM = 0;
				for( int d = 0; d <4; d++ )
				{
					nLoc = state.getLocation(cLoc, d);
					if (state.grid[nLoc.row][nLoc.col].isWater == false &&
						visited[nLoc.row][nLoc.col] == false)
					{
						SUM += state.grid[nLoc.row][nLoc.col].history;
						state.grid[nLoc.row][nLoc.col].history = 0;
					}
				}

//				fprintf(out,"Suma : %d \t Coordonate: x=%d \t y=%d \n",SUM, cLoc.row, cLoc.col);
				if (SUM >= MAX)
				{
					direction = Element.direction;
					MAX = SUM;
				}
			}

			else {
				for( int d=0; d < 4; ++d )
				{
					nLoc = state.getLocation( cLoc, d );
	//				fprintf(out,"Coordonate: x=%d \t y=%d \t", nLoc.row, nLoc.col);
	//				fprintf(out,"------ vizitat: %d ",(int)visited[nLoc.row][nLoc.col]);

					if (state.grid[nLoc.row][nLoc.col].isWater == false &&
						visited[nLoc.row][nLoc.col] == false)
					{
						visited[nLoc.row][nLoc.col] = true ;
						Element.loc = nLoc;
						Element.depth = depth;
						Inspect.push(Element);
	//					state.grid[cLoc.row][cLoc.col].history = 0;
	//					fprintf(out," ----- history: %d ", state.grid[cLoc.row][cLoc.col].history);
	//					fprintf(out," ----- added -> step %d",depth);
	//					fflush(out);
					}
	//				fprintf(out,"\n");
				}
			}
        }

		fprintf(out,"Furnica %d muta : %d \n", i, direction);
		fflush(out);

		nLoc = state.getLocation(sLoc, direction);
		if (state.grid[nLoc.row][nLoc.col].ant != 0) {
			state.makeMove( sLoc, direction);
		}
		else {
			int start = 0;
			for (int k = start; k < start+4; k++) {
				int d = k % 4;
				if (d == direction ) continue;
					nLoc = state.getLocation(sLoc, d);
					if (state.grid[nLoc.row][nLoc.col].ant != 0 &&
						state.grid[nLoc.row][nLoc.col].isWater == false &&
						state.grid[nLoc.row][nLoc.col].isHill == false) {
						state.makeMove( sLoc, d);
						break;
					}
			}
		}

    }
    fclose(out);
}


void Bot::sendToBorder()
{
	int i, j;
	int steps = 11;
    Location sLoc, cLoc, nLoc;
    queue < State::expBorder > findBorder;
	State::expBorder Element;
	FILE *out  = fopen("z_border.txt", "w");
	out  = fopen("z_border.txt", "a");

	/**	Set the visited matrix to 0
	 **/
	int visited[state.rows][state.cols];

	for (i=0; i<state.rows; i++)
		memset(visited[i], 0, state.cols * sizeof(int));

	Element.depth = 0;
	Element.direction = -1;

	/**	Add all Enemy Ants in the queue
	 **/
	if (state.enemyAnts.size() == 0)
		return;

	for (unsigned i=0; i<state.enemyAnts.size(); i++)
	{
		Element.loc = state.enemyAnts[i];
		Element.antType = 2;
		visited[Element.loc.row][Element.loc.col] = 2;
		findBorder.push(Element);
	}

	/**	Add all my Ants in the queue
	 **/
	for (unsigned i=0; i<state.myAnts.size(); i++)
	{
		Element.loc = state.myAnts[i];
		Element.antType = 1;
		Element.antIndex = i;
		visited[Element.loc.row][Element.loc.col] = 1;
		findBorder.push(Element);
	}

	fprintf(out,"Enemy ants: %d\n", state.enemyAnts.size());
	fprintf(out,"My ants: %d\n", state.myAnts.size());
	fprintf(out,"Queue size: %d\n", findBorder.size());
	fflush(out);

	while (!findBorder.empty()) {
		Element = findBorder.front();
		findBorder.pop();

		if (Element.depth == steps)
			continue;

		Element.depth++;
		for( int d=0; d < 4; d++ )
		{
			nLoc = state.getLocation( Element.loc, d );

			if (Element.depth == 1)
				Element.direction = d;


			if (state.grid[nLoc.row][nLoc.col].isWater == false &&
				visited[nLoc.row][nLoc.col] != Element.antType &&
				visited[nLoc.row][nLoc.col] != 3)
			{
				visited[nLoc.row][nLoc.col] += Element.antType;
				if (visited[nLoc.row][nLoc.col] != 3)
				{
					Element.loc = nLoc;
					findBorder.push(Element);
				}
			}

			if (visited[nLoc.row][nLoc.col] == 3 &&  Element.antType == 1 &&
				state.used[Element.antIndex] == false)
			{
				state.used[Element.antIndex] = true;
				fprintf(out," Trying to move ant %d in direction %d\n", Element.antIndex, Element.direction);
				fflush(out);
				nLoc = state.getLocation( state.myAnts[Element.antIndex], Element.direction );
				Square tile = state.grid[nLoc.row][nLoc.col];
				if (tile.isWater == false)
					state.makeMove( state.myAnts[Element.antIndex], Element.direction);
			}
		}
	}

	fprintf(out, "Turn %d \n\n\n", state.turn);
	for (i=0; i<state.rows; i++) {
		for (j=0; j<state.cols; j++) {
			if (state.grid[i][j].ant == 0)
				fprintf(out, "# ");
			else
				fprintf(out, "%d ", visited[i][j]);
		}
		fprintf(out, "\n");
	}
	fflush(out);
}


//finishes the turn
void Bot::endTurn()
{
    if(state.turn > 0)
        state.reset();
    state.turn++;

    cout << "go" << endl;
};

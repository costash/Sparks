#include "Bot.h"
#include "BfsQueueElement.h"
#include <time.h>
#include <string.h>

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
	getToBorder();
	exploreMap();
//	sendToBorder();

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
	int MAX, SUM, depth, steps = 12, direction = 0;
	State::explore Element;
    queue< State::explore > Inspect;
    Location sLoc, cLoc, nLoc;

    for( unsigned int i = 0; i < state.used.size(); i++ )
    {
    	if (state.used[i] == true)
			continue;

		MAX = 0;
        sLoc = state.myAnts[i];
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
					if (state.grid[nLoc.row][nLoc.col].isWater == false &&
						visited[nLoc.row][nLoc.col] == false)
					{
						visited[nLoc.row][nLoc.col] = true ;
						Element.loc = nLoc;
						Element.depth = depth;
						Inspect.push(Element);
					}
				}
			}
        }

		nLoc = state.getLocation(sLoc, direction);
		if (state.grid[nLoc.row][nLoc.col].ant != 0) {
			state.used[i] = true;
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


void Bot::getToBorder()
{
	FILE *outB  = fopen("z_border.txt", "a");
	int steps = 12;
    Location nLoc;
    queue < State::expBorder > findBorder;
	State::expBorder Element;
	Element.depth = 0;

	/**	Add all my Ants in the queue
	 **/
	for (unsigned int i=0; i<state.myAnts.size(); i++)
	{
		Element.loc = state.myAnts[i];
		Element.antType = 1;
		Element.antIndex = i;
		state.grid[Element.loc.row][Element.loc.col].border = 1;
		findBorder.push(Element);
	}

	/**	Add all Enemy Ants in the queue
	 **/

	for (unsigned int i=0; i<state.enemyAnts.size(); i++)
	{
		Element.loc = state.enemyAnts[i];
		Element.antType = 2;
		state.grid[Element.loc.row][Element.loc.col].border = 2;
		findBorder.push(Element);
	}

	fprintf(outB,"Enemy ants: %d\n", state.enemyAnts.size());
	fprintf(outB,"My ants: %d\n", state.myAnts.size());
	fprintf(outB,"Queue size: %d\n", findBorder.size());
	fflush(outB);

	while (!findBorder.empty())
	{
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
				state.grid[nLoc.row][nLoc.col].border != Element.antType &&
				state.grid[nLoc.row][nLoc.col].border != 3)
			{
				if (Element.antType == 1) {
					switch (state.grid[nLoc.row][nLoc.col].border)
					{
						case 0:
							if (Element.depth == steps)
								state.grid[nLoc.row][nLoc.col].border = 4;
							else
								state.grid[nLoc.row][nLoc.col].border = 1;
							break;
						case 1:
							break;
						case 2:
							state.grid[nLoc.row][nLoc.col].border = 3;
							break;
						case 4:
							state.grid[nLoc.row][nLoc.col].border = 1;
							break;
						default:
							break;
					}
				}
				else
				{
					if (state.grid[nLoc.row][nLoc.col].border == 1)
						continue;
					state.grid[nLoc.row][nLoc.col].border = 2;
				}

				if (state.grid[nLoc.row][nLoc.col].border != 3)
				{
					Element.loc = nLoc;
					findBorder.push(Element);
				}
			}

			if (Element.antType == 1 &&
				state.grid[nLoc.row][nLoc.col].border == 3 &&
				state.used[Element.antIndex] == false )
			{
				state.used[Element.antIndex] = true;
				fprintf(outB," Trying to move ant %d in direction %d\n", Element.antIndex, Element.direction);
				fflush(outB);
				nLoc = state.getLocation( state.myAnts[Element.antIndex], Element.direction );
				Square tile = state.grid[nLoc.row][nLoc.col];
				if (tile.isWater == false)
					state.makeMove( state.myAnts[Element.antIndex], Element.direction);
			}
		}
	}
	state.printBorders();
}

void Bot::sendToBorder()
{
	FILE *out = fopen("z_border.txt","a");
    Location nLoc;
    queue < State::explore > findBorder;
	State::explore Element;

	int visited[state.rows][state.cols];

	/**	Add all my Ants in the queue
	 **/
	for (unsigned int i=0; i<state.myAnts.size(); i++)
	{
		if (state.used[i] == true )
			continue;

		nLoc = state.myAnts[i];
		fprintf(out," Furnica %d - pozitie %d - %d este: %d\n", i, nLoc.row, nLoc.col, (int)state.used[i]);
		fflush(out);

		for (int k=0; k<state.rows; k++)
			memset(visited[k], 0, state.cols * sizeof(int));

		int ok = 1;
		Element.depth = 0;
		Element.loc = state.myAnts[i];
		visited[Element.loc.row][Element.loc.col] = 1;
		findBorder.push(Element);

		while (!findBorder.empty() && ok)
		{
			Element = findBorder.front();
			findBorder.pop();
			Element.depth++;

			for(int d=0; d < 4 && ok; d++ )
			{
				nLoc = state.getLocation( Element.loc, d );

				if (Element.depth == 1)
					Element.direction = d;

				if (state.grid[nLoc.row][nLoc.col].isWater == false &&
					visited[nLoc.row][nLoc.col] == 0)
				{
					visited[nLoc.row][nLoc.col] = 1;
					if (state.grid[nLoc.row][nLoc.col].border < 3)
					{
						Element.loc = nLoc;
						findBorder.push(Element);
					}

					if (state.grid[nLoc.row][nLoc.col].border >= 3)
					{
						visited[nLoc.row][nLoc.col] = 3;
						state.used[i] = true;
						nLoc = state.getLocation( state.myAnts[i], Element.direction );
						Square tile = state.grid[nLoc.row][nLoc.col];
						if (tile.isWater == false)
							state.makeMove( state.myAnts[i], Element.direction);
						fprintf(out," Mutata spre %d\n", Element.direction);
						ok = 0;
					}
				}
			}
		}

		int i, j;
		fprintf(out, "Turn %d \n\n\n", state.turn);
		for (i=0; i<state.rows; i++) {
			for (j=0; j<state.cols; j++) {
				if (state.grid[i][j].ant == 0)
					fprintf(out, " A ");
				else
					fprintf(out, "%d ", visited[i][j]);
			}
			fprintf(out, "\n");
		}
		fflush(out);

	}
}

void Bot::findBorders() {
	int steps = 12;
    Location nLoc;
    queue < State::explore > findBorder;
	State::explore Element;
	Element.depth = 0;

	/**	Add all my Ants in the queue
	 **/
	for (unsigned int i=0; i<state.myAnts.size(); i++)
	{
		Element.loc = state.myAnts[i];
		state.grid[Element.loc.row][Element.loc.col].border = 1;
		findBorder.push(Element);
	}


	while (!findBorder.empty()) {
		Element = findBorder.front();
		findBorder.pop();
		Element.depth++;

		if (Element.depth == steps)
			return;

		for( int d=0; d<TDIRECTIONS; d++)
		{
			nLoc = state.getLocation( Element.loc, d );

			if (state.grid[nLoc.row][nLoc.col].isWater == false &&
				state.grid[nLoc.row][nLoc.col].border == 0)
			{
				state.grid[nLoc.row][nLoc.col].border = 1;
				Element.loc = nLoc;
				findBorder.push(Element);
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

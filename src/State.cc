#include "State.h"

using namespace std;

//constructor
State::State()
{
	gameover = 0;
	turn = 0;
	bug.open("z_debug.txt");
};

//deconstructor
State::~State()
{
	bug.close();
};

//sets the state up
void State::setup()
{
	grid = vector<vector<Square> >(rows, vector<Square>(cols, Square()));

	/** Initialize the incremental history for every tile **/
	initHistory();

	/** File Debugging clear **/
	#ifdef DEBUG
	FILE *out  = fopen("z_info.txt","w");
	fclose(out);
	#endif
};

//resets all non-water squares to land and clears the bots ant vector
void State::reset()
{
	myAnts.clear();
	enemyAnts.clear();
	myHills.clear();
	enemyHills.clear();
	food.clear();
	for(int row=0; row<rows; row++)
		for(int col=0; col<cols; col++)
			if(!grid[row][col].isWater)
				grid[row][col].reset();
};


/*******************************************************************************
 ---------------------------- Our Code IS DOWN ---------------------------------
 *******************************************************************************/


void State::initHistory() {
	for( int i = 0; i < rows; ++i )
	{
		for( int j = 0; j < cols; ++j )
		{
			grid[i][j].history=0;
		}
	}

	#ifdef DEBUG
	FILE *out  = fopen("moves.txt","a");
	fprintf(out,"\n TURN : %d\n", turn);
	fclose(out);
	#endif
}


void State::setReachableTiles()
{
	for ( int i = 0; i < rows; i++)
		for ( int j = 0; j < cols; j++)
			if (grid[i][j].isVisible)
					grid[i][j].history = 0;
}


void State::updateHistory() {
	for( int i = 0; i < rows; ++i )
		for( int j = 0; j < cols; ++j ) {
				if(grid[i][j].isWater == false)
					grid[i][j].history++;
				grid[i][j].border=0;
		}
}


// * Useless
void State::visibleBorder()
{
	for( int i = 0; i < rows; ++i )
	{
		for( int j = 0; j < cols; ++j )
		{
			if (grid[i][j].isWater) {
				grid[i][j].info = 7;
				continue;
			}

			if (grid[i][j].isFood) {
				grid[i][j].info = 3;
				continue;
			}

			if (grid[i][j].isHill) {
				grid[i][j].info = 9;
				continue;
			}

			if (grid[i][j].indexAnt >=0) {
				grid[i][j].info = 5;
				continue;
			}

			if (grid[i][j].isVisible) {
				grid[i][j].info = 1;
				continue;
			}

			grid[i][j].info = 0;
		}
	}

//	printInfo();
}


void State::printInfo()
{
	FILE *out = fopen("z_info.txt","w");

	fprintf(out,"\n");
	fprintf(out,"Turn %d\n", turn);
	for( int i = 0; i < rows; ++i )
	{
		fprintf(out,"\n");
		for( int j = 0; j < cols; ++j )
			fprintf(out,"%d ", grid[i][j].info);
	}
	fflush(out);
	fclose(out);
}


void State::newTurn()
{
	used.assign( myAnts.size(), 0 );	// init with 0 the used vector
	updateHistory();
	setReachableTiles();
}

void State::printHistory()
{
	FILE *out = fopen("z_map.txt","w");

	for( int i = 0; i < rows; ++i )
	{
		fprintf(out,"\n");
		for( int j = 0; j < cols; ++j )
		{
			if (grid[i][j].ant == 0)
				fprintf(out,"A");
			else
			if (grid[i][j].isWater == true)
				fprintf(out,"W");
			else
			if (grid[i][j].isFood == true)
				fprintf(out,"F");
			else
			if (grid[i][j].isHill == true)
				fprintf(out,"M");
			else
				if(grid[i][j].history > 10)
				fprintf(out,"U");
				else
				fprintf(out,"%d",grid[i][j].history);
				fprintf(out," ");
		}
	}
	fflush(out);
	fclose(out);
}

void State::printBorders()
{
	FILE *outB  = fopen("z_border.txt", "a");

	int i, j;
	fprintf(outB, "Turn %d \n\n\n", turn);
	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < cols; j++)
		{
			if (grid[i][j].ant == 0)
				fprintf(outB, "A ");
			else
				fprintf(outB, "%d ", grid[i][j].border);
		}
		fprintf(outB, "\n");
	}
	fflush(outB);
	fclose(outB);

}


/*******************************************************************************
 ----------------------------- Our Code IS UP ----------------------------------
 *******************************************************************************/


//update lastVisit information for squares
void State::updateLastVisit()
{
	for(unsigned int i = 0; i < grid.size(); ++i )
		for(unsigned int j = 0; j < grid[i].size(); ++j )
		{
			if( grid[i][j].ant == 0 )
				grid[i][j].lastVisit = 0;
			else if( grid[i][j].lastVisit != -1 )
				grid[i][j].lastVisit++;
		}
}

//outputs move information to the engine
void State::makeMove(const Location &loc, int direction)
{
	cout << "o " << loc.row << " " << loc.col << " " << CDIRECTIONS[direction] << endl;

	Location nLoc = getLocation(loc, direction);
	grid[nLoc.row][nLoc.col].ant = grid[loc.row][loc.col].ant;
	grid[loc.row][loc.col].ant = -1;
};

//returns the euclidean distance between two locations with the edges wrapped
double State::distance(const Location &loc1, const Location &loc2)
{
	int d1 = abs(loc1.row-loc2.row),
		d2 = abs(loc1.col-loc2.col),
		dr = min(d1, rows-d1),
		dc = min(d2, cols-d2);
	return sqrt(dr*dr + dc*dc);
};

//returns the new location from moving in a given direction with the edges wrapped
Location State::getLocation(const Location &loc, int direction)
{
	return Location( (loc.row + DIRECTIONS[direction][0] + rows) % rows,
					 (loc.col + DIRECTIONS[direction][1] + cols) % cols );
};

/*
	This function will update update the lastSeen value for any squares currently
	visible by one of your live ants.

	BE VERY CAREFUL IF YOU ARE GOING TO TRY AND MAKE THIS FUNCTION MORE EFFICIENT,
	THE OBVIOUS WAY OF TRYING TO IMPROVE IT BREAKS USING THE EUCLIDEAN METRIC, FOR
	A CORRECT MORE EFFICIENT IMPLEMENTATION, TAKE A LOOK AT THE GET_VISION FUNCTION
	IN ANTS.PY ON THE CONTESTS GITHUB PAGE.
*/
void State::updateVisionInformation()
{
	std::queue<Location> locQueue;
	Location sLoc, cLoc, nLoc;

	for(int a=0; a<(int) myAnts.size(); a++)
	{
		sLoc = myAnts[a];
		locQueue.push(sLoc);

		std::vector<std::vector<bool> > visited(rows, std::vector<bool>(cols, 0));
		grid[sLoc.row][sLoc.col].isVisible = 1;
		visited[sLoc.row][sLoc.col] = 1;

		while(!locQueue.empty())
		{
			cLoc = locQueue.front();
			locQueue.pop();

			for(int d=0; d<TDIRECTIONS; d++)
			{
				nLoc = getLocation(cLoc, d);

				if(!visited[nLoc.row][nLoc.col] && distance(sLoc, nLoc) <= viewradius)
				{
					grid[nLoc.row][nLoc.col].isVisible = 1;
					locQueue.push(nLoc);
				}
				visited[nLoc.row][nLoc.col] = 1;
			}
		}
	}
};

/*
	This is the output function for a state. It will add a char map
	representation of the state to the output stream passed to it.

	For example, you might call "cout << state << endl;"
*/
ostream& operator<<(ostream &os, const State &state)
{
	for(int row=0; row<state.rows; row++)
	{
		for(int col=0; col<state.cols; col++)
		{
			if(state.grid[row][col].isWater)
				os << '%';
			else if(state.grid[row][col].isFood)
				os << '*';
			else if(state.grid[row][col].isHill)
				os << (char)('A' + state.grid[row][col].hillPlayer);
			else if(state.grid[row][col].ant >= 0)
				os << (char)('a' + state.grid[row][col].ant);
			else if(state.grid[row][col].isVisible)
				os << '.';
			else
				os << '?';
		}
		os << endl;
	}

	return os;
};

//input function
istream& operator>>(istream &is, State &state)
{
	int row, col, player;
	string inputType, junk;

	//finds out which turn it is
	while(is >> inputType)
	{
		if(inputType == "end")
		{
			state.gameover = 1;
			break;
		}
		else if(inputType == "turn")
		{
			is >> state.turn;
			break;
		}
		else //unknown line
			getline(is, junk);
	}

	if(state.turn == 0)
	{
		//reads game parameters
		while(is >> inputType)
		{
			if(inputType == "loadtime")
				is >> state.loadtime;
			else if(inputType == "turntime")
				is >> state.turntime;
			else if(inputType == "rows")
				is >> state.rows;
			else if(inputType == "cols")
				is >> state.cols;
			else if(inputType == "turns")
				is >> state.turns;
			else if(inputType == "player_seed")
				is >> state.seed;
			else if(inputType == "viewradius2")
			{
				is >> state.viewradius;
				state.viewradius = sqrt(state.viewradius);
			}
			else if(inputType == "attackradius2")
			{
				is >> state.attackradius;
				state.attackradius = sqrt(state.attackradius);
			}
			else if(inputType == "spawnradius2")
			{
				is >> state.spawnradius;
				state.spawnradius = sqrt(state.spawnradius);
			}
			else if(inputType == "ready") //end of parameter input
			{
				state.timer.start();
				break;
			}
			else	//unknown line
				getline(is, junk);
		}
	}
	else
	{
		//reads information about the current turn
		while(is >> inputType)
		{
			if(inputType == "w") //water square
			{
				is >> row >> col;
				state.grid[row][col].isWater = 1;
			}
			else if(inputType == "f") //food square
			{
				is >> row >> col;
				state.grid[row][col].isFood = 1;
				state.food.push_back(Location(row, col));
			}
			else if(inputType == "a") //live ant square
			{
				is >> row >> col >> player;
				state.grid[row][col].ant = player;
				if(player == 0)
				{
					state.myAnts.push_back(Location(row, col));
					state.grid[row][col].indexAnt = state.myAnts.size() - 1;
				}
				else
					state.enemyAnts.push_back(Location(row, col));
			}
			else if(inputType == "d") //dead ant square
			{
				is >> row >> col >> player;
				state.grid[row][col].deadAnts.push_back(player);
			}
			else if(inputType == "h")
			{
				is >> row >> col >> player;
				state.grid[row][col].isHill = 1;
				state.grid[row][col].hillPlayer = player;
				if(player == 0)
					state.myHills.push_back(Location(row, col));
				else
					state.enemyHills.push_back(Location(row, col));

			}
			else if(inputType == "players") //player information
				is >> state.noPlayers;
			else if(inputType == "scores") //score information
			{
				state.scores = vector<double>(state.noPlayers, 0.0);
				for(int p=0; p<state.noPlayers; p++)
					is >> state.scores[p];
			}
			else if(inputType == "go") //end of turn input
			{
				if(state.gameover)
					is.setstate(std::ios::failbit);
				else
					state.timer.start();
				break;
			}
			else //unknown line
				getline(is, junk);
		}
	}

	return is;
};

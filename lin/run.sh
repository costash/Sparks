#!/bin/bash

args=("$@")

echo ${args[0]} ${args[1]}

if [ ${args[0]} -eq "1" ]; then
	./playgame.py --player_seed 0 --engine_seed 42 --end_wait=0.25 --verbose --log_dir game_logs --turns ${args[1]} --map_file maps/cell_maze/cell_maze_p02_10.map "./bot32" "./MyBot"
	
else
	if [ ${args[0]} -eq "2" ]; then
		./playgame.py --player_seed 0 --engine_seed 42 --end_wait=0.25 --verbose --log_dir game_logs --turns ${args[1]} --map_file maps/cell_maze/cell_maze_p02_15.map "./bot32" "./MyBot"
	else
		if [ ${args[0]} -eq "3" ]; then
			./playgame.py --player_seed 0 --engine_seed 42 --end_wait=0.25 --verbose --log_dir game_logs --turns ${args[1]} --map_file maps/maze/maze_p02_14.map "./bot32" "./MyBot"
		else
			if [ ${args[0]} -eq "4" ]; then
				./playgame.py --player_seed 0 --engine_seed 42 --end_wait=0.25 --verbose --log_dir game_logs --turns ${args[1]} --map_file maps/random_walk/random_walk_p02_10.map "./bot32" "./MyBot"
			fi
		fi
	fi
fi

#./playgame.py --player_seed 0 --engine_seed 42 --end_wait=0.25 --verbose --log_dir game_logs --turns 1000 --map_file maps/cell_maze/cell_maze_p02_10.map "$@" "./bot32" "./MyBot"

#!/usr/bin/env sh
./playgame.py --player_seed 0 --engine_seed 42 --end_wait=0.25 --verbose --log_dir game_logs --turns 1000 --map_file maps/cell_maze/cell_maze_p02_19.map "$@" "./bot32" "./MyBot"

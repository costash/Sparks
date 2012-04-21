cls
if ($args[1] -eq 1) {
	$map = ".\maps\cell_maze\cell_maze_p02_10.map";
}
if ($args[1] -eq 2) {
	$map = ".\maps\cell_maze\cell_maze_p02_15.map";
}
if ($args[1] -eq 3) {
	$map = ".\maps\maze\maze_p02_14.map";
}
if ($args[1] -eq 4) {
	$map = ".\maps\random_walk\random_walk_p02_10.map";
}

python "playgame.py"  --player_seed 0 --engine_seed 42 --end_wait=0.25 --verbose --log_dir game_logs --turns $args[0] --map_file $map "bot" "Sparks"

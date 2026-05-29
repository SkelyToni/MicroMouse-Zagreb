#ifndef FLOODFILL_H
#define FLOODFILL_H

#include "maze.h"

// Prima 'true' ako se vraća na start (0,0), ili 'false' ako ide prema centru
void flood_update(bool to_start);

int flood_get_distance(int x, int y);

Direction flood_get_best_direction(int x, int y, Direction current_dir);

#endif
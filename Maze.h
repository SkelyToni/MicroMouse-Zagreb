#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>

#define MAZE_SIZE 16

#define WALL_NORTH (1 << 0)
#define WALL_EAST  (1 << 1)
#define WALL_SOUTH (1 << 2)
#define WALL_WEST  (1 << 3)
#define VISITED    (1 << 4)

typedef enum {
    NORTH = 0,
    EAST  = 1,
    SOUTH = 2,
    WEST  = 3
} Direction;

void maze_init(void);
void maze_scan_and_map(int x, int y, Direction current_dir);
bool maze_has_wall(int x, int y, Direction dir);
bool maze_is_visited(int x, int y); // <-- NOVO: Provjera je li polje posjećeno

#endif
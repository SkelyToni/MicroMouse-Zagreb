#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>

#define MAZE_SIZE 16

// Bitmaske za zidove i posjećenost
#define WALL_NORTH (1 << 0) // 00000001
#define WALL_EAST  (1 << 1) // 00000010
#define WALL_SOUTH (1 << 2) // 00000100
#define WALL_WEST  (1 << 3) // 00001000
#define VISITED    (1 << 4) // 00010000

// Smjerovi orijentacije miša (u smjeru kazaljke na satu)
typedef enum {
    NORTH = 0,
    EAST  = 1,
    SOUTH = 2,
    WEST  = 3
} Direction;

// Javne funkcije dostupne ostatku programa
void maze_init(void);
void maze_scan_and_map(int x, int y, Direction current_dir);
bool maze_has_wall(int x, int y, Direction dir);

#endif // MAZE_H
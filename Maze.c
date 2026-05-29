#include "maze.h"
#include "API.h"

static unsigned char maze[MAZE_SIZE][MAZE_SIZE];

static unsigned char get_wall_mask(Direction dir) {
    switch (dir) {
        case NORTH: return WALL_NORTH;
        case EAST:  return WALL_EAST;
        case SOUTH: return WALL_SOUTH;
        case WEST:  return WALL_WEST;
    }
    return 0;
}

void maze_init(void) {
    for (int x = 0; x < MAZE_SIZE; x++) {
        for (int y = 0; y < MAZE_SIZE; y++) {
            maze[x][y] = 0;
            if (y == MAZE_SIZE - 1) maze[x][y] |= WALL_NORTH;
            if (x == MAZE_SIZE - 1) maze[x][y] |= WALL_EAST;
            if (y == 0)             maze[x][y] |= WALL_SOUTH;
            if (x == 0)             maze[x][y] |= WALL_WEST;
        }
    }
}

static void set_wall_bidirectional(int x, int y, Direction dir) {
    maze[x][y] |= get_wall_mask(dir);
    API_setWall(x, y, "nesw"[dir]);

    int nx = x, ny = y;
    Direction opposite_dir = (dir + 2) % 4;

    if (dir == NORTH) ny++;
    else if (dir == EAST) nx++;
    else if (dir == SOUTH) ny--;
    else if (dir == WEST) nx--;

    if (nx >= 0 && nx < MAZE_SIZE && ny >= 0 && ny < MAZE_SIZE) {
        maze[nx][ny] |= get_wall_mask(opposite_dir);
        API_setWall(nx, ny, "nesw"[opposite_dir]);
    }
}

void maze_scan_and_map(int x, int y, Direction current_dir) {
    // Označi trenutno polje kao posjećeno
    maze[x][y] |= VISITED;

    if (API_wallFront())  set_wall_bidirectional(x, y, current_dir);
    
    Direction left_dir = (current_dir + 3) % 4;
    if (API_wallLeft())   set_wall_bidirectional(x, y, left_dir);
    
    Direction right_dir = (current_dir + 1) % 4;
    if (API_wallRight())  set_wall_bidirectional(x, y, right_dir);
}

bool maze_has_wall(int x, int y, Direction dir) {
    return (maze[x][y] & get_wall_mask(dir)) != 0;
}

// Vraća true ako je polje posjećeno (unutar granica)
bool maze_is_visited(int x, int y) {
    if (x < 0 || x >= MAZE_SIZE || y < 0 || y >= MAZE_SIZE) return true; // Izvan labirinta tretiramo kao posjećeno
    return (maze[x][y] & VISITED) != 0;
}
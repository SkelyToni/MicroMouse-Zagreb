#include <stdio.h>
#include <stdbool.h>
#include "API.h"

#include <stdio.h>
#include <stdbool.h>
#include "API.h"

#define MAZE_SIZE 16

// Bitmask definitions for walls and states
#define WALL_NORTH (1 << 0) // 00000001 -> 1
#define WALL_EAST  (1 << 1) // 00000010 -> 2
#define WALL_SOUTH (1 << 2) // 00000100 -> 4
#define WALL_WEST  (1 << 3) // 00001000 -> 8
#define VISITED    (1 << 4) // 00010000 -> 16

// Heading directions (Clockwise order matters for easy turning math)
typedef enum {
    NORTH = 0,
    EAST  = 1,
    SOUTH = 2,
    WEST  = 3
} Direction;

// Internal memory representation of the maze
unsigned char maze[MAZE_SIZE][MAZE_SIZE];

// Initialize or reset the internal maze memory
void init_maze() {
    for (int x = 0; x < MAZE_SIZE; x++) {
        for (int y = 0; y < MAZE_SIZE; y++) {
            maze[x][y] = 0;
            
            // Pre-program known outer boundary walls
            if (y == MAZE_SIZE - 1) maze[x][y] |= WALL_NORTH;
            if (x == MAZE_SIZE - 1) maze[x][y] |= WALL_EAST;
            if (y == 0)             maze[x][y] |= WALL_SOUTH;
            if (x == 0)             maze[x][y] |= WALL_WEST;
        }
    }
}

// Convert absolute heading into its matching bitmask
unsigned char get_wall_mask(Direction dir) {
    switch (dir) {
        case NORTH: return WALL_NORTH;
        case EAST:  return WALL_EAST;
        case SOUTH: return WALL_SOUTH;
        case WEST:  return WALL_WEST;
    }
    return 0;
}

// Map the current cell based on simulator API readings
void scan_and_map_walls(int x, int y, Direction current_dir) {
    // Mark current cell as visited
    maze[x][y] |= VISITED;

    // Check front wall
    if (API_wallFront()) {
        maze[x][y] |= get_wall_mask(current_dir);
        API_setWall(x, y, "nesw"[current_dir]);
    }
    
    // Check left wall
    Direction left_dir = (current_dir + 3) % 4;
    if (API_wallLeft()) {
        maze[x][y] |= get_wall_mask(left_dir);
        API_setWall(x, y, "nesw"[left_dir]);
    }
    
    // Check right wall
    Direction right_dir = (current_dir + 1) % 4;
    if (API_wallRight()) {
        maze[x][y] |= get_wall_mask(right_dir);
        API_setWall(x, y, "nesw"[right_dir]);
    }
}

// Check if a wall exists in our internal memory
bool has_wall_internal(int x, int y, Direction dir) {
    return (maze[x][y] & get_wall_mask(dir)) != 0;
}

int main(int argc, char* argv[]) {
    // State Tracking
    int x = 0, y = 0;
    Direction current_dir = NORTH;

    init_maze();
    API_clearAllColor();
    API_clearAllText();
    API_setColor(0, 0, 'G');
    API_setText(0, 0, "Start");

    while (true) {
        // Handle simulator reset button
        if (API_wasReset()) {
            API_ackReset();
            x = 0; y = 0;
            current_dir = NORTH;
            init_maze();
            API_clearAllColor();
            API_clearAllText();
            API_setColor(0, 0, 'G');
            API_setText(0, 0, "Start");
        }

        // 1. Scan the environment and update our internal byte matrix
        scan_and_map_walls(x, y, current_dir);
        API_setColor(x, y, 'B'); // Visual breadcrumb trail

        // 2. Navigation Decision (Using our internal memory map)
        Direction left_dir = (current_dir + 3) % 4;
        Direction right_dir = (current_dir + 1) % 4;

        if (!has_wall_internal(x, y, left_dir)) {
            API_turnLeft();
            current_dir = left_dir;
            API_moveForward();
        } else if (!has_wall_internal(x, y, current_dir)) {
            API_moveForward();
        } else if (!has_wall_internal(x, y, right_dir)) {
            API_turnRight();
            current_dir = right_dir;
            API_moveForward();
        } else {
            API_turnRight();
            API_turnRight();
            current_dir = (current_dir + 2) % 4;
            API_moveForward();
        }

        // 3. Keep internal coordinate tracking synchronized
        switch (current_dir) {
            case NORTH: y++; break;
            case EAST:  x++; break;
            case SOUTH: y--; break;
            case WEST:  x--; break;
        }

        char text[6];
        snprintf(text, sizeof(text), "%d,%d", x, y);
        API_setText(x, y, text);
    }

    return 0;
}
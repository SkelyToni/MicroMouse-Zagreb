#ifndef MAZE_H
#define MAZE_H

#include <stdint.h>

#define MAZE_SIZE 16
#define MAX_DIST 255

// Wall bitmask for each cell
#define WALL_NORTH 0x01
#define WALL_EAST  0x02
#define WALL_SOUTH 0x04
#define WALL_WEST  0x08

// Directions
enum Direction { NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3 };

struct Position {
    uint8_t x;
    uint8_t y;
};

class Maze {
public:
    Maze();

    void reset();
    void setWall(uint8_t x, uint8_t y, uint8_t wallMask);
    bool hasWall(uint8_t x, uint8_t y, uint8_t wallMask) const;
    void floodFill(Position goal[], uint8_t goalCount);
    uint8_t getDistance(uint8_t x, uint8_t y) const;
    Direction bestDirection(uint8_t x, uint8_t y, Direction facing) const;

private:
    uint8_t walls[MAZE_SIZE][MAZE_SIZE];    // wall data per cell
    uint8_t dist[MAZE_SIZE][MAZE_SIZE];     // flood-fill distances
};

#endif

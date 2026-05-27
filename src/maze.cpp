#include "maze.h"
#include <string.h>

Maze::Maze() {
    reset();
}

void Maze::reset() {
    memset(walls, 0, sizeof(walls));
    memset(dist, MAX_DIST, sizeof(dist));

    // Set outer boundary walls
    for (uint8_t i = 0; i < MAZE_SIZE; i++) {
        walls[i][0]              |= WALL_SOUTH;
        walls[i][MAZE_SIZE - 1]  |= WALL_NORTH;
        walls[0][i]              |= WALL_WEST;
        walls[MAZE_SIZE - 1][i]  |= WALL_EAST;
    }
}

void Maze::setWall(uint8_t x, uint8_t y, uint8_t wallMask) {
    walls[x][y] |= wallMask;

    // Set the matching wall in the neighboring cell
    if ((wallMask & WALL_NORTH) && y < MAZE_SIZE - 1)
        walls[x][y + 1] |= WALL_SOUTH;
    if ((wallMask & WALL_SOUTH) && y > 0)
        walls[x][y - 1] |= WALL_NORTH;
    if ((wallMask & WALL_EAST) && x < MAZE_SIZE - 1)
        walls[x + 1][y] |= WALL_WEST;
    if ((wallMask & WALL_WEST) && x > 0)
        walls[x - 1][y] |= WALL_EAST;
}

bool Maze::hasWall(uint8_t x, uint8_t y, uint8_t wallMask) const {
    return (walls[x][y] & wallMask) != 0;
}

void Maze::floodFill(Position goal[], uint8_t goalCount) {
    // Simple BFS flood from goal cells outward
    memset(dist, MAX_DIST, sizeof(dist));

    // Queue (big enough for entire maze)
    Position queue[MAZE_SIZE * MAZE_SIZE];
    uint16_t head = 0, tail = 0;

    // Seed goal cells with distance 0
    for (uint8_t i = 0; i < goalCount; i++) {
        dist[goal[i].x][goal[i].y] = 0;
        queue[tail++] = goal[i];
    }

    // BFS
    while (head != tail) {
        Position cur = queue[head++];
        uint8_t d = dist[cur.x][cur.y] + 1;

        // Check all four neighbors
        struct { int8_t dx; int8_t dy; uint8_t wall; } neighbors[4] = {
            { 0,  1, WALL_NORTH},
            { 1,  0, WALL_EAST},
            { 0, -1, WALL_SOUTH},
            {-1,  0, WALL_WEST}
        };

        for (auto& n : neighbors) {
            // Skip if wall blocks this direction
            if (walls[cur.x][cur.y] & n.wall)
                continue;

            uint8_t nx = cur.x + n.dx;
            uint8_t ny = cur.y + n.dy;

            if (nx < MAZE_SIZE && ny < MAZE_SIZE && dist[nx][ny] > d) {
                dist[nx][ny] = d;
                queue[tail++] = {nx, ny};
            }
        }
    }
}

uint8_t Maze::getDistance(uint8_t x, uint8_t y) const {
    return dist[x][y];
}

Direction Maze::bestDirection(uint8_t x, uint8_t y, Direction facing) const {
    uint8_t bestDist = MAX_DIST;
    Direction bestDir = facing;

    struct { int8_t dx; int8_t dy; uint8_t wall; Direction dir; } neighbors[4] = {
        { 0,  1, WALL_NORTH, NORTH},
        { 1,  0, WALL_EAST,  EAST},
        { 0, -1, WALL_SOUTH, SOUTH},
        {-1,  0, WALL_WEST,  WEST}
    };

    for (auto& n : neighbors) {
        if (walls[x][y] & n.wall)
            continue;

        uint8_t nx = x + n.dx;
        uint8_t ny = y + n.dy;

        if (nx < MAZE_SIZE && ny < MAZE_SIZE && dist[nx][ny] < bestDist) {
            bestDist = dist[nx][ny];
            bestDir = n.dir;
        }
    }

    return bestDir;
}

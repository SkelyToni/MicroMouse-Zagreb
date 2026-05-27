#include "maze.h"
#include "sensors.h"
#include "motion.h"

#ifdef SIMULATOR
#include "api.h"
#include <cstdio>
#endif

static Maze maze;
static Position pos = {0, 0};
static Direction facing = NORTH;

// Goal: center of 16x16 maze (4 cells)
static Position goals[] = {{7, 7}, {7, 8}, {8, 7}, {8, 8}};
static const uint8_t GOAL_COUNT = 4;

// Map which absolute wall corresponds to the mouse's relative view
static uint8_t wallMaskForDirection(Direction dir) {
    switch (dir) {
        case NORTH: return WALL_NORTH;
        case EAST:  return WALL_EAST;
        case SOUTH: return WALL_SOUTH;
        case WEST:  return WALL_WEST;
    }
    return 0;
}

static Direction relativeToAbsolute(Direction facing, int offset) {
    return (Direction)(((int)facing + offset + 4) % 4);
}

// Read sensors and update the maze wall map
static void scanWalls() {
    if (Sensors::wallFront()) {
        maze.setWall(pos.x, pos.y, wallMaskForDirection(facing));
    }
    if (Sensors::wallLeft()) {
        maze.setWall(pos.x, pos.y, wallMaskForDirection(relativeToAbsolute(facing, 3)));
    }
    if (Sensors::wallRight()) {
        maze.setWall(pos.x, pos.y, wallMaskForDirection(relativeToAbsolute(facing, 1)));
    }
}

// Move one cell in the current facing direction
static void advance() {
    Motion::moveForward();
    switch (facing) {
        case NORTH: pos.y++; break;
        case EAST:  pos.x++; break;
        case SOUTH: pos.y--; break;
        case WEST:  pos.x--; break;
    }
}

static bool atGoal() {
    for (uint8_t i = 0; i < GOAL_COUNT; i++) {
        if (pos.x == goals[i].x && pos.y == goals[i].y)
            return true;
    }
    return false;
}

void explore() {
    while (!atGoal()) {
        // 1. Scan walls at current cell
        scanWalls();

        // 2. Re-flood the maze toward the goal
        maze.floodFill(goals, GOAL_COUNT);

        #ifdef SIMULATOR
        // Show distances in the simulator for debugging
        for (int x = 0; x < MAZE_SIZE; x++) {
            for (int y = 0; y < MAZE_SIZE; y++) {
                char buf[4];
                snprintf(buf, sizeof(buf), "%d", maze.getDistance(x, y));
                API::setText(x, y, buf);
            }
        }
        API::setColor(pos.x, pos.y, 'G');
        #endif

        // 3. Pick the best direction and go
        Direction best = maze.bestDirection(pos.x, pos.y, facing);
        Motion::face(best, facing);
        advance();
    }
}

#ifdef SIMULATOR
int main() {
    Sensors::init();
    Motion::init();

    // Phase 1: Explore to the center
    explore();

    API::setText(pos.x, pos.y, "GOAL");
    API::setColor(pos.x, pos.y, 'Y');

    // TODO Phase 2: Return to start (reverse flood-fill)
    // TODO Phase 3: Speed run on known path

    return 0;
}

#else
#include <Arduino.h>

void setup() {
    Serial.begin(115200);
    Sensors::init();
    Motion::init();

    // Wait for a button press or delay before starting
    delay(3000);

    explore();
}

void loop() {
    // Nothing after exploration for now
}
#endif

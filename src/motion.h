#ifndef MOTION_H
#define MOTION_H

#include "maze.h"

namespace Motion {
    void init();

    // High-level commands (one cell at a time)
    void moveForward();
    void turnLeft();
    void turnRight();
    void turnAround();

    // Turn to face a specific direction
    void face(Direction target, Direction& current);

    // PID tuning (no-op in simulator)
    void setPID(float kp, float ki, float kd);
}

#endif

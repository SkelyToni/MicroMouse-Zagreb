#ifndef API_H
#define API_H

// Interface to the mms simulator (github.com/mackorone/mms)
// All functions communicate via stdin/stdout text protocol

namespace API {
    // Maze dimensions
    int mazeWidth();
    int mazeHeight();

    // Wall queries
    bool wallFront();
    bool wallRight();
    bool wallLeft();

    // Movement
    void moveForward(int distance = 1);
    void turnRight();
    void turnLeft();
    void turnAround();

    // Display (for debugging - colors cells/text in the simulator)
    void setWall(int x, int y, char direction);
    void clearWall(int x, int y, char direction);
    void setColor(int x, int y, char color);
    void clearColor(int x, int y);
    void clearAllColor();
    void setText(int x, int y, const char* text);
    void clearText(int x, int y);
    void clearAllText();
    bool wasReset();
    void ackReset();
}

#endif

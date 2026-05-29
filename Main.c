#include <stdio.h>
#include <stdbool.h>
#include "API.h"
#include "Maze.h" 

int main(int argc, char* argv[]) {
    int x = 0, y = 0;
    Direction current_dir = NORTH;

    maze_init();
    API_clearAllColor();
    API_clearAllText();
    API_setColor(0, 0, 'G');
    API_setText(0, 0, "Start");

    while (true) {
        if (API_wasReset()) {
            API_ackReset();
            x = 0; y = 0;
            current_dir = NORTH;
            maze_init();
            API_clearAllColor();
            API_clearAllText();
            API_setColor(0, 0, 'G');
            API_setText(0, 0, "Start");
        }

        // 1. Skeniranje okoline kroz novi modul
        maze_scan_and_map(x, y, current_dir);
        API_setColor(x, y, 'B'); 

        // 2. Donošenje odluke o kretanju (koristeći maze_has_wall)
        Direction left_dir = (current_dir + 3) % 4;
        Direction right_dir = (current_dir + 1) % 4;

        if (!maze_has_wall(x, y, left_dir)) {
            API_turnLeft();
            current_dir = left_dir;
        } else if (!maze_has_wall(x, y, current_dir)) {
            // Nastavlja ravno, nema potrebe za okretanjem
        } else if (!maze_has_wall(x, y, right_dir)) {
            API_turnRight();
            current_dir = right_dir;
        } else {
            API_turnRight();
            API_turnRight();
            current_dir = (current_dir + 2) % 4;
        }

        // Pomakni se naprijed nakon što je smjer ispravno postavljen
        API_moveForward();

        // 3. Sinkronizacija koordinata
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
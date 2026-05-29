#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "API.h"
#include "maze.h"
#include "floodfill.h"

typedef enum {
    EXPLORING,      // Ide prema centru i skenira zidove (Plavo)
    RETURNING,      // Vraća se na start (0,0) i skenira usput (Žuto)
    FASTEST_RUN     // Juri najbržom stazom, senzori su isključeni (Zeleno)
} MouseState;

// Snapshot the current flood distances into a buffer so we can
// compare them after the next explore/return cycle.
static int prev_flood[MAZE_SIZE][MAZE_SIZE];

static void snapshot_flood(void) {
    for (int x = 0; x < MAZE_SIZE; x++)
        for (int y = 0; y < MAZE_SIZE; y++)
            prev_flood[x][y] = flood_get_distance(x, y);
}

// Returns true if the flood map is identical to the last snapshot.
static bool flood_is_stable(void) {
    for (int x = 0; x < MAZE_SIZE; x++)
        for (int y = 0; y < MAZE_SIZE; y++)
            if (flood_get_distance(x, y) != prev_flood[x][y])
                return false;
    return true;
}

int main(int argc, char* argv[]) {
    int x = 0, y = 0;
    Direction current_dir = NORTH;
    MouseState state = EXPLORING;
    int pass = 0; // How many explore→return cycles completed

    maze_init();
    memset(prev_flood, 0xFF, sizeof(prev_flood)); // all 255 = "never computed"
    API_clearAllColor();
    API_clearAllText();
    API_setColor(0, 0, 'G');
    API_setText(0, 0, "Start");

    while (true) {
        if (API_wasReset()) {
            API_ackReset();
            x = 0; y = 0; current_dir = NORTH;
            state = EXPLORING;
            pass = 0;
            maze_init();
            memset(prev_flood, 0xFF, sizeof(prev_flood));
            API_clearAllColor();
            API_clearAllText();
            API_setColor(0, 0, 'G');
            API_setText(0, 0, "Start");
        }

        // Scan walls on every non-fast phase
        if (state != FASTEST_RUN) {
            maze_scan_and_map(x, y, current_dir);
        }

        // Colour by phase
        if      (state == EXPLORING)   API_setColor(x, y, 'B');
        else if (state == RETURNING)   API_setColor(x, y, 'Y');
        else if (state == FASTEST_RUN) API_setColor(x, y, 'G');

        // Display flood distance
        char text[6];
        snprintf(text, sizeof(text), "%d", flood_get_distance(x, y));
        API_setText(x, y, text);

        // --- STATE MACHINE ---

        // EXPLORING → RETURNING: reached centre
        if (state == EXPLORING && (x == 7 || x == 8) && (y == 7 || y == 8)) {
            state = RETURNING;
            API_setText(x, y, "CILJ");
        }

        // RETURNING → decide: more passes or fast run
        if (state == RETURNING && x == 0 && y == 0) {
            pass++;

            // Compute flood toward centre from the current (updated) wall map
            flood_update(false);

            // If the flood map didn't change since the last pass, the path is
            // stable — any further exploration won't improve it. Go fast.
            // Also go fast after 5 passes as a safety cap so it doesn't loop forever.
            bool stable = flood_is_stable();

            if (stable || pass >= 5) {
                API_clearAllColor();
                // Repaint final flood distances
                for (int px = 0; px < MAZE_SIZE; px++) {
                    for (int py = 0; py < MAZE_SIZE; py++) {
                        char buf[6];
                        snprintf(buf, sizeof(buf), "%d", flood_get_distance(px, py));
                        API_setText(px, py, buf);
                    }
                }
                API_setText(0, 0, "SPREMAN!");
                for (volatile long long i = 0; i < 30000000; i++);
                state = FASTEST_RUN;
            } else {
                // Snapshot current flood, then do another explore pass
                snapshot_flood();
                state = EXPLORING;
                API_setText(0, 0, "Pass");
            }
        }

        // FASTEST_RUN end: reached centre
        if (state == FASTEST_RUN && (x == 7 || x == 8) && (y == 7 || y == 8)) {
            API_setText(x, y, "POBJEDA!");
            while (!API_wasReset()) { /* wait */ }
            continue;
        }

        // --- FLOOD UPDATE (exploration phases only) ---
        if (state != FASTEST_RUN) {
            bool flood_to_start = (state == RETURNING);
            flood_update(flood_to_start);
        }

        // --- NAVIGATE ---
        Direction best_dir = flood_get_best_direction(x, y, current_dir);

        if (best_dir != current_dir) {
            int turn_step = (best_dir - current_dir + 4) % 4;
            if      (turn_step == 1) API_turnRight();
            else if (turn_step == 3) API_turnLeft();
            else if (turn_step == 2) { API_turnRight(); API_turnRight(); }
            current_dir = best_dir;
        }

        API_moveForward();
        if      (current_dir == NORTH) y++;
        else if (current_dir == EAST)  x++;
        else if (current_dir == SOUTH) y--;
        else if (current_dir == WEST)  x--;
    }
    return 0;
}
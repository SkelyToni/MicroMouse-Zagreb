#include <stdio.h>
#include <stdbool.h>
#include "API.h"
#include "maze.h"

// Pomoćna funkcija koja računa koordinate susjednog polja
void get_next_coords(int x, int y, Direction dir, int* nx, int* ny) {
    *nx = x; *ny = y;
    if (dir == NORTH) (*ny)++;
    else if (dir == EAST)  (*nx)++;
    else if (dir == SOUTH) (*ny)--;
    else if (dir == WEST)  (*nx)--;
}

int main(int argc, char* argv[]) {
    int x = 0, y = 0;
    Direction current_dir = NORTH;

    maze_init();
    API_clearAllColor();
    API_clearAllText();
    API_setColor(0, 0, 'G');

    while (true) {
        if (API_wasReset()) {
            API_ackReset();
            x = 0; y = 0; current_dir = NORTH;
            maze_init();
            API_clearAllColor();
            API_clearAllText();
        }

        // 1. Skeniraj okolinu i zabilježi posjetu
        maze_scan_and_map(x, y, current_dir);
        API_setColor(x, y, 'B'); // Plava boja za posjećeno polje

        Direction left_dir = (current_dir + 3) % 4;
        Direction right_dir = (current_dir + 1) % 4;

        // Izračunaj koordinate susjednih polja
        int nx_left, ny_left;   get_next_coords(x, y, left_dir, &nx_left, &ny_left);
        int nx_front, ny_front; get_next_coords(x, y, current_dir, &nx_front, &ny_front);
        int nx_right, ny_right; get_next_coords(x, y, right_dir, &nx_right, &ny_right);

        Direction next_dir = current_dir;
        bool found_unvisited = false;

        // --- STRATEGIJA 1: Traži PRVO slobodno polje koje NIKAD NIJE POSJEĆENO ---
        // Gledamo ulijevo, pa ravno, pa udesno (prioritet lijeve strane za konzistentnost)
        if (!maze_has_wall(x, y, left_dir) && !maze_is_visited(nx_left, ny_left)) {
            next_dir = left_dir;
            found_unvisited = true;
        } else if (!maze_has_wall(x, y, current_dir) && !maze_is_visited(nx_front, ny_front)) {
            next_dir = current_dir;
            found_unvisited = true;
        } else if (!maze_has_wall(x, y, right_dir) && !maze_is_visited(nx_right, ny_right)) {
            next_dir = right_dir;
            found_unvisited = true;
        }

        // --- STRATEGIJA 2: Ako su sva prohodna polja već posjećena, koristi standardno pravilo praćenja zida ---
        if (!found_unvisited) {
            if (!maze_has_wall(x, y, left_dir)) {
                next_dir = left_dir;
            } else if (!maze_has_wall(x, y, current_dir)) {
                next_dir = current_dir;
            } else if (!maze_has_wall(x, y, right_dir)) {
                next_dir = right_dir;
            } else {
                next_dir = (current_dir + 2) % 4; // Slijepa ulica, okreni se za 180°
            }
        }

        // 2. Izvrši skretanje u simulatoru
        if (next_dir != current_dir) {
            int turn_step = (next_dir - current_dir + 4) % 4;
            if (turn_step == 1)      API_turnRight();
            else if (turn_step == 3) API_turnLeft();
            else if (turn_step == 2) { API_turnRight(); API_turnRight(); }
            current_dir = next_dir;
        }

        // 3. Pomakni se i ažuriraj lokalne koordinate
        API_moveForward();
        get_next_coords(x, y, current_dir, &x, &y);

        // Ispis koordinata na polje
        char text[6];
        snprintf(text, sizeof(text), "%d,%d", x, y);
        API_setText(x, y, text);
    }
    return 0;
}
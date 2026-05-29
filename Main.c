#include <stdio.h>
#include <stdbool.h>
#include "API.h"
#include "maze.h"
#include "floodfill.h"

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
            x = 0; y = 0; current_dir = NORTH;
            maze_init();
            API_clearAllColor();
            API_clearAllText();
            API_setColor(0, 0, 'G');
        }

        // 1. Skeniraj senzore i mapiraj zidove bitwise tehnikom
        maze_scan_and_map(x, y, current_dir);
        API_setColor(x, y, 'B'); 

        // 2. Pokreni Flood-Fill proračun na temelju novog stanja zidova
        flood_update();

        // Ispisuj Flood-Fill udaljenost na polja radi lakšeg praćenja rada algoritma
        char text[6];
        snprintf(text, sizeof(text), "%d", flood_get_distance(x, y));
        API_setText(x, y, text);

        // Provjera jesmo li stigli u centar labirinta
        if ((x == 7 || x == 8) && (y == 7 || y == 8)) {
            API_setText(x, y, "CILJ!");
        }

        // 3. Saznaj od algoritma kamo ići (gdje je najmanji broj)
        Direction best_dir = flood_get_best_direction(x, y, current_dir);

        // 4. Izvrši skretanje miša
        if (best_dir != current_dir) {
            int turn_step = (best_dir - current_dir + 4) % 4;
            if (turn_step == 1)      API_turnRight();
            else if (turn_step == 3) API_turnLeft();
            else if (turn_step == 2) { API_turnRight(); API_turnRight(); }
            current_dir = best_dir;
        }

        // 5. Pomakni se naprijed i ažuriraj koordinate
        API_moveForward();
        if (current_dir == NORTH) y++;
        else if (current_dir == EAST)  x++;
        else if (current_dir == SOUTH) y--;
        else if (current_dir == WEST)  x--;
    }
    return 0;
}
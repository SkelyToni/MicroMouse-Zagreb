#include <stdio.h>
#include <stdbool.h>
#include "API.h"
#include "maze.h"
#include "floodfill.h"

// Definiramo tri faze u životu našeg miša
typedef enum {
    EXPLORING,      // Ide prema centru i skenira zidove (Plavo)
    RETURNING,      // Vraća se na start (0,0) i skenira usput (Žuto)
    FASTEST_RUN     // Juri najbržom stazom, senzori su isključeni (Zeleno)
} MouseState;

int main(int argc, char* argv[]) {
    int x = 0, y = 0;
    Direction current_dir = NORTH;
    
    // Početno stanje je istraživanje
    MouseState state = EXPLORING;

    maze_init();
    API_clearAllColor();
    API_clearAllText();
    API_setColor(0, 0, 'G');
    API_setText(0, 0, "Start");

    while (true) {
        // Handle simulator reset button
        if (API_wasReset()) {
            API_ackReset();
            x = 0; y = 0; current_dir = NORTH;
            state = EXPLORING;
            maze_init();
            API_clearAllColor();
            API_clearAllText();
            API_setColor(0, 0, 'G');
            API_setText(0, 0, "Start");
        }

        // 1. SKENIRANJE ZIDOVA (Samo ako NE vozimo brzu vožnju!)
        // U FASTEST_RUN fazi miš potpuno ignorira senzore i vjeruje samo svojoj mapi
        if (state != FASTEST_RUN) {
            maze_scan_and_map(x, y, current_dir);
        }
        
        // Bojanje staze na temelju trenutne faze
        if (state == EXPLORING) {
            API_setColor(x, y, 'B'); // Plavo prema centru
        } else if (state == RETURNING) {
            API_setColor(x, y, 'Y'); // Žuto natrag na start
        } else if (state == FASTEST_RUN) {
            API_setColor(x, y, 'G'); // Zeleno za idealnu stazu
        }

        // Ispis koordinata na ploču
        char text[6];
        snprintf(text, sizeof(text), "%d", flood_get_distance(x, y));
        API_setText(x, y, text);

        // --- LOGIKA PROMJENE FAZA (STATE MACHINE) ---
        
        // Faza 1 -> Faza 2: Stigao u centar, okreći se natrag
        if (state == EXPLORING && (x == 7 || x == 8) && (y == 7 || y == 8)) {
            state = RETURNING;
            API_setText(x, y, "CILJ 1");
        }

        // Faza 2 -> Faza 3: Vratio se na start, spremi se za brzi run!
        if (state == RETURNING && x == 0 && y == 0) {
            API_setText(0, 0, "SPREMAN!");
            
            // Mala pauza od 1.5 sekundi na startu da se miš "pribere" i smiri prije jurnjave
            // (Simulator će nakratko zastati, što izgleda jako profesionalno)
            for(volatile long long i=0; i<30000000; i++); 
            
            state = FASTEST_RUN;
        }

        // Faza 3 kraj: Ponovno stigao u centar kroz idealnu putanju!
        if (state == FASTEST_RUN && (x == 7 || x == 8) && (y == 7 || y == 8)) {
            API_setText(x, y, "POBJEDA!");
            while(!API_wasReset()) { /* Kraj vožnje, čekaj fizički reset */ }
            continue;
        }

        // 2. PRERAČUNAJ FLOOD-FILL
        // Ako se vraćamo na start, cilj poplave je (0,0). Za istraživanje i brzi run cilj je centar.
        bool flood_to_start = (state == RETURNING);
        flood_update(flood_to_start);

        // 3. ODREDI NAJBOLJI SMJER KRETANJA
        Direction best_dir = flood_get_best_direction(x, y, current_dir);

        // 4. IZVRŠI SKRETANJE
        if (best_dir != current_dir) {
            int turn_step = (best_dir - current_dir + 4) % 4;
            if (turn_step == 1)      API_turnRight();
            else if (turn_step == 3) API_turnLeft();
            else if (turn_step == 2) { API_turnRight(); API_turnRight(); }
            current_dir = best_dir;
        }

        // 5. POMAKNI SE NAPRIJED I AŽURIRAJ KOORDINATE
        API_moveForward();
        if (current_dir == NORTH) y++;
        else if (current_dir == EAST)  x++;
        else if (current_dir == SOUTH) y--;
        else if (current_dir == WEST)  x--;
    }
    return 0;
}
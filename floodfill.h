#ifndef FLOODFILL_H
#define FLOODFILL_H

#include "maze.h"

// Preračunava matricu udaljenosti na temelju trenutno otkrivenih zidova
void flood_update(void);

// Vraća izračunatu udaljenost za određeno polje (za ispis na ekranu)
int flood_get_distance(int x, int y);

// Određuje najbolji idući smjer (smjer s najmanjim brojem/udaljenosti)
Direction flood_get_best_direction(int x, int y, Direction current_dir);

#endif
#include "Maze.h"
#include "API.h"

// Interna matrica labirinta - skrivena unutar ovog file-a
static unsigned char maze[MAZE_SIZE][MAZE_SIZE];

// Pomoćna funkcija koja pretvara smjer u odgovarajuću bitmasku
static unsigned char get_wall_mask(Direction dir) {
    switch (dir) {
        case NORTH: return WALL_NORTH;
        case EAST:  return WALL_EAST;
        case SOUTH: return WALL_SOUTH;
        case WEST:  return WALL_WEST;
    }
    return 0;
}

// Inicijalizacija matrice i postavljanje vanjskih rubova labirinta
void maze_init(void) {
    for (int x = 0; x < MAZE_SIZE; x++) {
        for (int y = 0; y < MAZE_SIZE; y++) {
            maze[x][y] = 0;
            
            // Pre-programiranje poznatih vanjskih zidova labirinta
            if (y == MAZE_SIZE - 1) maze[x][y] |= WALL_NORTH;
            if (x == MAZE_SIZE - 1) maze[x][y] |= WALL_EAST;
            if (y == 0)             maze[x][y] |= WALL_SOUTH;
            if (x == 0)             maze[x][y] |= WALL_WEST;
        }
    }
}

// Postavlja zid u trenutno polje, ali i u susjedno polje (dvosmjerna sinkronizacija)
static void set_wall_bidirectional(int x, int y, Direction dir) {
    // Upiši u trenutno polje i javi simulatoru da iscrta zid
    maze[x][y] |= get_wall_mask(dir);
    API_setWall(x, y, "nesw"[dir]);

    // Izračunaj koordinate susjednog polja
    int nx = x, ny = y;
    Direction opposite_dir = (dir + 2) % 4; // Suprotni smjer (npr. Sjever -> Jug)

    if (dir == NORTH) ny++;
    else if (dir == EAST) nx++;
    else if (dir == SOUTH) ny--;
    else if (dir == WEST) nx--;

    // Ako je susjedno polje unutar granica labirinta, upiši zid i njemu
    if (nx >= 0 && nx < MAZE_SIZE && ny >= 0 && ny < MAZE_SIZE) {
        maze[nx][ny] |= get_wall_mask(opposite_dir);
        API_setWall(nx, ny, "nesw"[opposite_dir]);
    }
}

// Skenira senzore s trenutne pozicije i ažurira mapu
void maze_scan_and_map(int x, int y, Direction current_dir) {
    // Označi trenutno polje kao posjećeno
    maze[x][y] |= VISITED;

    // Provjeri prednji zid
    if (API_wallFront()) {
        set_wall_bidirectional(x, y, current_dir);
    }
    
    // Provjeri lijevi zid
    Direction left_dir = (current_dir + 3) % 4;
    if (API_wallLeft()) {
        set_wall_bidirectional(x, y, left_dir);
    }
    
    // Provjeri desni zid
    Direction right_dir = (current_dir + 1) % 4;
    if (API_wallRight()) {
        set_wall_bidirectional(x, y, right_dir);
    }
}

// Provjerava postoji li zid u memoriji miša za zadano polje i smjer
bool maze_has_wall(int x, int y, Direction dir) {
    return (maze[x][y] & get_wall_mask(dir)) != 0;
}
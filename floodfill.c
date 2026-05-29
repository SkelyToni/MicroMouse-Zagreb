#include "floodfill.h"
#include <stdbool.h>

#define MAX_VALUE 255

static int dist[MAZE_SIZE][MAZE_SIZE];

void flood_update(bool to_start) {
    for (int x = 0; x < MAZE_SIZE; x++) {
        for (int y = 0; y < MAZE_SIZE; y++) {
            dist[x][y] = MAX_VALUE;
        }
    }

    int queueX[256], queueY[256];
    int head = 0, tail = 0;

    if (to_start) {
        dist[0][0] = 0;
        queueX[tail] = 0;
        queueY[tail] = 0;
        tail++;
    } else {
        int centers[4][2] = {{7, 7}, {7, 8}, {8, 7}, {8, 8}};
        for (int i = 0; i < 4; i++) {
            int cx = centers[i][0];
            int cy = centers[i][1];
            dist[cx][cy] = 0;
            queueX[tail] = cx;
            queueY[tail] = cy;
            tail++;
        }
    }

    while (head < tail) {
        int cx = queueX[head];
        int cy = queueY[head];
        head++;

        int current_dist = dist[cx][cy];

        if (!maze_has_wall(cx, cy, NORTH) && (cy + 1 < MAZE_SIZE) && dist[cx][cy + 1] == MAX_VALUE) {
            dist[cx][cy + 1] = current_dist + 1;
            queueX[tail] = cx; queueY[tail] = cy + 1; tail++;
        }
        if (!maze_has_wall(cx, cy, EAST) && (cx + 1 < MAZE_SIZE) && dist[cx + 1][cy] == MAX_VALUE) {
            dist[cx + 1][cy] = current_dist + 1;
            queueX[tail] = cx + 1; queueY[tail] = cy; tail++;
        }
        if (!maze_has_wall(cx, cy, SOUTH) && (cy - 1 >= 0) && dist[cx][cy - 1] == MAX_VALUE) {
            dist[cx][cy - 1] = current_dist + 1;
            queueX[tail] = cx; queueY[tail] = cy - 1; tail++;
        }
        if (!maze_has_wall(cx, cy, WEST) && (cx - 1 >= 0) && dist[cx - 1][cy] == MAX_VALUE) {
            dist[cx - 1][cy] = current_dist + 1;
            queueX[tail] = cx - 1; queueY[tail] = cy; tail++;
        }
    }
}

int flood_get_distance(int x, int y) {
    return dist[x][y];
}

Direction flood_get_best_direction(int x, int y, Direction current_dir) {
    Direction best_dir = current_dir;
    int min_dist = MAX_VALUE + 1; // Sentinel: nothing chosen yet

    // Priority order: current direction first, then right, then left, then reverse.
    // This way, straight-ahead wins all tiebreaks, minimising unnecessary turns.
    const int priority[4] = {
        current_dir,              // straight  (cost 0 turns)
        (current_dir + 1) % 4,   // right     (cost 1 turn)
        (current_dir + 3) % 4,   // left      (cost 1 turn)
        (current_dir + 2) % 4    // reverse   (cost 2 turns)
    };

    for (int i = 0; i < 4; i++) {
        int d = priority[i];

        if (maze_has_wall(x, y, d)) continue;

        int nx = x, ny = y;
        if      (d == NORTH) ny++;
        else if (d == EAST)  nx++;
        else if (d == SOUTH) ny--;
        else if (d == WEST)  nx--;

        if (nx < 0 || nx >= MAZE_SIZE || ny < 0 || ny >= MAZE_SIZE) continue;

        // Strictly less-than: first direction checked wins all ties,
        // and the priority order above ensures straight wins ties.
        if (dist[nx][ny] < min_dist) {
            min_dist = dist[nx][ny];
            best_dir = d;
        }
    }

    return best_dir;
}

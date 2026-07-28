#ifndef GHOST_H
#define GHOST_H

#include <SDL.h>
#include "Maze.h"

class Ghost {
public:
    int col, row;
    int dirX, dirY;
    Uint32 lastMoveTick;

    Ghost(int startCol, int startRow);

    void reset();
    void update(const Maze& maze, int pacCol, int pacRow, Uint32 now);
};

#endif
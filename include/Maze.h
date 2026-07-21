#ifndef MAZE_H
#define MAZE_H

#include "Globals.h"

// Kích thước ô lưới chuẩn Arcade
constexpr int GRID_W = 28;
constexpr int GRID_H = 31;

// Các loại ô trong bản đồ
constexpr int EMPTY  = 0;
constexpr int WALL   = 1;
constexpr int PELLET = 2;
constexpr int POWER  = 3;
constexpr int DOOR   = 4;
constexpr int HOUSE  = 5;

class Maze {
public:
    int grid[GRID_H][GRID_W];
    int pelletTotal;

    Maze();
    void build();
    bool pacWalkable(int x, int y) const;
};

#endif
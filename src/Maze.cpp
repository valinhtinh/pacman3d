#include "Maze.h"
#include <cstdio>

Maze::Maze() {
    pelletTotal = 0;
}

void Maze::build() {
    // 1. Phủ kín bản đồ bằng tường
    for (int r = 0; r < GRID_H; r++)
        for (int c = 0; c < GRID_W; c++)
            grid[r][c] = WALL;

    // 2. Rải thức ăn nhỏ (Pellet) ở vùng bên trong
    for (int r = 1; r <= GRID_H - 2; r++)
        for (int c = 1; c <= GRID_W - 2; c++)
            grid[r][c] = PELLET;

    // 3. Tạo đường hầm xuyên bản đồ
    grid[11][0] = EMPTY;
    grid[11][GRID_W - 1] = EMPTY;

    // 4. Xây các khối trụ (tường bên trong)
    for (int r = 2; r <= GRID_H - 3; r++)
        for (int c = 2; c <= GRID_W - 3; c++)
            if ((r % 4 == 2 || r % 4 == 3) && (c % 4 == 2 || c % 4 == 3))
                grid[r][c] = WALL;

    // 5. Đặt thức ăn lớn (Power Pellet) ở 4 góc
    grid[1][1] = POWER;
    grid[1][GRID_W - 2] = POWER;
    grid[GRID_H - 2][1] = POWER;
    grid[GRID_H - 2][GRID_W - 2] = POWER;

    // 6. Xây nhà cho Ma (Ghost House)
    for (int c = 8; c <= 12; c++) grid[9][c] = WALL;
    grid[9][9] = DOOR; grid[9][10] = DOOR; grid[9][11] = DOOR;
    for (int c = 8; c <= 12; c++) grid[13][c] = WALL;
    for (int r = 10; r <= 12; r++) { grid[r][8] = WALL; grid[r][12] = WALL; }
    for (int r = 10; r <= 12; r++)
        for (int c = 9; c <= 11; c++)
            grid[r][c] = HOUSE;

    // 7. Xóa thức ăn ở vị trí xuất phát của Pac-Man
    grid[17][10] = EMPTY;

    // Đếm tổng số lượng thức ăn
    pelletTotal = 0;
    for (int r = 0; r < GRID_H; r++)
        for (int c = 0; c < GRID_W; c++)
            if (grid[r][c] == PELLET || grid[r][c] == POWER) pelletTotal++;
}

bool Maze::pacWalkable(int x, int y) const {
    int t = grid[y][x];
    // Pacman không thể đi xuyên tường, cửa nhà ma, và khu vực trong nhà ma
    return t != WALL && t != DOOR && t != HOUSE;
}

bool Maze::ghostWalkable(int x, int y, GMode mode) const {
    int t = grid[y][x];
    if (t == WALL) return false;
    
    // Ma bình thường không được vào lại nhà, trừ khi bị ăn (Eaten) hoặc đang trong tiến trình đi ra/vào
    if (t == DOOR || t == HOUSE)
        return mode == GMode::Eaten || mode == GMode::Entering ||
               mode == GMode::Exiting || mode == GMode::InHouse;
               
    return true;
}

bool Maze::validate() const {
    std::array<std::array<bool, GRID_W>, GRID_H> seen{};
    std::vector<Point> stack{{10, 17}};
    seen[17][10] = true;
    int reachablePellets = 0;
    
    if (grid[17][10] == PELLET || grid[17][10] == POWER) reachablePellets++;
    
    while (!stack.empty()) {
        Point p = stack.back(); stack.pop_back();
        static const Dir dirs[4] = {Dir::Up, Dir::Down, Dir::Left, Dir::Right};
        for (Dir d : dirs) {
            Point n = neighborWrap(p.x, p.y, d);
            if (n.y < 0 || n.y >= GRID_H) continue;
            if (seen[n.y][n.x]) continue;
            if (!pacWalkable(n.x, n.y)) continue;
            
            seen[n.y][n.x] = true;
            if (grid[n.y][n.x] == PELLET || grid[n.y][n.x] == POWER) reachablePellets++;
            stack.push_back(n);
        }
    }
    printf("validate: %d/%d pellets reachable\n", reachablePellets, pelletTotal);
    return reachablePellets == pelletTotal;
}
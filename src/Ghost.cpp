#include "Ghost.h"
#include <cmath>

namespace {
constexpr Uint32 MOVE_DELAY = 150; // ma đi chậm hơn Pacman (120ms) 1 chút
}

Ghost::Ghost(int startCol, int startRow)
    : col(startCol), row(startRow), dirX(-1), dirY(0), lastMoveTick(0) {}

void Ghost::reset() {
    // vị trí ban đầu được set lại từ ngoài (main.cpp) nếu cần,
    // ở đây chỉ reset hướng đi
    dirX = -1;
    dirY = 0;
}

void Ghost::update(const Maze& maze, int pacCol, int pacRow, Uint32 now) {
    if (now - lastMoveTick < MOVE_DELAY) return;
    lastMoveTick = now;

    // 4 hướng ưu tiên: Lên, Trái, Xuống, Phải
    static const int dx[4] = { 0, -1, 0, 1 };
    static const int dy[4] = { -1, 0, 1, 0 };

    int bestDx = dirX, bestDy = dirY;
    float bestDist = -1.0f;
    bool found = false;

    for (int i = 0; i < 4; i++) {
        // không cho quay đầu 180 độ
        if (dx[i] == -dirX && dy[i] == -dirY) continue;

        int nx = col + dx[i];
        int ny = row + dy[i];
        if (!maze.pacWalkable(nx, ny)) continue; // dùng chung luật đi lại với Pacman

        float d = std::sqrt(float((nx - pacCol) * (nx - pacCol) +
                                   (ny - pacRow) * (ny - pacRow)));
        if (!found || d < bestDist) {
            bestDist = d;
            bestDx = dx[i];
            bestDy = dy[i];
            found = true;
        }
    }

    if (found) {
        dirX = bestDx;
        dirY = bestDy;
        col += dirX;
        row += dirY;
    }
}
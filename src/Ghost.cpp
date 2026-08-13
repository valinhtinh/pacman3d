#include "Ghost.h"
#include <cmath>

namespace {
constexpr Uint32 MOVE_DELAY = 150; // ma đi chậm hơn Pacman(120ms) 30ms
}

Ghost::Ghost(LoaiMa loaiMa, int startCol, int startRow, int gocCol, int gocRow, Uint32 choMs)
    : loai(loaiMa), col(startCol), row(startRow), dirX(-1), dirY(0),
      lastMoveTick(0), gocRieng_col(gocCol), gocRieng_row(gocRow),
      isOut(choMs == 0), timeWait(choMs), mocBatDauGame(0) {}

void Ghost::reset(int startCol, int startRow) {
    col = startCol;
    row = startRow;
    dirX = 0;
    dirY = -1;
    isOut = (timeWait == 0);
}

void Ghost::resetGio(Uint32 now) {
    mocBatDauGame = now;
}

//ma chỉ bị chắn tường, ô house/door đi xuyên đc
bool Ghost::diChuyen(const Maze& maze, int x, int y) const {
    if (x < 0 || x >= GRID_W || y < 0 || y >= GRID_H) return false; 
    return maze.grid[y][x] != WALL; 
}

// Tính mục tiêu dựa trên loại ma và vị trí Pacman, Blinky
void Ghost::tinhMucTieu(int pacCol, int pacRow, int pacDirX, int pacDirY,
                         int blinkyCol, int blinkyRow,
                         int& mucTieuCol, int& mucTieuRow) const {
    switch (loai) {
        case BLINKY:
            // đuổi Pacman trực tiếp
            mucTieuCol = pacCol;
            mucTieuRow = pacRow;
            break;

        case PINKY:
            // đón ở phía trước Pacman 4 ô, không quan tâm Blinky
            mucTieuCol = pacCol + pacDirX * 4;
            mucTieuRow = pacRow + pacDirY * 4;
            break;

        case INKY: {
            // lấy 2 ô phía trước Pacman, rồi tính phản chiếu qua Blinky
            int diemTruocCol = pacCol + pacDirX * 2;
            int diemTruocRow = pacRow + pacDirY * 2;
            mucTieuCol = blinkyCol + 2 * (diemTruocCol - blinkyCol);
            mucTieuRow = blinkyRow + 2 * (diemTruocRow - blinkyRow);
            break;
        }

        case CLYDE: {
            float khoangCach = std::sqrt(
                float((pacCol - col) * (pacCol - col) + (pacRow - row) * (pacRow - row)));
            if (khoangCach > 8.0f) {
                // ở xa thì đuổi như Blinky
                mucTieuCol = pacCol;
                mucTieuRow = pacRow;
            } else {
                // ở gần thì bỏ chạy về góc riêng
                mucTieuCol = gocRieng_col;
                mucTieuRow = gocRieng_row;
            }
            break;
        }
    }
}

// chọn hướng đi tốt nhất 
void Ghost::chonHuongDi(const Maze& maze, int mucTieuCol, int mucTieuRow) {
    // lên, trái, xuống, phải
    static const int dx[4] = { 0, -1, 0, 1 }; 
    static const int dy[4] = { -1, 0, 1, 0 };

    int huongTotNhat_dx = dirX, huongTotNhat_dy = dirY;
    float khoangCachGanNhat = -1.0f;
    bool found = false;

    for (int i = 0; i < 4; i++) {
        if (dx[i] == -dirX && dy[i] == -dirY) continue; // không cho quay đầu 180 độ

        int nx = col + dx[i];
        int ny = row + dy[i];
        if (!diChuyen(maze, nx, ny)) continue; 

        float d = std::sqrt(float((nx - mucTieuCol) * (nx - mucTieuCol) +
                                   (ny - mucTieuRow) * (ny - mucTieuRow)));
        if (!found || d < khoangCachGanNhat) {
            khoangCachGanNhat = d;
            huongTotNhat_dx = dx[i];
            huongTotNhat_dy = dy[i];
            found = true;
        }
    }

    if (found) {
        dirX = huongTotNhat_dx;
        dirY = huongTotNhat_dy;
    }
}

void Ghost::capNhat(const Maze& maze,
                     int pacCol, int pacRow, int pacDirX, int pacDirY,
                     int blinkyCol, int blinkyRow, Uint32 now) {
    // Nếu ma chưa ra khỏi nhà
    if (!isOut) {
        if (now - mocBatDauGame >= timeWait) {
            isOut = true; // cho ma ra khỏi nhà
        } else {
            return; // vẫn đang chờ, không di chuyển
        }
    }

    if (now - lastMoveTick < MOVE_DELAY) return;
    lastMoveTick = now;

    int mucTieuCol, mucTieuRow;
    tinhMucTieu(pacCol, pacRow, pacDirX, pacDirY, blinkyCol, blinkyRow,
                mucTieuCol, mucTieuRow);
    chonHuongDi(maze, mucTieuCol, mucTieuRow);

    if (diChuyen(maze, col + dirX, row + dirY)) {
        col += dirX;
        row += dirY;
    }
}
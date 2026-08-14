#include "Ghost.h"
#include <cmath>
#include <cstdlib>

namespace {
constexpr Uint32 NORMAL_DELAY = 150; // ma đi chậm hơn Pacman(120ms) 30ms
constexpr Uint32 FEAR_DELAY = 200; // đi chậm hơn khi sợ
constexpr Uint32 HOME_DELAY = 100; // đi nhanh hơn khi về nhà
}

Ghost::Ghost(LoaiMa loaiMa, int startCol, int startRow, int gocCol, int gocRow, Uint32 choMs)
    : loai(loaiMa), trangThai(NORMAL),
      col(startCol), row(startRow), dirX(0), dirY(-1), lastMoveTick(0),
      nhaCol(startCol), nhaRow(startRow),
      gocRieng_col(gocCol), gocRieng_row(gocRow),
      isOut(choMs == 0), timeWait(choMs), startTime(0),
      endFearTime(0) {}

void Ghost::reset(int startCol, int startRow) {
    col = startCol;
    row = startRow;
    dirX = 0;
    dirY = -1;
    trangThai = NORMAL;
    isOut = (timeWait == 0);
}
 
void Ghost::resetGio(Uint32 now) {
    startTime = now;
}

void Ghost::startFear(Uint32 durationMs, Uint32 now) {
    if(trangThai == HOME) return; // nếu đang về nhà thì không bị sợ
    trangThai = FEAR;
    endFearTime = now + durationMs;
    dirX = -dirX; // quay đầu
    dirY = -dirY;
}

void Ghost::biAn() {
    trangThai = HOME;
    col = nhaCol;
    row = nhaRow;
    dirX = 0;
    dirY = -1;
    isOut = false;
}

bool Ghost::isFear() const {
    return trangThai == FEAR;
}

bool Ghost::isFearEndingSoon(Uint32 now) const {
    if (trangThai != FEAR || now >= endFearTime) return false;
    return (endFearTime - now) <= 3000; // nhấp nháy báo hiệu 3s cuối
}

//ma chỉ bị chắn tường, ô house/door đi xuyên đc
bool Ghost::diChuyen(const Maze& maze, int x, int y) const {
    if (y < 0 || y >= GRID_H) return false;
    int wrappedX = maze.wrapCol(x);
    return maze.grid[y][wrappedX] != WALL;
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
void Ghost::chonHuongDi(const Maze& maze, int mucTieuCol, int mucTieuRow, bool choPhepQuayDau) {
    // lên, trái, xuống, phải
    static const int dx[4] = { 0, -1, 0, 1 }; 
    static const int dy[4] = { -1, 0, 1, 0 };

    int huongTotNhat_dx = dirX, huongTotNhat_dy = dirY;
    float ganNhat = -1.0f;
    bool found = false;

    for (int i = 0; i < 4; i++) {
        if (!choPhepQuayDau && dx[i] == -dirX && dy[i] == -dirY) continue; // không cho quay đầu 180 độ

        int nx = col + dx[i];
        int ny = row + dy[i];

        if (!diChuyen(maze, nx, ny)) continue; 

        float d = std::sqrt(float((nx - mucTieuCol) * (nx - mucTieuCol) +
                                   (ny - mucTieuRow) * (ny - mucTieuRow)));
        if (!found || d < ganNhat) {
            ganNhat = d;
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

void Ghost::diNgauNhien(const Maze& maze) {
    // lên, trái, xuống, phải
    static const int dx[4] = { 0, -1, 0, 1 }; 
    static const int dy[4] = { -1, 0, 1, 0 };

    int stt[4] = {0, 1, 2, 3};
    for ( int i = 3; i > 0; i--) std ::swap(stt[i], stt[std::rand() % (i + 1)]); // xáo trộn thứ tự

    for ( int i = 0; i < 4; i++) {
        int k = stt[i];
        if (dx[k] == -dirX && dy[k] == -dirY) continue; // không cho quay đầu 180 độ
        if (diChuyen(maze, col + dx[k], row + dy[k])) {
            dirX = dx[k];
            dirY = dy[k];
            return;
        }
    }
}

void Ghost::capNhat(const Maze& maze,
                     int pacCol, int pacRow, int pacDirX, int pacDirY,
                     int blinkyCol, int blinkyRow, Uint32 now) {
    // Nếu ma chưa ra khỏi nhà
    if (!isOut) {
        if (now - startTime >= timeWait) {
            isOut = true; // cho ma ra khỏi nhà
        } else {
            return; // vẫn đang chờ, không di chuyển
        }
    }
    // Về nhà thì trở lại bình thường
    if (trangThai == HOME && col == nhaCol && row == nhaRow) {
        trangThai = NORMAL;
    }
    // Hết thời gian sợ thì trở lại bình thường
    if (trangThai == FEAR && now >= endFearTime) {
        trangThai = NORMAL;
    }

    Uint32 MOVE_DELAY = (trangThai == FEAR) ? FEAR_DELAY 
                   : (trangThai == HOME) ? HOME_DELAY 
                   : NORMAL_DELAY;


    if (now - lastMoveTick < MOVE_DELAY) return;
    lastMoveTick = now;

    if (trangThai == FEAR) {
        // đi ngẫu nhiên khi sợ
        diNgauNhien(maze);
    } else if (trangThai == HOME) {
        chonHuongDi(maze, nhaCol, nhaRow, true); // về nhà thì cho phép quay đầu
    } else {
        int mucTieuCol, mucTieuRow;
        tinhMucTieu(pacCol, pacRow, pacDirX, pacDirY, blinkyCol, blinkyRow,
                    mucTieuCol, mucTieuRow);
        chonHuongDi(maze, mucTieuCol, mucTieuRow, false);
    }

    if (diChuyen(maze, col + dirX, row + dirY)) {
        col = maze.wrapCol(col + dirX);
        row += dirY;
    }
}
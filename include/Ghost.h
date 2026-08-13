#ifndef GHOST_H
#define GHOST_H

#include <SDL.h>
#include "Maze.h"

enum LoaiMa { BLINKY, PINKY, INKY, CLYDE };

class Ghost {
public:
    LoaiMa loai;
    int col, row;
    int dirX, dirY;
    Uint32 lastMoveTick;

    int gocRieng_col, gocRieng_row; // vị trí góc riêng ma,tính toán hướng đi khi ở scatter

    bool isOut; 
    Uint32 timeWait;
    Uint32 mocBatDauGame; // set = hàm resetGio, tính thời gian chờ cho ma

    Ghost(LoaiMa loaiMa, int startCol, int startRow, int gocCol, int gocRow, Uint32 choMs);

    void reset(int startCol, int startRow);
    void resetGio(Uint32 now); // gọi khi reset game, đặt lại thời gian chờ cho ma

    // Cập nhật trạng thái ma theo vị trí Pacman và Blinky
    void capNhat(const Maze& maze,
                 int pacCol, int pacRow, int pacDirX, int pacDirY,
                 int blinkyCol, int blinkyRow, Uint32 now);

private:
    bool diChuyen(const Maze& maze, int x, int y) const; // riêng cho ma:có đi được không, chỉ chặn tường

    void chonHuongDi(const Maze& maze, int mucTieuCol, int mucTieuRow);

    void tinhMucTieu(int pacCol, int pacRow, int pacDirX, int pacDirY,
                      int blinkyCol, int blinkyRow,
                      int& mucTieuCol, int& mucTieuRow) const;
};

#endif
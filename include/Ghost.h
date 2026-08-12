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

    Ghost(LoaiMa loaiMa, int startCol, int startRow, int gocCol, int gocRow);

    void reset(int startCol, int startRow);

    // Cập nhật trạng thái ma theo vị trí Pacman và Blinky
    void capNhat(const Maze& maze,
                 int pacCol, int pacRow, int pacDirX, int pacDirY,
                 int blinkyCol, int blinkyRow, Uint32 now);

private:
    void chonHuongDi(const Maze& maze, int mucTieuCol, int mucTieuRow);

    void tinhMucTieu(int pacCol, int pacRow, int pacDirX, int pacDirY,
                      int blinkyCol, int blinkyRow,
                      int& mucTieuCol, int& mucTieuRow) const;
};

#endif
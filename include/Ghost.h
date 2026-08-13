#ifndef GHOST_H
#define GHOST_H

#include <SDL.h>
#include "Maze.h"

enum LoaiMa { BLINKY, PINKY, INKY, CLYDE };
enum TrangThaiMa { NORMAL, FEAR, HOME };

class Ghost {
public:
    LoaiMa loai;
    TrangThaiMa trangThai;

    int col, row;
    int dirX, dirY;
    Uint32 lastMoveTick;

    int nhaCol, nhaRow; // vị trí nhà ma, reset về đây khi bị ăn
    int gocRieng_col, gocRieng_row; // vị trí góc riêng ma,tính toán hướng đi khi ở scatter

    bool isOut; 
    Uint32 timeWait;
    Uint32 startTime; // set = hàm resetGio, tính thời gian chờ cho ma
    Uint32 endFearTime; // thời gian kết thúc trạng thái sợ hãi, tính từ lúc Pacman ăn Power Pellet

    Ghost(LoaiMa loaiMa, int startCol, int startRow, int gocCol, int gocRow, Uint32 choMs);

    void reset(int startCol, int startRow);
    void resetGio(Uint32 now); // gọi khi reset game, đặt lại thời gian chờ cho ma
    void startFear(Uint32 durationMs, Uint32 now); // gọi khi Pacman ăn Power Pellet
    void biAn();
    bool isFear() const; 
    bool isFearEndingSoon(Uint32 now) const; // nhấp nháy báo hiệu

    // Cập nhật trạng thái ma theo vị trí Pacman và Blinky
    void capNhat(const Maze& maze,
                 int pacCol, int pacRow, int pacDirX, int pacDirY,
                 int blinkyCol, int blinkyRow, Uint32 now);

private:
    bool diChuyen(const Maze& maze, int x, int y) const; // riêng cho ma:có đi được không, chỉ chặn tường

    void chonHuongDi(const Maze& maze, int mucTieuCol, int mucTieuRow, bool choPhepQuayDau);

    void diNgauNhien(const Maze& maze);

    void tinhMucTieu(int pacCol, int pacRow, int pacDirX, int pacDirY,
                      int blinkyCol, int blinkyRow,
                      int& mucTieuCol, int& mucTieuRow) const;
};

#endif
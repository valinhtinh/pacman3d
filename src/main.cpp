#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <cmath>
#include <vector>
#include "Globals.h"
#include "Maze.h"
#include "Ghost.h"
#include "Renderer.h"

namespace {
struct Pacman {
    int col = 14;
    int row = 23;
    int dirX = -1;
    int dirY = 0;
    int nextDirX = -1;
    int nextDirY = 0;
    Uint32 lastMoveTick = 0;
};

bool canMove(const Maze& maze, int col, int row, int dx, int dy) {
    return maze.pacWalkable(col + dx, row + dy);
}

int updatePacman(Maze& maze, Pacman& pacman, const Uint8* keys) {
    int queuedX = 0;
    int queuedY = 0;

    if (keys[SDL_SCANCODE_LEFT]) queuedX = -1;
    else if (keys[SDL_SCANCODE_RIGHT]) queuedX = 1;
    else if (keys[SDL_SCANCODE_UP]) queuedY = -1;
    else if (keys[SDL_SCANCODE_DOWN]) queuedY = 1;

    if (queuedX != 0 || queuedY != 0) {
        pacman.nextDirX = queuedX;
        pacman.nextDirY = queuedY;
    }

    Uint32 now = SDL_GetTicks();
    constexpr Uint32 MOVE_DELAY = 120;
    if (now - pacman.lastMoveTick < MOVE_DELAY) return EMPTY;

    if (canMove(maze, pacman.col, pacman.row, pacman.nextDirX, pacman.nextDirY)) {
        pacman.dirX = pacman.nextDirX;
        pacman.dirY = pacman.nextDirY;
    }

    int loaiVuaAn = EMPTY;
    if (canMove(maze, pacman.col, pacman.row, pacman.dirX, pacman.dirY)) {
        pacman.col += pacman.dirX;
        pacman.row += pacman.dirY;
        loaiVuaAn = maze.eatPellet(pacman.col, pacman.row); // LẤY giá trị trả về
    }

    pacman.lastMoveTick = now;
    return loaiVuaAn;
}

// Tao du 4 con ma, dat o cac o trong nha ma, co thoi gian cho
std::vector<Ghost> taoDanhSachMa() {
    std::vector<Ghost> danhSachMa;
    danhSachMa.emplace_back(BLINKY, 13, 8, 27, 0, 0);   // goc rieng: tren-phai
    danhSachMa.emplace_back(PINKY, 13, 14, 0, 0, 5000);     // goc rieng: tren-trai
    danhSachMa.emplace_back(INKY, 12, 14, 27, 30, 10000);   // goc rieng: duoi-phai
    danhSachMa.emplace_back(CLYDE, 15, 14, 0, 30, 15000);   // goc rieng: duoi-trai
    return danhSachMa;
}

void resetViTri(Pacman& pacman, std::vector<Ghost>& danhSachMa, Uint32 now) {
    pacman.col = 14; pacman.row = 23;
    pacman.dirX = pacman.nextDirX = -1;
    pacman.dirY = pacman.nextDirY = 0;

    static const int startCol[4] = {13, 13, 12, 15};
    static const int startRow[4] = {8, 14, 14, 14};
    for (size_t i = 0; i < danhSachMa.size(); i++) {
        danhSachMa[i].reset(startCol[i], startRow[i]);
        danhSachMa[i].resetGio(now);
    }
}

}

int main(int argc, char* argv[]) {
    SDL_SetMainReady();

    Renderer renderer;
    if (!renderer.init("Pacman", SCREEN_W, SCREEN_H)) {
        return 1;
    }

    Maze maze;
    maze.build();

    Pacman pacman;
    maze.eatPellet(pacman.col, pacman.row); // ăn ô đầu tiên, không cần biết ăn gì

    std::vector<Ghost> danhSachMa = taoDanhSachMa();
    Uint32 gameStartTick = SDL_GetTicks();
    for (auto& ma : danhSachMa) {
        ma.resetGio(gameStartTick);
    }

    int score = 0;

    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;
        }

        Uint32 now = SDL_GetTicks();

        int pelletType = updatePacman(maze, pacman, SDL_GetKeyboardState(nullptr));
        if (pelletType == PELLET) {
            score += 10;
        }
        else if (pelletType == POWER) {
            score += 50;
            for (auto& ma : danhSachMa) {
                ma.startFear(15000, now); // 15 giây sợ
            }
        }

        // Blinky luon la con dau tien - lay vi tri no de cac con khac tinh muc tieu
        int blinkyCol = danhSachMa[0].col;
        int blinkyRow = danhSachMa[0].row;

        for (auto& ma : danhSachMa) {
            ma.capNhat(maze, pacman.col, pacman.row, pacman.dirX, pacman.dirY,
                       blinkyCol, blinkyRow, now);
        }

        // Va chạm pacman và ma
        for (auto& ma : danhSachMa) {
            if (ma.col != pacman.col || ma.row != pacman.row) continue;

            if (ma.isFear()) {
                score += 200;   // ăn đc ma đang sợ
                ma.biAn();       // chỉ con này về nhà, các con khác không ảnh hưởng
            } else if (ma.trangThai == NORMAL) {
                resetViTri(pacman, danhSachMa, now);
                break;
            }
        }

        renderer.clear();
        renderer.drawMaze(maze);
        renderer.drawPacman(
            pacman.col * CELL + CELL / 2.0f,
            TOP_MARGIN + pacman.row * CELL + CELL / 2.0f,
            pacman.dirX, pacman.dirY
        );
        for (auto& ma : danhSachMa) {
            bool ending = ma.isFearEndingSoon(now);
            renderer.drawGhost(
                ma.col * CELL + CELL / 2.0f,
                TOP_MARGIN + ma.row * CELL + CELL / 2.0f,
                ma.loai, ma.trangThai, ma.dirX, ma.dirY, ending
            );
        }
        renderer.drawScore(score);
        renderer.present();
    }

    return 0;
}
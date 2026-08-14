#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <cmath>
#include <vector>
#include "Globals.h"
#include "Maze.h"
#include "Ghost.h"
#include "Renderer.h"
#include "AudioManager.h"

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

int wrapPacmanCol(const Maze& maze, int col) {
    return maze.wrapCol(col);
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
        pacman.col = wrapPacmanCol(maze, pacman.col + pacman.dirX);
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
    pacman.lastMoveTick = now;

    static const int startCol[4] = {13, 13, 12, 15};
    static const int startRow[4] = {8, 14, 14, 14};
    for (size_t i = 0; i < danhSachMa.size(); i++) {
        danhSachMa[i].reset(startCol[i], startRow[i]);
        danhSachMa[i].resetGio(now);
    }
}

void restartGame(Maze& maze, Pacman& pacman, std::vector<Ghost>& danhSachMa,
                  int& score, int& lives, bool& gameEnded, bool& win, Uint32 now){
    maze.build(); // rebuild lại pellet
    score = 0;
    lives = 3;
    gameEnded = false;
    win = false;
    resetViTri(pacman, danhSachMa, now);
}

}

int main(int argc, char* argv[]) {
    SDL_SetMainReady();

    Renderer renderer;
    if (!renderer.init("Pacman", SCREEN_W, SCREEN_H)) {
        return 1;
    }

    AudioManager audioManager;
    audioManager.init();

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
    int lives = 3;
    bool gameEnded = false;
    bool win = false;

    bool running = true;
    bool started = false;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) running = false;
                if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_SPACE) {
                    if (!started) {
                        started = true;
                        gameStartTick = SDL_GetTicks();
                        for (auto& ma : danhSachMa) {
                            ma.resetGio(gameStartTick);
                        }
                        audioManager.playSound(AudioManager::BEGIN);
                    } else if (gameEnded) {
                        restartGame(maze, pacman, danhSachMa, score, lives, gameEnded, win, SDL_GetTicks());
                    }
                }
            }
        }

        if (!started) {
            renderer.clear();
            renderer.drawMaze(maze);
            renderer.drawText("PACMAN 3D", 150, {255, 255, 0, 255});
            renderer.drawText("Press Enter to Start", 210, {255, 255, 255, 255});
            renderer.drawText("Esc to Quit", 250, {255, 255, 255, 255});
            renderer.present();
            continue;
        }

        Uint32 now = SDL_GetTicks();

        if (!gameEnded) {
            int pelletType = updatePacman(maze, pacman, SDL_GetKeyboardState(nullptr));
            if (pelletType == PELLET) {
                score += 10;
                audioManager.playSound(AudioManager::CHOMP);
            }
            else if (pelletType == POWER) {
                score += 50;
                audioManager.playSound(AudioManager::CHOMP);
                for (auto& ma : danhSachMa) {
                    ma.startFear(15000, now);
                }
            }
            // ăn hết pelle -> win
            if (maze.pelletTotal <= 0) {
                gameEnded = true;
                win = true;
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
                    audioManager.playSound(AudioManager::EATGHOST);
                } else if (ma.trangThai == NORMAL) {
                    lives--;
                    audioManager.playSound(AudioManager::DEATH);
                    if (lives <= 0) {
                        gameEnded = true; 
                        win = false;
                        break;
                    }
                    resetViTri(pacman, danhSachMa, now);
                    break;
                }
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

        if (gameEnded && !win) {
            renderer.drawLives(lives);
            renderer.drawText("YOU LOSE", 280, {255, 60, 60, 255});
            renderer.drawText("Press Enter to Restart", 320, {255, 255, 255, 255});
        } else if (gameEnded && win) {
            renderer.drawLives(lives);
            renderer.drawText("YOU WIN", 280, {255, 255, 0, 255});
            renderer.drawText("Score: " + std::to_string(score), 320, {255, 255, 255, 255});
            renderer.drawText("Press Enter to Restart", 360, {255, 255, 255, 255});
        } else {
            renderer.drawScore(score);
            renderer.drawLives(lives);
        }
        renderer.present();

    }

    audioManager.cleanup();
    return 0;
}
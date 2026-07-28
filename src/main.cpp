#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <cmath>
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

void consumeTile(Maze& maze, Pacman& pacman) {
    maze.eatPellet(pacman.col, pacman.row);
}

void updatePacman(Maze& maze, Pacman& pacman, const Uint8* keys) {
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
    if (now - pacman.lastMoveTick < MOVE_DELAY) return;

    if (canMove(maze, pacman.col, pacman.row, pacman.nextDirX, pacman.nextDirY)) {
        pacman.dirX = pacman.nextDirX;
        pacman.dirY = pacman.nextDirY;
    }
    if (canMove(maze, pacman.col, pacman.row, pacman.dirX, pacman.dirY)) {
        pacman.col += pacman.dirX;
        pacman.row += pacman.dirY;
        consumeTile(maze, pacman);
    }
    pacman.lastMoveTick = now;
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
    consumeTile(maze, pacman);

    // Tạm spawn ma ở ngoài nhà ma để test di chuyển (chưa xử lý logic ra/vào nhà)
    Ghost ghost(13, 8);

    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;
        }

        Uint32 now = SDL_GetTicks();

        updatePacman(maze, pacman, SDL_GetKeyboardState(nullptr));
        ghost.update(maze, pacman.col, pacman.row, now);

        // Va chạm: ma trùng ô với Pacman -> reset về vị trí ban đầu
        if (ghost.col == pacman.col && ghost.row == pacman.row) {
            pacman.col = 14; pacman.row = 23;
            pacman.dirX = pacman.nextDirX = -1;
            pacman.dirY = pacman.nextDirY = 0;
            ghost.col = 13; ghost.row = 8;
            ghost.reset();
        }

        renderer.clear();
        renderer.drawMaze(maze);
        renderer.drawPacman(
            pacman.col * CELL + CELL / 2.0f,
            TOP_MARGIN + pacman.row * CELL + CELL / 2.0f,
            pacman.dirX, pacman.dirY
        );
        renderer.drawGhost(
            ghost.col * CELL + CELL / 2.0f,
            TOP_MARGIN + ghost.row * CELL + CELL / 2.0f,
            ghost.dirX, ghost.dirY
        );
        renderer.present();
        
    }

    return 0;
}
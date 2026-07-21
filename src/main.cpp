#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "Globals.h"
#include "Maze.h"
#include "Renderer.h"

int main(int argc, char* argv[]) {
    SDL_SetMainReady();

    Renderer renderer;
    if (!renderer.init("base case", SCREEN_W, SCREEN_H)) {
        return 1;
    }

    // Khởi tạo và xây dựng bản đồ
    Maze maze;
    maze.build();

    bool running = true;
    SDL_Event e;

    // Vòng lặp chính
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
        }

        // Vẽ bản đồ
        renderer.clear();
        renderer.drawMaze(maze);
        renderer.present();
    }

    return 0;
}
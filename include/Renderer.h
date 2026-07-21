#ifndef RENDERER_H
#define RENDERER_H

#include <SDL.h>
#include <SDL_ttf.h>
#include "Maze.h"

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool init(const char* title, int width, int height);
    void close();

    void clear();
    void present();

    void drawMaze(const Maze& maze);

private:
    SDL_Window* window;
    SDL_Renderer* sdlRenderer;
    TTF_Font* font;

    void fillArc(float cx, float cy, float radius, SDL_Color col, float startDeg, float endDeg, int segments);
    void drawWallTile(int x, int y);
    void drawDoorTile(int x, int y);
};

#endif
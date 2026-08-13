#ifndef RENDERER_H
#define RENDERER_H

#include <SDL.h>
#include <SDL_ttf.h>
#include "Maze.h"
#include "Ghost.h"

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool init(const char* title, int width, int height);
    void close();

    void clear();
    void present();

    void drawMaze(const Maze& maze);
    void drawPacman(float cx, float cy, int dirX, int dirY);

    void drawGhost(float cx, float cy, LoaiMa loai, TrangThaiMa trangThai,
                   int dirX, int dirY, bool isFearEndingSoon);
    void Score(int score); 

private:
    SDL_Window* window;
    SDL_Renderer* sdlRenderer;
    TTF_Font* font;
    SDL_Color colorMa(LoaiMa loai, TrangThaiMa trangThai, bool isFearEndingSoon) const;

    void fillArc(float cx, float cy, float radius, SDL_Color col, float startDeg, float endDeg, int segments);
    void drawWallTile(int x, int y);
    void drawDoorTile(int x, int y);
};

#endif
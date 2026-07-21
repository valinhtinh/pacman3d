#include "Renderer.h"
#include <cmath>
#include <vector>
#include <iostream>

Renderer::Renderer() : window(nullptr), sdlRenderer(nullptr), font(nullptr) {}

Renderer::~Renderer() {
    close();
}

bool Renderer::init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return false;
    }
    if (TTF_Init() != 0) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << "\n";
        return false;
    }
    
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!window) return false;

    sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!sdlRenderer) return false;

    return true;
}

void Renderer::close() {
    if (font) { TTF_CloseFont(font); font = nullptr; }
    TTF_Quit();
    if (sdlRenderer) { SDL_DestroyRenderer(sdlRenderer); sdlRenderer = nullptr; }
    if (window) { SDL_DestroyWindow(window); window = nullptr; }
    SDL_Quit();
}

void Renderer::clear() {
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255); // Nền đen
    SDL_RenderClear(sdlRenderer);
}

void Renderer::present() {
    SDL_RenderPresent(sdlRenderer);
}

void Renderer::fillArc(float cx, float cy, float radius, SDL_Color col, float startDeg, float endDeg, int segments) {
    if (segments < 2) segments = 2;
    std::vector<SDL_Vertex> verts;
    std::vector<int> idx;
    verts.reserve(segments + 2);
    
    SDL_Vertex center;
    center.position = {cx, cy}; center.color = col; center.tex_coord = {0, 0};
    verts.push_back(center);
    
    float startRad = startDeg * PI_F / 180.0f;
    float endRad = endDeg * PI_F / 180.0f;
    
    for (int i = 0; i <= segments; i++) {
        float t = startRad + (endRad - startRad) * ((float)i / segments);
        SDL_Vertex v;
        v.position = {cx + radius * std::cos(t), cy + radius * std::sin(t)};
        v.color = col; v.tex_coord = {0, 0};
        verts.push_back(v);
    }
    
    for (int i = 1; i <= segments; i++) { 
        idx.push_back(0); idx.push_back(i); idx.push_back(i + 1); 
    }
    
    SDL_RenderGeometry(sdlRenderer, nullptr, verts.data(), (int)verts.size(), idx.data(), (int)idx.size());
}

void Renderer::drawWallTile(int x, int y) {
    // Màu nền tường nguyên bản: Xanh lam đậm (Blue Arcade)
    SDL_SetRenderDrawColor(sdlRenderer, 33, 33, 222, 255);
    SDL_Rect rect{x, y, CELL, CELL};
    SDL_RenderDrawRect(sdlRenderer, &rect); // Vẽ đường viền ô
}
void Renderer::drawDoorTile(int x, int y) {
    SDL_SetRenderDrawColor(sdlRenderer, 255, 182, 222, 255);
    SDL_Rect r{x + 2, y + CELL / 2 - 2, CELL - 4, 4};
    SDL_RenderFillRect(sdlRenderer, &r);
}

void Renderer::drawMaze(const Maze& maze) {
    int ox = 0, oy = TOP_MARGIN;
    Uint32 ticks = SDL_GetTicks();

    for (int r = 0; r < GRID_H; r++) {
        for (int c = 0; c < GRID_W; c++) {
            int x = ox + c * CELL, y = oy + r * CELL;
            int t = maze.grid[r][c];
            
            if (t == WALL) {
                drawWallTile(x, y);
            } 
            else if (t == DOOR) {
                drawDoorTile(x, y);
            } 
            else if (t == PELLET) {
                SDL_Color col{255, 200, 170, 255};
                fillArc(x + CELL / 2.0f, y + CELL / 2.0f, 3.0f, col, 0, 360, 8);
            } 
            else if (t == POWER) {
                // Hiệu ứng hạt to nhỏ liên tục
                float rad = 6.0f + 2.0f * std::sin(ticks * 0.006f);
                SDL_Color col{255, 220, 180, 255};
                fillArc(x + CELL / 2.0f, y + CELL / 2.0f, rad, col, 0, 360, 16);
            }
        }
    }
}
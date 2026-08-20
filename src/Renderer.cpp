#include "Renderer.h"
#include <cmath>
#include <vector>
#include <string>
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
    if (TTF_Init() != 0) { // vẽ text
        std::cerr << "TTF_Init failed: " << TTF_GetError() << "\n";
        return false;
    }
    
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!window) return false;

    sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!sdlRenderer) return false;

    font = TTF_OpenFont("assets/Southern.ttf", 22); // load font chữ
    if (!font) {
        std::cerr << "TTF_OpenFont failed: " << TTF_GetError() << "\n";
        return false;
    }

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
// Hàm vẽ cung tròn tô kín, dùng chung cho pac, ma, pellet
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

void Renderer::drawPacman(float cx, float cy, int dirX, int dirY) {
    SDL_Color yellow{255, 220, 0, 255};
    fillArc(cx, cy, 8.5f, yellow, 0.0f, 360.0f, 24);

    float facingDeg = 0.0f;
    if (dirX < 0) facingDeg = 180.0f;
    else if (dirY < 0) facingDeg = -90.0f;
    else if (dirY > 0) facingDeg = 90.0f;

    float mouthHalf = 18.0f + 6.0f * std::sin(SDL_GetTicks() * 0.01f);
    float startRad = (facingDeg - mouthHalf) * PI_F / 180.0f;
    float endRad = (facingDeg + mouthHalf) * PI_F / 180.0f;

    SDL_Vertex center;
    center.position = {cx, cy};
    center.color = {0, 0, 0, 255};
    center.tex_coord = {0, 0};

    SDL_Vertex start;
    start.position = {cx + 9.0f * std::cos(startRad), cy + 9.0f * std::sin(startRad)};
    start.color = {0, 0, 0, 255};
    start.tex_coord = {0, 0};

    SDL_Vertex end;
    end.position = {cx + 9.0f * std::cos(endRad), cy + 9.0f * std::sin(endRad)};
    end.color = {0, 0, 0, 255};
    end.tex_coord = {0, 0};

    SDL_Vertex verts[3] = {center, start, end};
    int idx[3] = {0, 1, 2};
    SDL_RenderGeometry(sdlRenderer, nullptr, verts, 3, idx, 3);
}

void Renderer::drawScore(int score) {
    if (!font) return; // nhỡ font load lỗi thì ko crash game

    std::string noiDung = "Score: " + std::to_string(score);
    SDL_Color white{255, 255, 255, 255};

    SDL_Surface* surface = TTF_RenderText_Blended(font, noiDung.c_str(), white);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(sdlRenderer, surface);
    SDL_Rect viTri{10, 5, surface->w, surface->h}; // góc trên bên trái
    SDL_FreeSurface(surface);

    if (texture) {
        SDL_RenderCopy(sdlRenderer, texture, nullptr, &viTri);
        SDL_DestroyTexture(texture); 
    }
}

void Renderer::drawText(const std::string& text, int y, SDL_Color color) {
    if (!font) return;

    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(sdlRenderer, surface);
    SDL_Rect dst{(SCREEN_W - surface->w) / 2, y, surface->w, surface->h};
    SDL_FreeSurface(surface);

    if (texture) {
        SDL_RenderCopy(sdlRenderer, texture, nullptr, &dst);
        SDL_DestroyTexture(texture);
    }
}

void Renderer::drawLives(int lives) {
    if (!font) return;

    std::string noiDung = "Lives: " + std::to_string(lives);
    SDL_Color white{255, 255, 255, 255};

    SDL_Surface* surface = TTF_RenderText_Blended(font, noiDung.c_str(), white);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(sdlRenderer, surface);
    SDL_Rect viTri{SCREEN_W - 120, 5, surface->w, surface->h};
    SDL_FreeSurface(surface);

    if (texture) {
        SDL_RenderCopy(sdlRenderer, texture, nullptr, &viTri);
        SDL_DestroyTexture(texture);
    }
}

namespace {
// Màu gốc của ma khi bình thường
SDL_Color mauTheoLoaiMa(LoaiMa loai) {
    switch (loai) {
        case BLINKY: return {255, 0, 0, 255};     // do
        case PINKY:  return {255, 184, 255, 255}; // hong
        case INKY:   return {0, 255, 255, 255};   // xanh cyan
        case CLYDE:  return {255, 184, 82, 255};  // cam
    }
    return {255, 255, 255, 255};
}
}

// Màu thực tế 
SDL_Color Renderer::colorMa(LoaiMa loai, TrangThaiMa trangThai, bool isFearEndingSoon) const {
    // Đang sợ và sắp hết thời gian -> nhấp nháy giữa trắng và xanh dương
    if (trangThai == FEAR && isFearEndingSoon) {
        bool white = (SDL_GetTicks() / 150) % 2 == 0;
        return white ? SDL_Color{255, 255, 255, 255}
                     : SDL_Color{33, 33, 222, 255};
    }

    // Đang sợ nhưng chưa sắp hết thời gian 
    if (trangThai == FEAR && !isFearEndingSoon) {
        return {33, 33, 222, 255};
    }

    // bình thường
    return mauTheoLoaiMa(loai);
}

void Renderer::drawGhost(float cx, float cy, LoaiMa loai, TrangThaiMa trangThai,
                          int dirX, int dirY, bool isFearEndingSoon) {
    const float radius = 8.5f;

    // nếu ma đang bay về thì vẽ mắt ko vẽ thân
    bool haveBody = (trangThai != HOME);

    if (haveBody) {
        SDL_Color colorBody = colorMa(loai, trangThai, isFearEndingSoon);

        // Nửa đầu trên
        fillArc(cx, cy, radius, colorBody, 180.0f, 360.0f, 16);

        // Thân dưới hcn
        SDL_SetRenderDrawColor(sdlRenderer, colorBody.r, colorBody.g, colorBody.b, 255);
        SDL_Rect than{ (int)(cx - radius), (int)cy, (int)(radius * 2), (int)radius };
        SDL_RenderFillRect(sdlRenderer, &than);
    }

    // luôn vẽ mắt ma trong mọi trạng thái
    SDL_Color matTrang{255, 255, 255, 255};
    SDL_Color trongDen{20, 20, 90, 255};
    for (int ben = -1; ben <= 1; ben += 2) {
        float ex = cx + ben * 3.0f;
        float ey = cy - 2.0f;
        fillArc(ex, ey, 3.0f, matTrang, 0, 360, 8);
        fillArc(ex + dirX * 1.3f, ey + dirY * 1.3f, 1.4f, trongDen, 0, 360, 6);
    }
}
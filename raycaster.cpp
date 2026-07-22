// raycaster.cpp
// Demo ray casting don gian (khong dung DDA).
// Bien dich: g++ -std=c++17 -Iinclude -Iinclude/SDL2 raycaster.cpp -o raycaster -lSDL2

#include <SDL.h>
#include <cmath>
#include <algorithm>

constexpr int SCREEN_WIDTH  = 800;
constexpr int SCREEN_HEIGHT = 600;
constexpr double FOV        = M_PI / 3.0; // ~60 do
constexpr double STEP       = 0.02;       // moi lan tia di them bao nhieu
constexpr double MAX_DIST   = 20.0;

// Ban do: 1 = tuong, 0 = duong di
static int worldMap[8][8] = {
    {1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,1},
    {1,0,1,0,1,1,0,1},
    {1,0,1,0,0,0,0,1},
    {1,0,1,1,1,0,1,1},
    {1,0,0,0,1,0,0,1},
    {1,1,0,0,0,0,1,1},
    {1,1,1,1,1,1,1,1},
};
constexpr int MAP_W = 8;
constexpr int MAP_H = 8;

struct Player {
    double x, y;   // vi tri
    double angle;  // huong nhin (radian)
};

static bool isWall(double x, double y) {
    int mx = static_cast<int>(x);
    int my = static_cast<int>(y);
    if (mx < 0 || mx >= MAP_W || my < 0 || my >= MAP_H) return true;
    return worldMap[my][mx] != 0;
}

// Ban 1 tia theo goc rayAngle: di tung buoc nho cho den khi cham tuong.
static double castRay(const Player &p, double rayAngle) {
    double dx = std::cos(rayAngle);
    double dy = std::sin(rayAngle);
    double dist = 0.0;
    while (dist < MAX_DIST && !isWall(p.x + dx * dist, p.y + dy * dist)) {
        dist += STEP;
    }
    // nhan cos(lech goc) de sua hieu ung fish-eye
    return dist * std::cos(rayAngle - p.angle);
}

static void renderScene(SDL_Renderer *renderer, const Player &player) {
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_Rect ceiling{0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2};
    SDL_RenderFillRect(renderer, &ceiling);

    SDL_SetRenderDrawColor(renderer, 90, 90, 90, 255);
    SDL_Rect floor{0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2};
    SDL_RenderFillRect(renderer, &floor);

    for (int x = 0; x < SCREEN_WIDTH; ++x) {
        double rayAngle = player.angle - FOV / 2.0 + FOV * x / SCREEN_WIDTH;
        double dist = castRay(player, rayAngle);
        if (dist >= MAX_DIST) continue;

        int lineHeight = static_cast<int>(SCREEN_HEIGHT / dist);
        int drawStart = std::max(0, SCREEN_HEIGHT / 2 - lineHeight / 2);
        int drawEnd   = std::min(SCREEN_HEIGHT - 1, SCREEN_HEIGHT / 2 + lineHeight / 2);

        // cang xa cang toi, mo phong do bong don gian
        Uint8 shade = static_cast<Uint8>(std::clamp(255.0 - dist * 20.0, 40.0, 255.0));
        SDL_SetRenderDrawColor(renderer, shade, 20, 20, 255);
        SDL_RenderDrawLine(renderer, x, drawStart, x, drawEnd);
    }
}

int main(int, char*[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init loi: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Ray Casting Demo",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    Player player{3.5, 3.5, M_PI}; // dung giua ban do, nhin sang trai

    const double moveSpeed = 3.0;  // don vi ban do / giay
    const double rotSpeed  = 2.0;  // radian / giay

    bool running = true;
    Uint64 lastTicks = SDL_GetPerformanceCounter();

    while (running) {
        Uint64 nowTicks = SDL_GetPerformanceCounter();
        double dt = double(nowTicks - lastTicks) / SDL_GetPerformanceFrequency();
        lastTicks = nowTicks;

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;
        }

        const Uint8 *keys = SDL_GetKeyboardState(nullptr);
        double nx = player.x, ny = player.y;
        if (keys[SDL_SCANCODE_W]) { nx += std::cos(player.angle) * moveSpeed * dt; ny += std::sin(player.angle) * moveSpeed * dt; }
        if (keys[SDL_SCANCODE_S]) { nx -= std::cos(player.angle) * moveSpeed * dt; ny -= std::sin(player.angle) * moveSpeed * dt; }
        if (!isWall(nx, player.y)) player.x = nx;
        if (!isWall(player.x, ny)) player.y = ny;

        if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])  player.angle -= rotSpeed * dt;
        if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) player.angle += rotSpeed * dt;

        renderScene(renderer, player);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

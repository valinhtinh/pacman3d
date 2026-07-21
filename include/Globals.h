#ifndef GLOBALS_H
#define GLOBALS_H

constexpr int CELL = 20;           // Kích thước mỗi ô (20x20 pixel)
constexpr int TOP_MARGIN = 40;     // Khoảng trống phía trên để vẽ điểm số

// Tự động tính chiều rộng và chiều cao màn hình
constexpr int SCREEN_W = 28 * CELL; // 560 pixel
constexpr int SCREEN_H = 31 * CELL + TOP_MARGIN; // 660 pixel

constexpr float PI_F = 3.14159265358979323846f;

#endif
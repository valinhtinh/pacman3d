# Pacman3D

A Pac-Man clone written in C++ with SDL2

## Overview

Guide Pac-Man through a maze, eating pellets while dodging four ghosts
(Blinky, Pinky, Inky, Clyde), each with its own chase behavior. Power
pellets turn the ghosts fearful and edible for a limited time. Includes
lives, scoring, a persisted high score list, and sound effects.

## Controls

| Key | Action |
|---|---|
| Arrow keys | Move Pac-Man |
| Enter | Start / restart |
| Space | Return to menu (after game over) |
| Esc | Quit |

## Technology

- **Language:** C++
- **Graphics / Input / Audio:** SDL2, SDL2_image, SDL2_mixer, SDL2_ttf
- **Build:** plain GNU Makefiles (`Makefile` for Windows, `Makefile-linux`
  for Linux) — no CMake

## Install & Run

### Windows (MinGW-w64)

Prebuilt SDL2 libraries and DLLs are already included in the repo, so no
extra install is needed besides a MinGW-w64 toolchain.

```bash
mingw32-make
main.exe
```

### Linux

Install the SDL2 development packages first, e.g. on Ubuntu/Debian:

```bash
sudo apt install build-essential libsdl2-dev libsdl2-image-dev \
                  libsdl2-mixer-dev libsdl2-ttf-dev pkg-config
```

Then build and run:

```bash
make -f Makefile-linux
./main
```

Run the binary from the repository root — it loads assets via the
relative path `assets/`.



## License

GNU General Public License v3 (see `LICENSE`).

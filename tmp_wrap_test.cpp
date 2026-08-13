#include <iostream>
#include "Maze.h"
int main() {
    Maze maze;
    maze.build();
    std::cout << maze.wrapCol(-1) << " " << maze.wrapCol(28) << "\n";
    std::cout << maze.pacWalkable(-1, 14) << " " << maze.pacWalkable(28, 14) << "\n";
    return 0;
}

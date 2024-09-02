#include <SFML/Graphics.hpp>
#include <iostream>

#include "model/game.hpp"

int main() {
    std::cout << "Skydiver-ai (by github.com/ivansansao) " << std::endl;
    std::cout << "c++ version: " << __cplusplus << std::endl;
    std::cout << std::endl;
    Game *game = new Game();
    game->run();
    delete game;
    game = nullptr;

    return EXIT_SUCCESS;
}
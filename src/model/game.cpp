#include "game.hpp"

#include <bits/stdc++.h>
#include <dirent.h>

#include <cmath>
#include <random>

#include "iostream"
#include "tools.hpp"

using namespace std;

Game::Game() {
    sf::ContextSettings settings;
    settings.depthBits = 0;          // Desativar o buffer de profundidade se não for necessário
    settings.stencilBits = 0;        // Desativar o buffer de stencil se não for necessário
    settings.antialiasingLevel = 0;  // Manter o antialiasing desativado
    settings.majorVersion = 2;       // Usar uma versão mais baixa do OpenGL
    settings.minorVersion = 1;

    window.create(sf::VideoMode(1600, 900), "Skydiver-ai @ivansansao", sf::Style::Titlebar | sf::Style::Close, settings);
    window.setVerticalSyncEnabled(false);  // Don't allow more FPS than your monitor support.
    window.setFramerateLimit(60);          // There is a relation between framerate and setVerticalSyncEnabled.
    window.setPosition(sf::Vector2i(0, 0));
    view.reset(sf::FloatRect(0.f, 0.f, 1600, 900));
    window.setView(this->view);
    window.setMouseCursorVisible(true);

    Tools::configure();

    font_roboto.loadFromFile("./src/asset/fonts/RobotoFlex-Regular.ttf");

    scenario.init(1, 0.5f, "./src/asset/image/scenario.png", sf::IntRect(0, 0, 1600, 900), true, 0, 0);

    // Configurando o gerador de números aleatórios
    // std::random_device rd;                            // Obter uma semente para o gerador
    // std::mt19937 gen(rd());                           // Gerador de números aleatórios
    // std::uniform_int_distribution<> dist(100, 1500);  // Distribuição
    // std::uniform_int_distribution<> disty(64, 200);   // Distribuição

    skydivers.clear();
    for (int i{}; i < 1; ++i) {
        Skydiver* skydiver = new Skydiver();
        // int x = dist(gen);
        // int y = disty(gen);
        // skydiver->set_position(x, (float)y);
        skydivers.push_back(skydiver);
    }
}

enum menuopcs { Play,
                Exit };

void Game::play() {
    window.clear(sf::Color(255, 255, 255, 255));

    scenario.draw(0, 0, &window);
    plane.update();
    plane.draw(&window);

    for (auto& skydiver : skydivers) {
        skydiver->think(plane);
        skydiver->update(plane);
        skydiver->draw(&window);
    }
    window.display();
}

void Game::close() {
    window.close();
};

void Game::loop_events() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        } else if (event.type == sf::Event::KeyReleased) {
            key_released = true;
        }
    }
}

void Game::run() {
    while (window.isOpen()) {
        this->loop_events();
        if (this->page == pages::GAME_PLAY) {
            this->play();
        }
    }
}
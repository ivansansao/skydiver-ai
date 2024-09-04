#include "game.hpp"

#include <bits/stdc++.h>
#include <dirent.h>

#include <cmath>
#include <random>

#include "iostream"

using namespace std;

Game::Game() {
    sf::ContextSettings settings;
    settings.depthBits = 0;          // Desativar o buffer de profundidade se não for necessário
    settings.stencilBits = 0;        // Desativar o buffer de stencil se não for necessário
    settings.antialiasingLevel = 0;  // Manter o antialiasing desativado
    settings.majorVersion = 2;       // Usar uma versão mais baixa do OpenGL
    settings.minorVersion = 1;

    window.create(sf::VideoMode(1600, 900), "Skydiver-ai @ivansansao", sf::Style::Titlebar | sf::Style::Close, settings);
    window.setVerticalSyncEnabled(true);  // Don't allow more FPS than your monitor support.
    window.setFramerateLimit(60);         // There is a relation between framerate and setVerticalSyncEnabled.
    window.setPosition(sf::Vector2i(0, 0));
    view.reset(sf::FloatRect(0.f, 0.f, 1600, 900));
    window.setView(this->view);
    window.setMouseCursorVisible(true);

    Tools::configure();

    font_roboto.loadFromFile("./src/asset/fonts/RobotoFlex-Regular.ttf");
    font_spacemono_regular.loadFromFile("./src/asset/fonts/SpaceMono-Regular.ttf");

    scenario.init(1, 0.5f, "./src/asset/image/scenario.png", sf::IntRect(0, 0, 1600, 900), true, 0, 0);

    // Configurando o gerador de números aleatórios
    // std::random_device rd;                            // Obter uma semente para o gerador
    // std::mt19937 gen(rd());                           // Gerador de números aleatórios
    // std::uniform_int_distribution<> dist(100, 1500);  // Distribuição
    // std::uniform_int_distribution<> disty(64, 200);   // Distribuição

    skydivers.clear();
    for (int i{}; i < 60; ++i) {
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
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
        paused = true;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
        paused = false;
    }
    window.clear(sf::Color(255, 255, 255, 255));

    scenario.draw(0, 0, &window);

    if (!paused) plane.update();
    plane.draw(&window);
    if (!paused) boat.update();
    boat.draw(&window);

    uint8_t onPlane = 0;
    uint8_t onAir = 0;
    uint8_t onBoat = 0;
    uint8_t died = 0;
    uint8_t sdTotal = 0;

    for (auto& skydiver : skydivers) {
        if (!paused) skydiver->think(plane, boat);
        if (!paused) skydiver->update(plane);
        skydiver->draw(&window, boat);

        if (skydiver->state == skydiver->State::ON_PLANE) onPlane++;
        if (skydiver->state == skydiver->State::ON_AIR) onAir++;
        if (skydiver->state == skydiver->State::ON_BOAT) onBoat++;
        if (skydiver->state == skydiver->State::DIED) died++;
        sdTotal++;
    }

    Tools::say(&window, "TOTAL: " + to_string(sdTotal), 4, 12 * 1);
    Tools::say(&window, "ON PLANE: " + to_string(onPlane), 4, 12 * 2);
    Tools::say(&window, "ON AIR: " + to_string(onAir), 4, 12 * 3);
    Tools::say(&window, "ON BOAT: " + to_string(onBoat), 4, 12 * 4);
    Tools::say(&window, "DIED: " + to_string(died), 4, 12 * 5);
    Tools::say(&window, "PLAY TIMER: " + to_string(playTimer), 4, 12 * 6);

    window.display();

    if (!paused) frameCount++;
    if (frameCount > 100000) frameCount = 0;

    if (playTimer > 60) {
        playTimer = 0;
        plane.reset_position();
        boat.reset_position();
        skydivers.clear();

        for (int i{}; i < 100; ++i) {
            Skydiver* skydiver = new Skydiver();
            skydivers.push_back(skydiver);
        }
    } else {
        if (!paused) playTimer += 0.02;
    }
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
#include "game.hpp"

#include <bits/stdc++.h>
#include <dirent.h>

#include <cmath>

#include "iostream"
#include "tools.hpp"

using namespace std;

Game::Game() {
    // window.create(sf::VideoMode::getDesktopMode(), "Skydiver-ai", sf::Style::Fullscreen);
    window.create(sf::VideoMode(1280, 736), "Skydiver-ai", sf::Style::Titlebar | sf::Style::Close);
    window.setVerticalSyncEnabled(true);  // Don't allow more FPS than your monitor support.
    window.setFramerateLimit(60);         // There is a relation between framerate and setVerticalSyncEnabled.

    window.setPosition(sf::Vector2i(0, 0));
    view.reset(sf::FloatRect(0.f, 0.f, 1280.f, 736.f));

    font_roboto.loadFromFile("./src/asset/fonts/RobotoFlex-Regular.ttf");
    window.setMouseCursorVisible(true);
}

enum menuopcs { Play,
                Exit };

void Game::play() {
    std::stringstream ss;

    view.reset(sf::FloatRect(0.f, 0.f, 1280, 736.f));
    window.setView(this->view);
    window.clear(sf::Color(255, 255, 255, 80));

    skydiver.update();
    skydiver.draw(&window);
    window.display();
}

void Game::close() {
    window.close();
};
bool Game::is_fullscreen() {
    return window.getSize().x == sf::VideoMode::getDesktopMode().width;
}

void Game::loop_events() {
    sf::Event event;
    sf::Clock clock;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::KeyReleased || event.type == sf::Event::JoystickButtonReleased) {
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
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

    scenario.init(1, 0.5f, "./src/asset/image/scenario.png", sf::IntRect(0, 0, 1600, 900), true, 0, 0, false);

    // Configurando o gerador de números aleatórios
    // std::random_device rd;                            // Obter uma semente para o gerador
    // std::mt19937 gen(rd());                           // Gerador de números aleatórios
    // std::uniform_int_distribution<> dist(100, 1500);  // Distribuição
    // std::uniform_int_distribution<> disty(64, 200);   // Distribuição

    qtd_skydivers = 40;

    skydivers.clear();

    // Skydiver* skydiverTest = new Skydiver();
    // std::cout << "sky TEST " << std::endl;
    // skydiverTest->mind.setWeights("-0.231753,0.212437,0.363693,0.128851,0.131988,0.98923,0.0639589,-0.0114211,-0.0792374,-0.307554,-0.749453,0.023334,0.366762,0.177239,0.373656,0.191526,-0.299836,-0.00969508,-0.384004,-0.396488,-0.346737,0.538566,0.494882,0.25105,0.0949097,0.785266,0.645206,-0.110854,0.35558,-0.952787,-0.505122,-0.128861,-0.357298,0.258899,-0.0194009,0.00668131,0.455153,0.351084,0.0723921,0.365679,-0.454371,0.512759,-0.0404545,0.202803,-0.531711,0.0443609,-0.511245,-0.841791,-0.223057,0.390155,0.27952,0.391405,-0.0579165,0.684708,-0.655377,0.181777");
    // skydivers.push_back(skydiverTest);

    // std::cout << "sky TEST getweights " << skydiverTest->mind.getWeights() << std::endl;

    for (int i{}; i < qtd_skydivers; ++i) {
        // int x = dist(gen);
        // int y = disty(gen);
        // skydiver->set_position(x, (float)y);
        // skydivers.push_back(Skydiver());
        Skydiver* skydiver = new Skydiver();
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
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
        window.setVerticalSyncEnabled(false);
        window.setFramerateLimit(999);
    }

    // UPDATE

    if (!paused) plane.update();
    if (!paused) boat.update();

    uint8_t onPlane = 0;
    uint8_t onAir = 0;
    uint8_t onBoat = 0;
    uint8_t died = 0;
    uint8_t sdTotal = 0;

    for (auto& skydiver : skydivers) {
        if (!paused) skydiver->think(plane, boat);
        if (!paused) skydiver->update(plane, boat);
        if (skydiver->state == skydiver->State::ON_PLANE) onPlane++;
        if (skydiver->state == skydiver->State::ON_AIR) onAir++;
        if (skydiver->state == skydiver->State::ON_BOAT) onBoat++;
        if (skydiver->died) died++;
        sdTotal++;
    }

    // DRAW

    window.clear(sf::Color(255, 255, 255, 255));
    scenario.draw(0, 0, &window);
    plane.draw(&window);
    boat.draw(&window);
    for (auto& skydiver : skydivers) {
        skydiver->draw(&window, boat);
    }
    // Tools::say(&window, "TOTAL: " + to_string(sdTotal), 4, 12 * 1);
    // Tools::say(&window, "ON PLANE: " + to_string(onPlane), 4, 12 * 2);
    // Tools::say(&window, "ON AIR: " + to_string(onAir), 4, 12 * 3);
    // Tools::say(&window, "ON BOAT: " + to_string(onBoat), 4, 12 * 4);
    // Tools::say(&window, "DIED: " + to_string(died), 4, 12 * 5);
    // Tools::say(&window, "PLAY TIMER: " + to_string(playTimer), 4, 12 * 6);
    window.display();

    if (!paused) frameCount++;
    if (frameCount > 100000) frameCount = 0;

    if (playTimer > 60) {
        playTimer = 0;

        // Get better score
        int better = 0;
        Skydiver* betterSkydiver = new Skydiver();
        for (auto skydiver : skydivers) {
            if (skydiver->getScore() > better) {
                better = skydiver->getScore();
                betterSkydiver = skydiver;
            }
        }

        plane.reset_position();
        boat.reset_position();
        skydivers.clear();

        // Add beter to new round
        if (better > 10) {
            Skydiver* child = new Skydiver();
            child->mind.setWeights(betterSkydiver->mind.getWeights());
            skydivers.push_back(child);

            std::cout << "Weights of better: " << betterSkydiver->mind.getWeights() << std::endl;
            std::cout << "Weights of child.: " << child->mind.getWeights() << std::endl;
        }

        for (int i{}; i < (qtd_skydivers - 1); ++i) {
            Skydiver* skydiver = new Skydiver();
            if (better > 10) {
                std::cout << "Has better: " << better << std::endl;
                skydiver->mind.setWeights(betterSkydiver->mind.getWeights());
                skydiver->mind.mutate(10);
            }
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
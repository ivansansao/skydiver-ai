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
    setWindowIcon(&window);
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

    lastBetterSkydiver = new Skydiver();
    qtd_skydivers = 40;

    skydivers.clear();
    lastBetterWeight = loadWeights();

    if (lastBetterWeight.length() > 0) {
        std::cout << "Loading weights...";
        Skydiver* skydiver = new Skydiver();
        skydivers.push_back(skydiver);
        skydiver->mind.setWeights(lastBetterWeight);

        std::cout << " Weights:" << skydiver->mind.getWeights() << std::endl;
        std::cout << " done!" << std::endl;
    }

    for (int i{}; i < qtd_skydivers; ++i) {
        Skydiver* skydiver = new Skydiver();
        skydivers.push_back(skydiver);
    }
}

enum menuopcs { Play,
                Exit };

void Game::play() {
    // UPDATE

    uint8_t onPlane = 0;
    uint8_t onAir = 0;
    uint8_t onBoat = 0;
    uint8_t died = 0;
    uint8_t landedCount = 0;
    uint8_t sdTotal = 0;

    if (!paused) {
        plane.update();
        boat.update();
        for (auto& skydiver : skydivers) {
            skydiver->think(plane, boat);
            skydiver->update(plane, boat);
            if (skydiver->state == skydiver->State::ON_PLANE) onPlane++;
            if (skydiver->state == skydiver->State::ON_AIR) onAir++;
            if (skydiver->state == skydiver->State::ON_BOAT) onBoat++;
            if (skydiver->died) died++;
            if (skydiver->landed) landedCount++;
            sdTotal++;
        }
    }

    if (playTimer > 60 && !hasOnScreenAir()) {
        playTimer = 0;
        round++;

        // Get better score
        Skydiver* betterSkydiver = new Skydiver();

        if (landedCount) {
            for (auto skydiver : skydivers) {
                if (skydiver->getScore() > betterSkydiver->getScore()) {
                    betterSkydiver = skydiver;
                    this->lastBetterSkydiver = skydiver;
                }
            }
        }

        plane.start_round();
        boat.reset_position();
        boat.pos.left = 735 + (Tools::getRand() * 220);
        skydivers.clear();

        // Add beter to new round
        if (betterSkydiver->getScore() > 0) {
            this->lastBetterWeight = betterSkydiver->mind.getWeights();
            Skydiver* child = new Skydiver();
            child->mind.setWeights(lastBetterWeight);
            skydivers.push_back(child);
            std::cout << "New better Weights: " << lastBetterWeight << std::endl;
            std::cout << "Has better (Score): " << betterSkydiver->getScore() << std::endl;
            saveWeights(lastBetterWeight);
        }

        for (int i{}; i < qtd_skydivers; ++i) {
            Skydiver* skydiver = new Skydiver();
            if (betterSkydiver->getScore() > 0 && i > (qtd_skydivers * 0.3)) {  // Add 30% without mutate.
                skydiver->mind.setWeights(this->lastBetterWeight);
            }
            skydiver->mind.mutate(i);
            skydivers.push_back(skydiver);
        }
    }

    // DRAW

    window.clear(sf::Color(255, 255, 255, 255));
    scenario.draw(0, 0, &window);
    plane.draw(&window);
    boat.draw(&window);
    for (auto& skydiver : skydivers) {
        skydiver->draw(&window, boat);
    }

    if (show_information) {
        std::string info = "";
        info += "ROUND.......: " + to_string(round);
        info += "\n";
        info += "\nTOTAL.......: " + to_string(sdTotal);
        info += "\nON PLANE....: " + to_string(onPlane);
        info += "\nON AIR......: " + to_string(onAir);
        info += "\nON BOAT.....: " + to_string(onBoat);
        info += "\nLANDED......: " + to_string(landedCount);
        info += "\nDIED........: " + to_string(died);
        info += "\nPLAY TIMER..: " + to_string(playTimer);
        info += "\n";
        info += "\nLAST BEST SKYDIVER";
        info += "\n";
        info += "\nGRADE: Landing place ..: " + to_string(lastBetterSkydiver->grade_landing_place);
        info += "\nGRADE: Landing softly..: " + to_string(lastBetterSkydiver->grade_landing_softly);
        info += "\nGRADE: Max vel right...: " + to_string(lastBetterSkydiver->grade_max_velocity_right);
        info += "\nGRADE: Max vel left....: " + to_string(lastBetterSkydiver->grade_max_velocity_left);
        info += "\nGRADE: Direc changes...: " + to_string(lastBetterSkydiver->grade_direction_changes);
        info += "\nSCORE..................: " + to_string(lastBetterSkydiver->getScore());
        info += "\n";
        info += "\nOTHER";
        info += std::string("\nSYNC...................: ") + (syncronism ? std::string("ON") : std::string("OFF"));

        Tools::say(&window, info, 10, 8);
    }
    window.display();

    // END DRAW

    if (!paused) {
        playTimer += 0.02;
        frameCount++;
        if (frameCount > 100000) frameCount = 0;
    }
}

bool Game::hasOnScreenAir() {
    for (auto& skydiver : skydivers) {
        if (skydiver->state == skydiver->State::ON_AIR) {
            if (skydiver->pos.left > 0 && skydiver->pos.left + skydiver->pos.width < 1600) {
                return true;
            }
        }
    }

    return false;
}

void Game::saveWeights(std::string weights) {
    std::ofstream outFile("weights.txt");
    if (outFile.is_open()) {
        outFile << weights;
        outFile.close();
    } else {
        std::cerr << "Erro opening weights file to save!" << std::endl;
    }
}
std::string Game::loadWeights() {
    std::ifstream inFile("weights.txt");
    std::string weights;
    if (inFile.is_open()) {
        std::getline(inFile, weights);
        inFile.close();
    } else {
        std::cerr << "Don't used weights.txt file!" << std::endl;
    }

    return weights;
}

void Game::setWindowIcon(sf::RenderWindow* w) {
    sf::Image icon;
    if (!icon.loadFromFile("./src/asset/image/icon.png")) {
        std::cerr << "Erro ao carregar o ícone." << std::endl;
    } else {
        w->setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
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
        } else if (event.type == sf::Event::GainedFocus) {
            window_has_focus = true;
        } else if (event.type == sf::Event::LostFocus) {
            window_has_focus = false;
        } else if (event.type == sf::Event::KeyReleased) {
            key_released = true;
        } else if (event.type == sf::Event::KeyPressed) {
            if (window_has_focus) {
                if (event.key.code == sf::Keyboard::I) {
                    show_information = !show_information;  // Inverte o estado da variável
                } else if (event.key.code == sf::Keyboard::F7) {
                    paused = !paused;
                } else if (event.key.code == sf::Keyboard::F8) {
                    syncronism = !syncronism;
                    window.setVerticalSyncEnabled(syncronism);
                    if (syncronism)
                        window.setFramerateLimit(60);
                    else
                        window.setFramerateLimit(0);
                }
            }
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
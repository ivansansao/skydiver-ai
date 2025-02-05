#include "game.hpp"

#include <bits/stdc++.h>
#include <dirent.h>

#include <cmath>
#include <filesystem>
#include <random>
#include <unordered_map>

#include "iostream"

using namespace std;

Game::Game() {
    Config config = loadConfig("config.txt");

    round = config.round.has_value() ? config.round.value() : round;
    plane.velocity.x = config.planeVelocityX.has_value() ? config.planeVelocityX.value() : plane.velocity.x;
    boat.velocity.x = config.boatVelocityX.has_value() ? config.boatVelocityX.value() : boat.velocity.x;
    qtd_skydivers = config.qtdSkydivers.has_value() ? config.qtdSkydivers.value() : qtd_skydivers;
    fullscreen = config.fullscreen.has_value() ? config.fullscreen.value() : fullscreen;

    plane.set_start_pos();

    sf::ContextSettings settings;
    settings.depthBits = 0;
    settings.stencilBits = 0;
    settings.antialiasingLevel = 8;
    settings.majorVersion = 2;
    settings.minorVersion = 1;

    if (fullscreen) {
        window.create(sf::VideoMode::getDesktopMode(), "Skydivers", sf::Style::Fullscreen, settings);
    } else {
        window.create(sf::VideoMode(1600, 900), "Skydiver-ai @ivansansao", sf::Style::Titlebar | sf::Style::Close, settings);
    }
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

    skydivers.clear();

    lastBetterSkydiver = new Skydiver(0, qtd_skydivers);
    lastBetterSkydiver->mind.setWeights(loadWeights());
    lastBetterSkydiver->mind.setBias(loadBiases());

    for (int i{}; i < qtd_skydivers; ++i) {
        Skydiver* skydiver = new Skydiver(i, qtd_skydivers);
        skydiver->mind.setWeights(lastBetterSkydiver->mind.getWeights());
        skydiver->mind.setBias(lastBetterSkydiver->mind.getBias());
        if (i > 0) skydiver->mind.mutate(i, true);
        skydivers.push_back(skydiver);
    }
}

enum menuopcs { Play,
                Exit };

void Game::play() {
    // UPDATE

    onPlane = 0;
    onAir = 0;
    onBoat = 0;
    died = 0;
    landedCount = 0;
    sdTotal = 0;

    if (!paused) {
        // Update Plane

        if (plane.on) {
            plane.update();
            if (!plane.on) {
                for (auto& skydiver : skydivers) {
                    if (skydiver->state == skydiver->State::ON_PLANE) {
                        skydiver->died = true;
                    }
                }
            }
        }

        // Update Boat

        boat.update();

        // Update Skydiver

        for (auto& skydiver : skydivers) {
            if (skydiver->died) {
                died++;
                if (skydiver->state == skydiver->State::ON_BOAT) {
                    skydiver->update(plane, boat, positionCounter);
                }
            } else {
                if (frameCount % 8 == 0) {  // Reaction time each ~0,133s (133ms) means can react 7,5 times per seconds.
                    if (skydiver->state != skydiver->State::ON_BOAT) {
                        skydiver->think(plane, boat, bootSkydivers);
                    }
                }
                skydiver->doAction();
                skydiver->update(plane, boat, positionCounter);
            }

            if (skydiver->state == skydiver->State::ON_PLANE)
                onPlane++;
            else if (skydiver->state == skydiver->State::ON_AIR)
                onAir++;
            else if (skydiver->state == skydiver->State::ON_BOAT)
                onBoat++;

            if (skydiver->landed) {
                landedCount++;
                positionCounter = positionCounter > skydiver->position ? positionCounter : skydiver->position;
            };
            sdTotal++;
        }
    }

    // Finish
    if (playTimer > 60 || (died && died == sdTotal)) {
        // Get better score

        if (landedCount) {
            Skydiver* last = skydivers[0];
            for (size_t i = 1; i < skydivers.size(); ++i) {
                if (skydivers[i]->getScore() > last->getScore()) {
                    last = skydivers[i];
                }
            }

            // It is good to choose the last best round because boat move of place.
            if (last->getScore() > 0) {
                lastBetterSkydiver = last;
                lastBetterSkydiver->round = round;
                saveWeights(lastBetterSkydiver->mind.getWeights());
                saveBiases(lastBetterSkydiver->mind.getBias());
                saveScore(lastBetterSkydiver->getScore());
            }

            plane.reverse_direction(round % 2 == 0);
            boat.velocity.x *= -1;
        }

        playTimer = 0;
        round++;
        positionCounter = 0;

        plane.start_round();
        boat.start_position_random();

        skydivers.clear();
        for (int i{}; i < qtd_skydivers; ++i) {
            Skydiver* skydiver = new Skydiver(i, qtd_skydivers);
            skydiver->mind.setWeights(lastBetterSkydiver->mind.getWeights());
            skydiver->mind.setBias(lastBetterSkydiver->mind.getBias());

            if (i > 0) skydiver->mind.mutate(i, true);
            skydivers.push_back(skydiver);
        }

        this->bootSkydivers = !std::filesystem::exists("weights.txt");

        Config config = {round, boat.velocity.x, plane.velocity.x, qtd_skydivers, fullscreen};
        saveConfig(config, "config.txt");
    }

    // DRAW

    scenario.draw(0, 0, &window);
    plane.draw(&window);
    boat.draw(&window);
    for (auto& skydiver : skydivers) {
        skydiver->draw(&window, boat, show_information);
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
        info += "\nGRADE: Position........: " + to_string(lastBetterSkydiver->grade_position);
        info += "\nGRADE: Landing place...: " + to_string(lastBetterSkydiver->grade_landing_place);
        info += "\nGRADE: Landing softly..: " + to_string(lastBetterSkydiver->grade_landing_softly);
        info += "\nGRADE: Max vel right...: " + to_string((int)lastBetterSkydiver->grade_max_velocity_right);
        info += "\nGRADE: Max vel left....: " + to_string((int)lastBetterSkydiver->grade_max_velocity_left);
        // info += "\nGRADE: Time on air.....: " + to_string(lastBetterSkydiver->grade_time_on_air);
        info += "\nGRADE: Direc changes...: " + to_string(lastBetterSkydiver->grade_direction_changes);
        info += "\nGRADE: Used actions....: " + to_string(lastBetterSkydiver->grade_used_actions);
        info += "\n---------------------------------";
        info += "\nSCORE..................: " + to_string(lastBetterSkydiver->getScore());
        info += "\n";
        info += "\nOTHER";
        info += "\nROUND..................: " + to_string(lastBetterSkydiver->round);
        info += "\nBOOT SKYDIVERS.........: " + (this->bootSkydivers ? std::string("ON") : std::string("OFF"));
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
void Game::saveBiases(std::string biases) {
    std::ofstream outFile("biases.txt");
    if (outFile.is_open()) {
        outFile << biases;
        outFile.close();
    } else {
        std::cerr << "Erro opening biases file to save!" << std::endl;
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
std::string Game::loadBiases() {
    std::ifstream inFile("biases.txt");
    std::string biases;
    if (inFile.is_open()) {
        std::getline(inFile, biases);
        inFile.close();
    } else {
        std::cerr << "Don't used biases.txt file!" << std::endl;
    }

    return biases;
}
void Game::saveScore(unsigned int score) {
    std::ofstream outFile("score.txt");
    if (outFile.is_open()) {
        outFile << score;
        outFile.close();
    } else {
        std::cerr << "Erro opening weights file to save!" << std::endl;
    }
}
unsigned int Game::loadScore() {
    std::ifstream inFile("score.txt");
    std::string score = "";
    if (inFile.is_open()) {
        std::getline(inFile, score);
        inFile.close();
    } else {
        std::cerr << "Don't used weights.txt file!" << std::endl;
    }

    if (score.length() > 0) {
        return std::stoi(score);
    }
    return 0;
}

void Game::saveConfig(const Config& config, const std::string& arquivo) {
    std::ofstream outFile(arquivo);
    if (!outFile) {
        std::cerr << "Erro ao abrir o arquivo para escrita!\n";
        return;
    }

    outFile << "round=" << config.round.value() << "\n"
            << "boatVelocityX=" << config.boatVelocityX.value() << "\n"
            << "planeVelocityX=" << config.planeVelocityX.value() << "\n"
            << "qtdSkydivers=" << config.qtdSkydivers.value() << "\n"
            << "fullscreen=" << config.fullscreen.value() << "\n";

    outFile.close();
}

Config Game::loadConfig(const std::string& arquivo) {
    Config config;
    std::ifstream inFile(arquivo);
    if (!inFile) {
        std::cerr << "Erro ao abrir o arquivo para leitura!\n";
        return config;
    }

    std::string linha;
    std::unordered_map<std::string, std::string> configMap;

    while (std::getline(inFile, linha)) {
        std::istringstream ss(linha);
        std::string chave, valor;

        if (std::getline(ss, chave, '=') && std::getline(ss, valor)) {
            configMap[chave] = valor;
        }
    }

    // Converte os valores para os tipos corretos
    if (configMap.find("round") != configMap.end())
        config.round = std::stoi(configMap["round"]);
    if (configMap.find("boatVelocityX") != configMap.end())
        config.boatVelocityX = std::stof(configMap["boatVelocityX"]);
    if (configMap.find("planeVelocityX") != configMap.end())
        config.planeVelocityX = std::stof(configMap["planeVelocityX"]);
    if (configMap.find("qtdSkydivers") != configMap.end())
        config.qtdSkydivers = std::stoi(configMap["qtdSkydivers"]);
    if (configMap.find("fullscreen") != configMap.end())
        config.fullscreen = (configMap["fullscreen"] == "1");

    inFile.close();
    return config;
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
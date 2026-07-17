#include "game.hpp"

#include <bits/stdc++.h>
#include <dirent.h>

#include <cmath>
#include <filesystem>
#include <random>

#include "iostream"

using namespace std;

Game::Game() {
    config = loadConfig("config.txt");

    round = config.round.value_or(round);
    plane.velocity.x = config.planeVelocityX.value_or(plane.velocity.x);
    boat.velocity.x = config.boatVelocityX.value_or(boat.velocity.x);
    qtd_skydivers = config.qtdSkydivers.value_or(qtd_skydivers);
    fullscreen = config.fullscreen.value_or(fullscreen);

    plane.set_start_pos();

    sf::ContextSettings settings;
    settings.depthBits = 0;
    settings.stencilBits = 0;
    settings.antiAliasingLevel = 8;
    settings.majorVersion = 2;
    settings.minorVersion = 1;

    if (fullscreen) {
        window.create(sf::VideoMode::getDesktopMode(), "Skydivers", sf::State::Fullscreen, settings);
    } else {
        window.create(sf::VideoMode(sf::Vector2u(1600, 900)), "Skydiver-ai @ivansansao", sf::Style::Titlebar | sf::Style::Close, sf::State::Windowed, settings);
    }
    setWindowIcon(&window);
    window.setVerticalSyncEnabled(true);  // Don't allow more FPS than your monitor support.
    window.setFramerateLimit(60);         // There is a relation between framerate and setVerticalSyncEnabled.
    window.setPosition(sf::Vector2i(0, 0));
    view = sf::View(sf::FloatRect({0.f, 0.f}, {1600.f, 900.f}));
    window.setView(this->view);
    window.setMouseCursorVisible(true);

    Tools::configure();

    if (!font_roboto.openFromFile("./src/asset/fonts/RobotoFlex-Regular.ttf")) {
        std::cerr << "Erro ao carregar a fonte RobotoFlex-Regular.ttf." << std::endl;
    }
    if (!font_spacemono_regular.openFromFile("./src/asset/fonts/SpaceMono-Regular.ttf")) {
        std::cerr << "Erro ao carregar a fonte SpaceMono-Regular.ttf." << std::endl;
    }

    scenario.init(1, 0.5f, "./src/asset/image/scenario.png", sf::IntRect({0, 0}, {1600, 900}), true, 0, 0, false);

    skydivers.clear();

    lastBetterSkydiver = new Skydiver(0, qtd_skydivers, config.hiddenLayers.value_or(1), config.layersSize.value_or(14));
    lastBetterSkydiver->mind.setWeights(config.weights.value_or(""));
    lastBetterSkydiver->mind.setBias(config.biases.value_or(""));
    lastBetterSkydiver->score = config.score.has_value() ? config.score.value() : 0.f;

    for (int i{}; i < qtd_skydivers; ++i) {
        Skydiver* skydiver = new Skydiver(i, qtd_skydivers, config.hiddenLayers.value_or(1), config.layersSize.value_or(14));
        skydiver->mind.setWeights(lastBetterSkydiver->mind.getWeights());
        skydiver->mind.setBias(lastBetterSkydiver->mind.getBias());
        if (i > 0) skydiver->mind.mutate(i, true);
        skydivers.push_back(skydiver);
    }

    this->bootSkydivers = config.weights.value_or("") == "";
    sf::Clock roundClock;
    plane.start_round();
}

enum menuopcs { Play,
                Exit };

void Game::onLand() {
    if (config.commandOnLand.has_value()) {
        const int status = std::system(config.commandOnLand.value().c_str());
        if (status != 0) {
            std::cerr << "Erro ao executar commandOnLand. Status: " << status << std::endl;
        }
    }
}
void Game::play() {
    // UPDATE

    onPlane = 0;
    onAir = 0;
    onBoat = 0;
    died = 0;
    landedCount = 0;
    sdTotal = 0;

    const bool drawFrame = this->training ? (frameCount % config.drawInterval.value_or(1) == 0) : true;

    if (!paused) {
        // Update Plane

        if (plane.on) {
            plane.update();
        }

        // Update Boat

        boat.update(&window);

        if (drawing && drawFrame) {
            window.clear();
            scenario.draw(0, 0, &window);
            plane.draw(&window);
            boat.draw(&window);
        }

        // Think
        if (frameCount % 20 == 0) {
#pragma omp parallel for
            for (auto& skydiver : skydivers) {
                if (!skydiver->died) {
                    if (skydiver->state != skydiver->State::ON_BOAT) {
                        skydiver->think(plane, boat, bootSkydivers);
                    }
                }
            }
        }

        // Update/draw Skydiver

        for (auto& skydiver : skydivers) {
            if (skydiver->died) {
                died++;
                if (skydiver->state == skydiver->State::ON_BOAT) {
                    skydiver->update(plane, boat, positionCounter);
                }
            } else {
                // if (frameCount % 8 == 0) {  // Reaction time each ~0,133s (133ms) means can react 7,5 times per seconds.
                //     if (skydiver->state != skydiver->State::ON_BOAT) {
                //         skydiver->think(plane, boat, bootSkydivers);
                //     }
                // }
                skydiver->doAction();
                skydiver->update(plane, boat, positionCounter, this);

                // Eliminate skydivers that are out of screen e no final do round?
                if (!plane.on && playTimer > 10) {
                    if (skydiver->state == skydiver->State::ON_PLANE) {
                        skydiver->died = true;
                    }
                }
            }

            // PROCESS STATISTICS
            if (skydiver->state == skydiver->State::ON_PLANE)
                onPlane++;
            else if (skydiver->state == skydiver->State::ON_AIR)
                onAir++;
            else if (skydiver->state == skydiver->State::ON_BOAT)
                onBoat++;

            if (skydiver->landed) {
                landedCount++;
                positionCounter = positionCounter > skydiver->position ? positionCounter : skydiver->position;

                if (skydiver->isMaster()) {
                    this->endForTraining = true;
                }
            };
            sdTotal++;

            if (drawing && drawFrame) {
                // DRAW SKYDIVER IN THIS LOOP
                skydiver->draw(&window, boat, show_information);
            }
        }
    }

    if (training) {
        if (!this->endForTraining) this->endForTraining = training && (landedCount + died == sdTotal);
    }

    // Finish
    if (playTimer > 60 || (died && died == sdTotal) || this->endForTraining) {
        this->endForTraining = false;

        // Imprime o tempo que levou
        float roundTime = roundClock.getElapsedTime().asSeconds();  // Tempo decorrido
        roundClock.restart();

        // Save statistics
        std::string log = "Round " + to_string(round) + " lasted " + to_string(roundTime) + " seconds. Skydivers: " + to_string(qtd_skydivers) + " Training: " + Tools::onOff(training) + " Drawing: " + Tools::onOff(drawing) + " Syncronism: " + Tools::onOff(syncronism) + " Landed: " + to_string(landedCount);

        // Get better score

        Skydiver* last = skydivers[0];
        for (size_t i = 1; i < skydivers.size(); ++i) {
            if (skydivers[i]->getScore() > last->getScore()) {
                last = skydivers[i];
            }
        }

        // Keep master if he could land
        if (config.keepMaster) {
            for (auto& skydiver : skydivers) {
                if (skydiver->isMaster() && skydiver->landed) {
                    last = skydiver;
                    break;
                }
            }
        }

        // It is good to choose the last best round because boat move of place.
        lastBetterSkydiver = last;
        lastBetterSkydiver->round = round;
        config.weights = lastBetterSkydiver->mind.getWeights();
        config.biases = lastBetterSkydiver->mind.getBias();

        if (landedCount) {
            plane.reverse_direction(round % 2 == 0);
            boat.velocity.x *= -1;
        }

        log = log + " Mutations: " + to_string(lastBetterSkydiver->mind.mutated);

        playTimer = 0;
        round++;
        positionCounter = 0;

        plane.start_round();
        boat.start_position_random(&window);

        // // A tried this but segment faults
        // for (auto* skydiver : skydivers) {
        //     delete skydiver;
        // }
        skydivers.clear();
        for (int i{}; i < qtd_skydivers; ++i) {
            Skydiver* skydiver = new Skydiver(i, qtd_skydivers, config.hiddenLayers.value_or(1), config.layersSize.value_or(14));
            skydiver->mind.setWeights(lastBetterSkydiver->mind.getWeights());
            skydiver->mind.setBias(lastBetterSkydiver->mind.getBias());

            if (i > 0) skydiver->mind.mutate(skydiver->mind.countWeights(), true);
            skydivers.push_back(skydiver);
        }

        if (this->bootSkydivers) {
            if (landedCount) {
                this->bootSkydivers = false;
            }
        }

        Config pconfig = {round, boat.velocity.x, plane.velocity.x, qtd_skydivers, fullscreen, config.drawInterval.value_or(1), lastBetterSkydiver->getScore(), config.commandOnLand.value_or(""), config.hiddenLayers.value_or(1), config.layersSize.value_or(14), config.keepMaster.value_or(0), config.weights.value_or(""), config.biases.value_or("")};
        saveConfig(pconfig, "config.txt");

        log = log + " Score: " + Tools::formatDouble(lastBetterSkydiver->getScore(), 2);
        log = log + " Actions: " + lastBetterSkydiver->usedActions;

        Tools::fileLog(log, "log.txt");
    }

    // DRAW
    if (drawing && drawFrame) {
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
            // info += "\nGRADE: Position........: " + to_string(lastBetterSkydiver->grade_position);
            // info += "\nGRADE: Max vel right...: " + to_string((int)lastBetterSkydiver->grade_max_velocity_right);
            // info += "\nGRADE: Max vel left....: " + to_string((int)lastBetterSkydiver->grade_max_velocity_left);
            // info += "\nGRADE: Time on air.....: " + to_string(lastBetterSkydiver->grade_time_on_air);
            // info += "\nGRADE: Direc changes........: " + Tools::formatDouble(lastBetterSkydiver->grade_direction_changes, 4);
            info += "\nGRADE: Horizontal velocity..: " + Tools::formatDouble(lastBetterSkydiver->grade_horizontal_velocity, 4);
            info += "\nGRADE: Vertical velocity....: " + Tools::formatDouble(lastBetterSkydiver->grade_vertical_velocity, 4);
            info += "\nGRADE: Landing place........: " + Tools::formatDouble(lastBetterSkydiver->grade_landing_place, 4);
            info += "\nGRADE: Used actions.........: " + Tools::formatDouble(lastBetterSkydiver->grade_used_actions, 4);
            info += "\nGRADE: Landed...............: " + Tools::formatDouble(lastBetterSkydiver->landed ? 1000 : 0, 4);
            info += "\n--------------------------------------";
            info += "\nSCORE.......................: " + Tools::formatDouble(lastBetterSkydiver->getScore(), 4);
            info += "\n";
            info += "\nOTHER";
            info += "\nROUND.......................: " + to_string(lastBetterSkydiver->round);
            info += "\nBOOT SKYDIVERS..............: " + Tools::onOff(this->bootSkydivers);
            info += "\nPAUSE............(F7).......: " + Tools::onOff(this->paused);
            info += "\nSYNC.............(F8).......: " + Tools::onOff(this->syncronism);
            info += "\nMOSTRAR..........(F9).......: " + Tools::onOff(this->drawing);
            info += "\nTRAINING.........(F10)......: " + Tools::onOff(this->training);

            Tools::say(&window, info, 10, 8);
        }

        window.display();
    }

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
            if (skydiver->pos.position.x > 0 && skydiver->pos.position.x + skydiver->pos.size.x < 1600) {
                return true;
            }
        }
    }

    return false;
}

void Game::saveConfig(const Config& pconfig, const std::string& arquivo) {
    std::ofstream outFile(arquivo);
    if (!outFile) {
        std::cerr << "Erro ao abrir o arquivo para escrita!\n";
        return;
    }

    outFile << "round=" << pconfig.round.value() << "\n"
            << "boatVelocityX=" << pconfig.boatVelocityX.value() << "\n"
            << "planeVelocityX=" << pconfig.planeVelocityX.value() << "\n"
            << "qtdSkydivers=" << pconfig.qtdSkydivers.value() << "\n"
            << "fullscreen=" << pconfig.fullscreen.value() << "\n"
            << "drawInterval=" << pconfig.drawInterval.value() << "\n"
            << "score=" << Tools::formatDouble(pconfig.score.value(), 2) << "\n"
            << "commandOnLand=" << pconfig.commandOnLand.value() << "\n"
            << "hiddenLayers=" << pconfig.hiddenLayers.value() << "\n"
            << "layersSize=" << pconfig.layersSize.value() << "\n"
            << "keepMaster=" << pconfig.keepMaster.value() << "\n"
            << "weights=" << pconfig.weights.value() << "\n"
            << "biases=" << pconfig.biases.value() << "\n";

    outFile.close();
}

Config Game::loadConfig(const std::string& arquivo) {
    Config config;
    std::ifstream inFile(arquivo);
    if (!inFile) {
        std::cerr << "Erro ao abrir o arquivo config.txt para leitura!\n";
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
    if (configMap.find("drawInterval") != configMap.end())
        config.drawInterval = std::stoi(configMap["drawInterval"]);
    if (configMap.find("score") != configMap.end())
        config.score = std::stof(configMap["score"]);
    if (configMap.find("commandOnLand") != configMap.end())
        config.commandOnLand = configMap["commandOnLand"];
    if (configMap.find("hiddenLayers") != configMap.end())
        config.hiddenLayers = std::stoi(configMap["hiddenLayers"]);
    if (configMap.find("layersSize") != configMap.end())
        config.layersSize = std::stoi(configMap["layersSize"]);
    if (configMap.find("keepMaster") != configMap.end())
        config.keepMaster = (configMap["keepMaster"] == "1");
    if (configMap.find("weights") != configMap.end())
        config.weights = configMap["weights"];
    if (configMap.find("biases") != configMap.end())
        config.biases = configMap["biases"];

    inFile.close();
    return config;
}

void Game::setWindowIcon(sf::RenderWindow* w) {
    sf::Image icon;
    if (!icon.loadFromFile("./src/asset/image/icon.png")) {
        std::cerr << "Erro ao carregar o ícone." << std::endl;
    } else {
        w->setIcon(icon);
    }
}

void Game::close() {
    window.close();
};

void Game::loop_events() {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        } else if (event->is<sf::Event::FocusGained>()) {
            window_has_focus = true;
        } else if (event->is<sf::Event::FocusLost>()) {
            window_has_focus = false;
        } else if (event->is<sf::Event::KeyReleased>()) {
            key_released = true;
        } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (window_has_focus) {
                if (keyPressed->code == sf::Keyboard::Key::I) {
                    show_information = !show_information;  // Inverte o estado da variável
                } else if (keyPressed->code == sf::Keyboard::Key::F7) {
                    paused = !paused;
                } else if (keyPressed->code == sf::Keyboard::Key::F8) {
                    syncronism = !syncronism;
                    if (syncronism) {
                        window.setVerticalSyncEnabled(true);
                        window.setFramerateLimit(0);
                    } else {
                        window.setVerticalSyncEnabled(false);
                        window.setFramerateLimit(60);
                    }
                } else if (keyPressed->code == sf::Keyboard::Key::F9) {
                    drawing = !drawing;
                } else if (keyPressed->code == sf::Keyboard::Key::F10) {
                    training = !training;
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
